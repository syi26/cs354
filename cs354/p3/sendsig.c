////////////////////////////////////////////////////////////////////////////////
// Main File:        sendsig.c
// This File:        sendsig.c
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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

/*
 * main function to send signals
 *
 * argc: the number of arguments
 *
 * argv: arrayof arguments
 */

int main(int argc, char **argv) {
    // print error message and quit when there is less than 3 arguments
    if (argc != 3) {
        printf("Usage: <signal type> <pid>");

        // exit
        exit(0);
    }

    // convert the string for the third argument into integer
    pid_t p = atoi(argv[2]);
    
    // check if the first argument is -i or -u
    if (!strcmp("-i", argv[1])) {
        // if the first argument is -i, send exit signal
        kill(p, SIGINT);
    } else if (!strcmp("-u", argv[1])) {
        // if the first argument is -u, send user input
        kill(p, SIGUSR1);
    }

    // return after signal is sent
    return 0;
}

