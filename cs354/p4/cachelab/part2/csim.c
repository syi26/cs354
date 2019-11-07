/*

 * CS login:
 * Section(s):
 *
 * csim.c - A cache simulator that can replay traces from Valgrind
 *     and output statistics such as number of hits, misses, and
 *     evictions.  The replacement policy is LRU.
 *
 * Implementation and assumptions:
 *  1. Each load/store can cause at most one cache miss plus a possible eviction.
 *  2. Instruction loads (I) are ignored.
 *  3. Data modify (M) is treated as a load followed by a store to the same
 *  address. Hence, an M operation can result in two cache hits, or a miss and a
 *  hit plus a possible eviction.
 *
 * The function printSummary() is given to print output.
 * Please use this function to print the number of hits, misses and evictions.
 * This is crucial for the driver to evaluate your work. 
 */

#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/****************************************************************************/
/***** DO NOT MODIFY THESE VARIABLE NAMES ***********************************/

/* Globals set by command line args */
int verbosity = 0; /* print trace if set */
int s = 0; /* set index bits */
int b = 0; /* block offset bits */
int E = 0; /* associativity */
char* trace_file = NULL;

/* Derived from command line args */
int S; /* number of sets S = 2^s In C, you can use the left shift operator */
int B; /* block size (bytes) B = 2^b */

/* Counters used to record cache statistics */
int miss_count = 0;
int hit_count = 0;
int eviction_count = 0;
/*****************************************************************************/


/* Type: Memory address 
 * Use this type whenever dealing with addresses or address masks
 */
typedef unsigned long long int mem_addr_t;

/* Type: Cache line
 *
 * 
 * NOTE: 
 * You might (not necessarily though) want to add an extra field to this struct
 * depending on your implementation
 * 
 * For example, to use a linked list based LRU,
 * you might want to have a field "struct cache_line * next" in the struct 
 */
typedef struct cache_line {
    char valid;
    mem_addr_t tag;
    struct cache_line * next;
} cache_line_t;

typedef cache_line_t* cache_set_t;
typedef cache_set_t* cache_t;

/* The cache we are simulating */
cache_t cache;  

/* TODO - COMPLETE THIS FUNCTION
 * initCache - 
 * Allocate data structures to hold info regrading the sets and cache lines
 * use struct "cache_line_t" here
 * Initialize valid and tag field with 0s.
 * use S (= 2^s) and E while allocating the data structures here
 */
void initCache()
{
    S = 1 << s;
    cache = (cache_set_t*) malloc(sizeof(cache_set_t) * S);
    
    for(int i=0; i<S; i++){
        cache_line_t * head = NULL;
        cache_line_t * curr = NULL;
        for(int j=0; j<E; j++){
            cache_line_t* newLine = (cache_line_t*) malloc(sizeof(cache_line_t));
            newLine -> valid = 0;
            newLine -> next = NULL;
            newLine -> tag = 0;
            if (head == NULL){
                // If head is null, make newLine head!
                head = newLine;
                curr = head;
                continue;
            }
            // If not, add newLine to the end of the list.
            curr -> next = newLine;
            curr = curr -> next; // put the new line at the end
        }
        cache[i] = head;// assign the head to the set
    }
}


/* TODO - COMPLETE THIS FUNCTION 
 * freeCache - free each piece of memory you allocated using malloc 
 * inside initCache() function
 */
void freeCache()
{
    
    for (int i = 0;i < S;i++){
        free(cache[i]);
    }
    
    free(cache);
    cache = NULL;
    printf("freeCache() finished!\n");
}

/* TODO - COMPLETE THIS FUNCTION 
 * accessData - Access data at memory address addr.
 *   If it is already in cache, increase hit_count
 *   If it is not in cache, bring it in cache, increase miss count.
 *   Also increase eviction_count if a line is evicted.
 *   you will manipulate data structures allocated in initCache() here
 */
