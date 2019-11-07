////////////////////////////////////////////////////////////////////////////////
// Main File:        mem.c
// This File:        mem.c
// Other Files:      (name of all other files if any)
// Semester:         CS 354 Spring 2018
//
// Author:           Sirui Yi
// Email:            syi26@wisc.edu
// CS Login:         sirui
//
/////////////////////////// OTHER SOURCES OF HELP //////////////////////////////
//                   fully acknowledge and credit all sources of help,
//                   other than Instructors and TAs.
//
// Persons:          Identify persons by name, relationship to you, and email.
//                   Describe in detail the the ideas and help they provided.
//
// Online sources:   avoid web searches to solve your problems, but if you do
//                   search, be sure to include Web URLs and description of 
//                   of any information you find.
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "mem.h"

/*
 * This structure serves as the header for each allocated and free block
 * It also serves as the footer for each free block
 * The blocks are ordered in the increasing order of addresses 
 */
typedef struct blk_hdr {                         
        int size_status;
  
    /*
    * Size of the block is always a multiple of 8
    * => last two bits are always zero - can be used to store other information
    *
    * LSB -> Least Significant Bit (Last Bit)
    * SLB -> Second Last Bit 
    * LSB = 0 => free block
    * LSB = 1 => allocated/busy block
    * SLB = 0 => previous block is free
    * SLB = 1 => previous block is allocated/busy
    * 
    * When used as the footer the last two bits should be zero
    */

    /*
    * Examples:
    * 
    * For a busy block with a payload of 20 bytes (i.e. 20 bytes data + an
    * additional 4 bytes for header)
    * Header:
    * If the previous block is allocated, size_status should be set to 27
    * If the previous block is free, size_status should be set to 25
    * 
    * For a free block of size 24 bytes (including 4 bytes for header +
    * 4 bytes for footer)
    * Header:
    * If the previous block is allocated, size_status should be set to 26
    * If the previous block is free, size_status should be set to 24
    * Footer:
    * size_status should be 24
    * 
    */
} blk_hdr;

/* Global variable - This will always point to the first block
 * i.e. the block with the lowest address */
blk_hdr *first_blk = NULL;

/*
 * Note: 
 *  The end of the available memory can be determined using end_mark
 *  The size_status of end_mark has a value of 1
 *
 */

/* 
 * Function for allocating 'size' bytes
 * Returns address of allocated block on success 
 * Returns NULL on failure 
 * Here is what this function should accomplish 
 * - Check for sanity of size - Return NULL when appropriate 
 * - Round up size to a multiple of 8 
 * - Traverse the list of blocks and allocate the best free block which
 * can accommodate the requested size 
 * - Also, when allocating a block - split it into two blocks
 * Tips: Be careful with pointer arithmetic 
 */
void* Mem_Alloc(int size) {
    // Your code goes in here
    // current: current block accessed, initialize to first block
    // bestFit: the best block we could find to allocate memory
    // header: the header of desired block
    // footer: the footer of desired block
    // tempSize: size of temporary block processed
    // tempStat: status of temporary block processed
    blk_hdr *current = first_blk;
    blk_hdr *bestFit = NULL;
    blk_hdr *header = NULL;
    blk_hdr *footer = NULL;
    int tempSize = 0;
    int tempStat = 0;
    // fail to allocate if size is less than 0
    if (size < 0) {
        return NULL;
    }
    // round up the sum of size and size of block header to multiple of 8
    // to see how much space do we need to allocate memory
    if ((size + sizeof(blk_hdr)) % 8 != 0) {
        size = ((size + sizeof(blk_hdr)) / 8 + 1) * 8;
    } else {
        size = size + sizeof(blk_hdr);
    }
    // sizeDifference: the difference of sizes between the size of block
    // and the size need to allocate
    int sizeDifference = 10000;
    // traverse until reaching the end mark
    while (current->size_status != 1) {
        // set tempStat to the status of current block
        tempStat = current->size_status % 8;
        // set tempSize to the size of current block
        tempSize = current->size_status - tempStat;
        // check if current block is a free block of enough size for 
        // allocation, and is smaller than the block chosen before
        if (tempStat % 2 != 1 && tempSize - size >= 0
            && (tempSize - size) < sizeDifference) {
            // if a smaller usable block is found, update the difference of
            // sizes and set the pointer to this block
            sizeDifference = tempSize - size;
            bestFit = current;
        }
        // update pointer
        current += tempSize / sizeof(blk_hdr);
    }
    // fail to allocate if no available block is found
    if (bestFit == NULL) {
        return NULL;
    }
    // set tempSize to the size of smallest block found     
    tempSize = bestFit->size_status - bestFit->size_status % 8;
    // record the status of this block
    // prevStat: the status before changing the status of this block
    int prevStat = bestFit->size_status % 8 / 2;
    // update the size_status of chosen block
    bestFit->size_status = size + 2 * prevStat + 1;
    // get the position of header of next block
    blk_hdr *nextHeader = bestFit + tempSize / sizeof(blk_hdr);
    // check if the difference of size is larger than 8 or not
    if (sizeDifference >= 8) {
        // create a new header in the chosen block to split the block into two
        header = bestFit + size / sizeof(blk_hdr);
        // update the size_status of header
        header->size_status = sizeDifference + 2;
        // create a new footer in the block separated from chosen block
        footer = header + (sizeDifference - sizeof(blk_hdr))
            / sizeof(blk_hdr);
        // update the size_status of footer (the size of new block)
        footer->size_status = sizeDifference;
    } else if (nextHeader->size_status != 1) {
        // update the size_status of header of next block 
        // when it is not end mark
        nextHeader->size_status += 2;
    }
    // return the address of allocated block at success
    return bestFit + 1;
}

