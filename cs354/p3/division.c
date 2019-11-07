////////////////////////////////////////////////////////////////////////////////
// Main File:        division.c
// This File:        division.c
// Other Files:      N/A
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
//////////////////////////// 80 columns wide ///////////////////////////////////

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/* 
 * global field
 *
 * count: count how many times valid divisions are made
 */

int count;

/*
 * to handle the exceptions caused by dividing 0
 *
 * sig: the value of input signal
 */

void s_handler(int sig) {
    // print output message
    printf("Error: a division by 0 operation was attempted.\n");
    printf("Total number of operations successfully completed: %d\n", count);
    printf("The program will be terminated.\n");
    
    // exit
    exit(0);
}

/* 
 * to handle the ctrl-c signal
 *
 * sig: the value of input signal
 */

void exit_handler(int sig) {
    // print output message
    printf("\nTotal number of operations successfully completed: %d\n", count);
    printf("The program will be terminated.\n");
    
    // exit
    exit(0);
}

/*
 * main function to perform the division and go to exception handler when necessary
 */

int main() {
    // create structures of sigaction to handle different special conditions
    struct sigaction try, exit;

    // initialize the structures
    memset(&try, 0, sizeof(try));
    memset(&exit, 0, sizeof(exit));

    // initialize handlers to tell the program
    // to go where under different conditions
    try.sa_handler = s_handler;
    exit.sa_handler = exit_handler;

    // if received ctrl-c, go to exit_handler
    sigaction(SIGINT, &exit, NULL);
    // if divided by 0, go to s_handler
    sigaction(SIGFPE, &try, NULL);
    
    // keep infinitely looping until special conditions met
    while (1) {
        // set up input string for two arguments
        char input1[100];
        char input2[100];

        // initialize input numbers
        int int1 = 0;
        int int2 = 0;

        // print prompt for user to type the first argument
        printf("Enter first integer: ");
        // get the first argument
        if (fgets(input1, 100, stdin) == NULL) {
            continue;
        }
        // convert the first argument into integer and store it in int1
        int1 = atoi(input1);

        // print prompt for user to type the second argument
        printf("Enter second integer: ");
        // get the second argument
        if (fgets(input2, 100, stdin) == NULL) {
            continue;
        }
        // convert the second argument into integer and store it in int2
        int2 = atoi(input2); 

        // initialize the quotient and remainder of the division
        int remainder;
        int quotient;

        // calculate the quotient and remainder
        quotient = int1 / int2;
        remainder = int1 % int2;

        // increase count to record how many divisions have been 
        // successfully made
        count++;

        // print out the successful division formula
        printf("%d / %d is %d with a remainder of %d\n",
                int1, int2, quotient, remainder);
    }

    // return after loop is done
    return 0;
}