void accessData(mem_addr_t addr)
{
    
    
    //    find Set
    int t = 64 - s - b;
    int set_number = 0;
    set_number = (addr<<t)>>(t+b);
    int tag_number = (addr)>>(s+b);
    //    go through the line linked list in that set.
    for(int i = 0; i<E;i++){
        cache_line_t * head = cache[set_number];// store the head
        cache_line_t * curr = cache[set_number];// keep track of the curr
        cache_line_t * prev = NULL;// keep track of the previous
        //    If a hit (tag bits and valid bit)
        //				Move that node to the head of the list
        //				Return
        while(curr != NULL){
            if (curr->tag == tag_number && curr -> valid == 1){
		if(prev == NULL) {
		    hit_count++;
		    if(verbosity)
			printf("hit ");
                    return;
		}
                prev -> next = curr -> next;
                curr -> next = head;
		head = curr;
		cache[set_number] = head;
                hit_count++;
		if(verbosity)
		    printf("hit ");
                return;
            }
            prev = curr;
            curr = curr -> next;
        }
        
        //    --If it is a miss
        //        Run through the linked list for that set to see if any line is invalid
        //        If there is one, update that one with the new tag bits
        //        and also move it to the front of the list.
        //        Return
        miss_count++;
        curr = cache[set_number];// restore curr
	if(verbosity)
	    printf("miss ");
        while(curr != NULL){
            if (curr -> valid == 0){
		curr -> tag = tag_number;
		curr -> valid = 1;
                return;
            }
            curr = curr -> next;
        }
       
        
	if(verbosity)
	    printf("eviction ");
        eviction_count++;
        //     --If all lines in the set are valid.
        //      --Need to choose a line for eviction.
        //            Last line in the linked list will be the oldest accessed line!
        //            Replace that line with the new line.
        curr = cache[set_number];// restore curr
        prev = NULL;// restore previous
        while(curr != NULL){
            if (curr -> next == NULL){
		if(prev == NULL){
		   curr -> tag = tag_number;
		   return;
		}
		curr -> tag = tag_number;
		prev -> next = NULL;
		curr -> next = head;
		head = curr;
		cache[set_number] = head;
                return;
            }
            prev = curr;
            curr = curr -> next;
        }
    }
    
}

/* TODO - FILL IN THE MISSING CODE
 * replayTrace - replays the given trace file against the cache 
 * reads the input trace file line by line
 * extracts the type of each memory access : L/S/M
 * YOU MUST TRANSLATE one "L" as a load i.e. 1 memory access
 * YOU MUST TRANSLATE one "S" as a store i.e. 1 memory access
 * YOU MUST TRANSLATE one "M" as a load followed by a store i.e. 2 memory accesses 
 */
void replayTrace(char* trace_fn)
{
    char buf[1000];
    mem_addr_t addr=0;
    unsigned int len=0;
    FILE* trace_fp = fopen(trace_fn, "r");

    if(!trace_fp){
        fprintf(stderr, "%s: %s\n", trace_fn, strerror(errno));
        exit(1);
    }

    while( fgets(buf, 1000, trace_fp) != NULL) {
        if(buf[1]=='S' || buf[1]=='L' || buf[1]=='M') {
            sscanf(buf+3, "%llx,%u", &addr, &len);
      
            if(verbosity)
                printf("%c %llx,%u ", buf[1], addr, len);

           // TODO - MISSING CODE
           // now you have: 
           // 1. address accessed in variable - addr 
           // 2. type of acccess(S/L/M)  in variable - buf[1] 
           // call accessData function here depending on type of access
            switch (buf[1]){
                case 'S':
                    accessData(addr);
                    break;
                case 'L':
                    accessData(addr);
                    break;
                case 'M':
                    accessData(addr);
                    accessData(addr);
                    break;
                default:
                    break;
            }
            
            
            if (verbosity)
                printf("\n");
        }
    }

    fclose(trace_fp);
}

/*
 * printUsage - Print usage info
 */
void printUsage(char* argv[])
{
    printf("Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\nExamples:\n");
    printf("  linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("  linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
    exit(0);
}

/*
 * printSummary - Summarize the cache simulation statistics. Student cache simulators
 *                must call this function in order to be properly autograded.
 */
void printSummary(int hits, int misses, int evictions)
{
    printf("hits:%d misses:%d evictions:%d\n", hits, misses, evictions);
    FILE* output_fp = fopen(".csim_results", "w");
    assert(output_fp);
    fprintf(output_fp, "%d %d %d\n", hits, misses, evictions);
    fclose(output_fp);
}

/*
 * main - Main routine 
 */
int main(int argc, char* argv[])
{
    char c;
    
    // Parse the command line arguments: -h, -v, -s, -E, -b, -t 
    while( (c=getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c){
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_file = optarg;
            break;
        case 'v':
            verbosity = 1;
            break;
        case 'h':
            printUsage(argv);
            exit(0);
        default:
            printUsage(argv);
            exit(1);
        }
    }

    /* Make sure that all required command line args were specified */
    if (s == 0 || E == 0 || b == 0 || trace_file == NULL) {
        printf("%s: Missing required command line argument\n", argv[0]);
        printUsage(argv);
        exit(1);
    }

    /* Initialize cache */
    initCache();
 
    replayTrace(trace_file);

    /* Free allocated memory */
    freeCache();

    /* Output the hit and miss statistics for the autograder */
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}















































































            