/* 
 * Function for freeing up a previously allocated block 
 * Argument - ptr: Address of the block to be freed up 
 * Returns 0 on success 
 * Returns -1 on failure 
 * Here is what this function should accomplish 
 * - Return -1 if ptr is NULL
 * - Return -1 if ptr is not 8 byte aligned or if the block is already freed
 * - Mark the block as free 
 * - Coalesce if one or both of the immediate neighbours are free 
 */

int Mem_Free(void *ptr) {
    // curHead: pointer for the header of current block being processed
    // nextHead: pointer for the header of next block
    // preHead: pointer for the header of previous block
    // preFoot: pointer for the footer of previous block
    // newFoot: pointer for the footer of coalesced block
    // curSize: size of current block being processed
    // nextSize: size of next block
    // preSize: size of previous block
    // coalSize: size of coalesced block
    blk_hdr *curHead = NULL;
    blk_hdr *nextHead = NULL;
    blk_hdr *preHead = NULL;
    blk_hdr *preFoot = NULL;
    blk_hdr *newFoot = NULL;
    int curSize = 0;
    int nextSize = 0;
    int preSize = 0;
    int coalSize;
    // return -1 if ptr is NULL
    if (ptr == NULL) {
        return -1;
    }
    // return -1 if ptr is not 8 byte aligned
    // cast the address of the ptr to int and check if it's a multiple of 8
    if (((unsigned int)ptr) % 8 != 0) {
        return -1;
    }
    // update pointer for current header
    curHead = (blk_hdr*)(ptr - 4);
    // return -1 if current block is already freed
    if (curHead->size_status % 2 == 0) {
        return -1;
    }
    // mark the block being processed as free
    curHead->size_status = curHead->size_status - 1;
    // find the size of current block
    curSize = curHead->size_status / 8 * 8;
    // update the pointer for next Header and find the size of next block
    nextHead = curHead + curSize / sizeof(blk_hdr);
    nextSize = nextHead->size_status / 8 * 8;
    // update the pointer for previous footer
    preFoot = curHead - 1;
    // check if the previous block has already been allocated
    // find the size of previous block and use that to update
    // the header of previous
    // block only if previous block is free. 
    // if previous block has been allocated, the previous 
    // header would still remain NULL
    if (preFoot->size_status != 0 && curHead->size_status % 8  == 0) {
        preSize = preFoot->size_status;
        preHead = curHead - preSize / sizeof(blk_hdr);
    }
    // coalesce under different situations
    // if next block is free and previous block is allocated
    if (nextHead->size_status % 2 == 0 && preHead == NULL) {
        // calculate the size of the coalesced block
        coalSize = curSize + nextSize;
        // update pointers for changed headers
        nextHead->size_status = 0;
        curHead->size_status = coalSize + 2;
        // Create a new footer for the coalesced block
        newFoot = nextHead + nextSize / sizeof(blk_hdr) - 1;
        newFoot->size_status = coalSize;
        return 0;
    }
    // if prev block is free and next block is allocated
    if (nextHead->size_status % 2 == 1 && preHead != NULL) {
        // Calculate the size of the coalesced block
        coalSize = curSize + preSize;
        // update pointers for changed headers
        curHead->size_status = 0;
        preHead->size_status = coalSize + 2;
        preFoot->size_status = 0;
        nextHead->size_status -= 2;
        // create a new footer for the coalesced block
        newFoot = nextHead - 1;
        newFoot->size_status = coalSize;
        return 0;
    }
    // if both prev and next blocks are free
    if (nextHead->size_status % 2 == 0 && preHead != NULL) {
        // calculate the size of the coalesced block
        coalSize = curSize + preSize + nextSize;
        // update pointers for changed headers
        curHead->size_status = 0;
        nextHead->size_status = 0;
        preHead->size_status = coalSize + 2;
        preFoot->size_status = 0;
        // create a new footer for the coalesced block
        newFoot = nextHead + nextSize / sizeof(blk_hdr) - 1;
        newFoot->size_status = coalSize;
        return 0;
    }
    // if neither prev nor next blocks is free
    if (nextHead->size_status % 2 == 1 && preHead == NULL) {
        // calculate the size of the coalesced block
        coalSize = curSize;
        // update pointers for changed headers
        curHead->size_status = coalSize + 2;
        nextHead->size_status = nextSize + 1; 
        // Create a new footer for the coalesced block
        newFoot = nextHead - 1;
        newFoot->size_status = coalSize;
        return 0;
    }
    // return -1 when fail
    return -1;
}

