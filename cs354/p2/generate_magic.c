////////////////////////////////////////////////////////////////////////////////
// Main File:        generate_magic.txt
// This File:        generate_magic.txt
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

// Structure representing Square
// size: dimension(number of rows/columns) of the square
// array: 2D array of integers
typedef struct _Square {
    int size;
    int **array;
} Square;

int get_square_size();
Square * generate_magic(int size);
void write_to_file(char *filename, Square * square);

int main(int argc, char *argv[])                 
{
    // Check input arguments to get filename
    // First check if the number of arguments is 2 or not
    if (argc != 2) {
	fprintf(stderr, "%s", "Cannot locate the file to write in\n");
	exit(1);
    }
    // The second argument is the name of file
    // filename: the name of file
    char *filename = argv[1];
    // Get size from user
    // size: the size of square
    int size;
    size = get_square_size();
    // Generate the magic square
    // First check if size is valid of not
    if (size == 0) {
        exit(1);
    }
    // If the size is valid, generate the magic square.
    Square *square = generate_magic(size);
    // Write the square to the output file
    write_to_file(filename, square);
    // Free the location taken on heap
    for (int i = 0; i < size; i++) {
        free(*(square->array + i));
    }
    free(square->array);
    free(square);
    return 0;
}

/* get_square_size prompts the user for the magic square size
 * checks if it is an odd number >= 3 and returns the number
 */
int get_square_size()            
{
    // Print hint message for user to type input
    printf("Enter size of magic square, must be odd\n");
    // sizeString: the size of square of type string
    char sizeString[101];
    // Get the size of string from user input
    fgets(sizeString, 101, stdin);
    if (sizeString == NULL) {
        fprintf(stderr, "%s", "No input detected, failed to get square size.\n");
        return 0;
    }
    // sqaureSize: the size of square of type integer
    int squareSize = atoi( sizeString);
    // Check if the size typed in is valid or not
    if (squareSize < 3 || squareSize % 2 == 0) {
        printf("Size must be an odd number >= 3.\n");
        return 0;
    }
    return squareSize;
}

/* generate_magic constructs a magic square of size n
 * using the Siamese algorithm and returns the Square struct
 */
Square * generate_magic(int n)           
{
    // sqptr: an instance of Square
    Square *sqptr;
    // Give memory location for sqptr on heap
    sqptr = malloc(sizeof(Square));
    if ( sqptr == NULL) {
        fprintf(stderr, "%s", "Failed to allocate memory on heap.\n");
        exit(1);
    }
    sqptr->size = n;
    // Give memory location for the array in sqptr on heap
    sqptr->array = malloc(sizeof(int*) * n);
    if ( sqptr->array == NULL) {
        fprintf(stderr, "%s", "Failed to allocate memory on heap.\n");
        exit(1);
    }
    for (int i = 0; i < n; i++) {
        // Give memory location for each array in array in sqptr on heap
        *(sqptr->array + i) = malloc(sizeof(int) * n);
        if ( *(sqptr->array + i) == NULL) {
            fprintf(stderr, "%s", "Failed to allocate memory on heap.\n");
            exit(1);
        }
    }
    // Initialize each number in sqptr to 0
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
	    *(*(sqptr->array + i) + j) = 0;
	}
    }
    // row: the index of row
    // col: the index of column
    int row = 0; 
    int col = (n - 1) / 2;
    // Assign the middle number in the first row to 1
    *(*(sqptr->array) + (n - 1) / 2) = 1;
    // Put each number in the magic matrix using Siamese method
    for (int i = 2; i <= n * n; i++) {
        int tempRow = row; 
        int tempCol = col;
        --row;
        ++col;
        if (row < 0) {
            row = n - 1;
        }
	if (col > n - 1) {
	    col = 0;
        }
	if (*(*(sqptr->array + row) + col) == 0) {
	    *(*(sqptr->array + row) + col) = i;
	}
	else{
	    row = tempRow;
            col = tempCol;
	    row++;
	    if (row > n - 1) {
		row = 0;
	    }
	    *(*(sqptr->array + row) + col) = i;
	}	
    }
    return sqptr;
}

/* write_to_file opens up a new file(or overwrites the existing file)
 * and writes out the square in the format expected by verify_hetero.c
 */
void write_to_file(char *filename, Square * square)              
{  
    // fp: file stream
    FILE *fp;
    fp = fopen( filename, "w");
    if ( fp == NULL) {
	fprintf(stderr, "%s", "Cannot locate the file to write in");
        exit(1);
    }
    // Write the square to file
    // Firstly write the size of square in the first line
    fprintf( fp, "%d", square->size);
    fprintf( fp, "\n");
    // Then write each row for a line
    for (int i = 0; i < square->size; i++) {
	fprintf( fp, "%d", *(*(square->array + i)));
	for (int j = 1; j < square->size; j++) {
	    fprintf( fp, "%s", ",");
	    fprintf( fp, "%d", *(*(square->array + i) + j));
	}
        fprintf( fp, "\n");
    }
    // Close the file after writting and check if it is closed correctly
    if ( fclose(fp) == EOF) {
        printf("Failed to close file correctly.\n");
        exit(1);
    }
}
