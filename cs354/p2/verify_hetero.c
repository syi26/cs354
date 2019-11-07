////////////////////////////////////////////////////////////////////////////////
// Main File:        verify_hetero.c
// This File:        verify_hetero.c
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
#include <stdlib.h>
#include <string.h>

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

Square * construct_square(char *filename);
int verify_hetero(Square * square);
void insertion_sort(int* arr, int size);

int main(int argc, char *argv[])                        
{
    // Check input arguments to get filename
    // First check if the number of arguments passed in is 2 or not
    // Only runs when there are 2 arguments
    if (argc != 2) {
        fprintf(stderr, "%s", "Cannot open file for reading.\n");
        exit(1);
    }
    // The second argument should be the name of file to read
    // filename: the name of file to read
    char *filename = argv[1];
    // Construct square
    // sqptr: an instance of Square
    Square *sqptr = construct_square(filename);
    // Check if sqptr is constructed correctly
    if (sqptr == NULL) {
        fprintf(stderr, "%s", "Failed to generate square from file.\n");
        exit(1);
    } 
    // Verify if it's a heterosquare and print true or false
    // hetero: return value of verify_hetero(Square * square);
    int hetero = verify_hetero(sqptr);
    // Free the space taken on heap
    for (int i = 0; i < sqptr->size; i++) {
        free(*(sqptr->array + i));
    }
    free(sqptr->array);
    free(sqptr);
    // Print true if it is a hetero square, false on the other hand
    if (hetero == 0) {
        printf("false");
    }
    else {
        printf("true");
    }
    printf("\n");
    return 0;
}

/* construct_square reads the input file to initialize a square struct
 * from the contents of the file and returns the square.
 * The format of the file is defined in the assignment specifications
 */
Square * construct_square(char *filename)                
{   
    // squareSize: size of square
    // fp: file stream
    int squareSize;
    FILE *fp;
    // Open and read the file
    fp = fopen( filename, "r");
    // Check if the file stream exists or not
    if (fp == NULL) {
        fprintf(stderr, "%s", "Cannot open file for reading.\n");
        exit(1);
    }
    // Read the first line to get the square size
    // sizeString: the size of square read in of string type 
    char sizeString[101];
    // Read the first line from file stream
    fgets(sizeString, 101, fp);
    if (sizeString == NULL) {
        fprintf(stderr, "%s", "Failed to get the size of square.\n");
        exit(1);
    }
    // Convert string to integer to get the size of square
    squareSize = atoi( sizeString);
    if (squareSize == 0) {
        printf("Failed to get the size of square or size of square is 0.\n");
	exit(1);
    }
    // Initialize a new Square struct of that size
    // sqptr: an instance of Square
    Square *sqptr;
    // Give sqptr memory location on heap
    sqptr = malloc(sizeof(Square));
    // Check if malloc succeeded or not
    if ( sqptr == NULL) {
        fprintf(stderr, "%s", "Failed to allocate memory on heap.\n");
        exit(1);
    }
    // Assign value to the size of new square constructed
    sqptr->size = squareSize;
    // Read the rest of the file to fill up the square
    // Give the array in new square constructed memory location on heap
    sqptr->array = malloc(sizeof(int*) * squareSize);
    // Check if malloc succeeded or not
    if ( sqptr->array == NULL) {
        fprintf(stderr, "%s", "Failed to allocate memory on heap.\n");
        exit(1);
    }
    // Give every array in array memory location on heap
    for (int i = 0; i < sqptr->size; i++) {
        *(sqptr->array + i) = malloc(sizeof(int) * sqptr->size);
        if ( *(sqptr->array + i) == NULL) {
            fprintf(stderr, "%s", "Failed to allocate memory on heap.\n");
            exit(1);
        }
    }
    // s: the char to be seperated by
    // token: integers got after seperating strings
    const char s[2] = ",";
    char *token;
    for (int i = 0; i < squareSize; i++) {
        // str2: store each line of the rest of file stream
        char str2[101];
        // Get next line in file stream and store it in str2
        fgets( str2, 101, fp);
	if (str2 == NULL) {
            fprintf(stderr, "%s", "Failed to get data in square.\n");
	    exit(1);
	}
        // Assign value to the first integer in square
	token = strtok( str2, s);
        // col: record the number of integers got from file stream
	int col = 0;
	while ( token != NULL) {
            // Convert chars into integers
            *(*(sqptr->array + i) + col) = atoi( token);
            if  ( *(*(sqptr->array + i) + col) == 0) {
		fprintf(stderr, "%s", "Failed to get data in square.\n");
		exit(1);
	    }
            // Increase the counts
            col++;
            // Get the next integer
	    token = strtok(NULL, s);
	}
    }
    // Close the file and at same time check if it is closed correctly
    if (fclose(fp) == EOF) {
        fprintf( stderr, "%s", "Failed to close file correctly.\n");
        exit(1);
    }	
    return sqptr;
}

/* verify_hetero verifies if the square is a heterosquare
 * 
 * returns 1(true) or 0(false)
 */
int verify_hetero(Square * square)               
{
    // Calculate sum of the following and store it in an array
    // sums: the array storing all the sums
    int *sums = malloc(sizeof(int) * (square->size + 1) * 2); 
    // all rows and cols
    // First calculate the sums of all rows
    // rowSum: the sum of a specific row
    int rowSum = 0;
    // Calculate the sum of each row and store them in sums
    for (int i = 0; i < square->size; i++) {
	for (int j = 0; j < square->size; j++) {
	    rowSum += *(*(square->array + i) + j);
       	}
	*(sums + i) = rowSum;
        // Assign 0 to rowSum for next row
      	rowSum = 0;
    }
    
    // Then calculate the sum of each column and store them in sums
    // colSum: the sum of a specific column
    int colSum = 0;
    // Calculate the sum of each column and store them in sums
    for (int i = square->size; i < square->size * 2; i++) {
	for (int j = 0; j < square->size; j++) {
	    colSum += *(*(square->array + j) + (i - square->size));
	}
	*(sums + i) = colSum;
        // Assign 0 to colSum for next column
        colSum = 0;
    }
    
    // main diagonal
    // mDiaSum: the sum of main diagonal
    int mDiaSum = 0;
    // Calculate the sum of main diagonal and store it in sums
    for (int i = 0; i < square->size; i++) {
        mDiaSum += *(*(square->array + i) + i);
    }
    *(sums + 2 * square->size) = mDiaSum;
    
    // secondary diagonal
    // sDiaSum: the sum of secondary diagonal
    int sDiaSum = 0;
    // Calculate the sum of secondary diagonal and store it in sums
    for (int i = 0; i < square->size; i++) {
	sDiaSum += *(*(square->array + i) + (square->size - i - 1));
    }
    *(sums + (2 * square->size + 1)) = sDiaSum;
    // Pass the array to insertion_sort function
    insertion_sort( sums, (square->size + 1) * 2);
    // Check the sorted array for duplicates
    int dup = 1;
    for (int i = 0; i < (square->size * 2 + 1); i++) { 
	if ( *(sums + i) == *(sums + i + 1 )) {
	    dup = 0;
	}
    }
    return (dup);
}

/* insertion_sort sorts the arr in ascending order
 *
 */
void insertion_sort(int* arr, int size)                  
{
    // Sort the arr
    int key;
    for (int i = 0; i < size; i++) {
	int j = i;
        key = *(arr + j);
	while (j > 0 && *(arr + (j - 1)) > key) {
	    *(arr + j) = *(arr + (j - 1));
	    j--;
	}
	*(arr + j) = key;
    }
}