/*
 * Function used to initialize the memory allocator
 * Not intended to be called more than once by a program
 * Argument - sizeOfRegion: Specifies the size of the chunk which needs to be
 * allocated
 * Returns 0 on success and -1 on failure 
 */
int Mem_Init(int sizeOfRegion) {                         
    int pagesize;
    int padsize;
    int fd;
    int alloc_size;
    void* space_ptr;
    blk_hdr* end_mark;
    static int allocated_once = 0;
  
    if (0 != allocated_once) {
        fprintf(stderr, 
        "Error:mem.c: Mem_Init has allocated space during a previous call\n");
        return -1;
    }
    if (sizeOfRegion <= 0) {
        fprintf(stderr, "Error:mem.c: Requested block size is not positive\n");
        return -1;
    }

    // Get the pagesize
    pagesize = getpagesize();

    // Calculate padsize as the padding required to round up sizeOfRegion 
    // to a multiple of pagesize
    padsize = sizeOfRegion % pagesize;
    padsize = (pagesize - padsize) % pagesize;

    alloc_size = sizeOfRegion + padsize;

    // Using mmap to allocate memory
    fd = open("/dev/zero", O_RDWR);
    if (-1 == fd) {
        fprintf(stderr, "Error:mem.c: Cannot open /dev/zero\n");
        return -1;
    }
    space_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 
                    fd, 0);
    if (MAP_FAILED == space_ptr) {
        fprintf(stderr, "Error:mem.c: mmap cannot allocate space\n");
        allocated_once = 0;
        return -1;
    }
  
     allocated_once = 1;

    // for double word alignement and end mark
    alloc_size -= 8;

    // To begin with there is only one big free block
    // initialize heap so that first block meets 
    // double word alignement requirement
    first_blk = (blk_hdr*) space_ptr + 1;
    end_mark = (blk_hdr*)((void*)first_blk + alloc_size);
  
    // Setting up the header
    first_blk->size_status = alloc_size;

    // Marking the previous block as busy
    first_blk->size_status += 2;

    // Setting up the end mark and marking it as busy
    end_mark->size_status = 1;

    // Setting up the footer
    blk_hdr *footer = (blk_hdr*) ((char*)first_blk + alloc_size - 4);
    footer->size_status = alloc_size;
  
    return 0;
}

/* 
 * Function to be used for debugging 
 * Prints out a list of all the blocks along with the following information i
 * for each block 
 * No.      : serial number of the block 
 * Status   : free/busy 
 * Prev     : status of previous block free/busy
 * t_Begin  : address of the first byte in the block (this is where the header starts) 
 * t_End    : address of the last byte in the block 
 * t_Size   : size of the block (as stored in the block header) (including the header/footer)
 */ 
void Mem_Dump() {                        
    int counter;
    char status[5];
    char p_status[5];
    char *t_begin = NULL;
    char *t_end = NULL;
    int t_size;

    blk_hdr *current = first_blk;
    counter = 1;

    int busy_size = 0;
    int free_size = 0;
    int is_busy = -1;

    fprintf(stdout, "************************************Block list***\
                    ********************************\n");
    fprintf(stdout, "No.\tStatus\tPrev\tt_Begin\t\tt_End\t\tt_Size\n");
    fprintf(stdout, "-------------------------------------------------\
                    --------------------------------\n");
  
    while (current->size_status != 1) {
        t_begin = (char*)current;
        t_size = current->size_status;
    
        if (t_size & 1) {
            // LSB = 1 => busy block
            strcpy(status, "Busy");
            is_busy = 1;
            t_size = t_size - 1;
        } else {
            strcpy(status, "Free");
            is_busy = 0;
        }

        if (t_size & 2) {
            strcpy(p_status, "Busy");
            t_size = t_size - 2;
        } else {
            strcpy(p_status, "Free");
        }

        if (is_busy) 
            busy_size += t_size;
        else 
            free_size += t_size;

        t_end = t_begin + t_size - 1;
    
        fprintf(stdout, "%d\t%s\t%s\t0x%08lx\t0x%08lx\t%d\n", counter, status, 
        p_status, (unsigned long int)t_begin, (unsigned long int)t_end, t_size);
    
        current = (blk_hdr*)((char*)current + t_size);
        counter = counter + 1;
    }

    fprintf(stdout, "---------------------------------------------------\
                    ------------------------------\n");
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fprintf(stdout, "Total busy size = %d\n", busy_size);
    fprintf(stdout, "Total free size = %d\n", free_size);
    fprintf(stdout, "Total size = %d\n", busy_size + free_size);
    fprintf(stdout, "***************************************************\
                    ******************************\n");
    fflush(stdout);

    return;
}
