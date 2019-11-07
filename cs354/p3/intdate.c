////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
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

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

/*
 * global field
 *
 * count: a counter to record how many times SIGUSR1 is caught
 *
 * t: the time interval for the signal to be sent to the program in seconds
 */

int count = 0;
int t = 3;

/*
 * the signal handler for alrm signals
 *
 * sig: the value of the signal
 */

void sig_handler(int sig) {
    // intitialize current time
    time_t current_time;
    // get the current time
    time(&current_time);
    
    // when the signal matches, print the pid and time
    if (sig == SIGALRM) {
        // use ctime() for output
        printf("PID: %d | Current Time: %s", getpid(), ctime(&current_time));
        // send alarm signals every t seconds
        alarm(t);
    }
}

/*
 * the signal handler for user input signals
 *
 * sig: the value of the signal
 */

void usr_handler(int sig) {
    // when the signal matches SIGUSR1,
    // increase the counter and print caught message
    if (sig == SIGUSR1) {
        count++;
        printf("SIGUSR1 caught!\n");
    }
}

/*
 * the signal handler for ctrl-c signal
 *
 * sig: the value of the signal
 */

void exit_handler(int sig) {
    // when ctrl-c is typed in, print exit message and exit the program
    if (sig == SIGINT) {
        printf("\nSIGINT received.\n");
        printf("SIGUSR1 was received %d times. Exiting now.\n", count);

        // exit the program
        exit(0);
    }
}

/*
 * main function to process the time and signals
 */

int main() {
    // print the prompts
    printf("Pid and time will be printed every 3 seconds.\n");
    printf("Enter ^C to end the program.\n");
    
    // create structures of sigaction to handle different situations 
    struct sigaction act, usr, exit;
    
    // initialize the structures 
    memset(&act, 0, sizeof(act));
    memset(&usr, 0, sizeof(act));
    memset(&exit, 0, sizeof(exit));
    
    // initialize the handlers to tell program
    // to go where under different conditions 
    act.sa_handler = sig_handler;
    usr.sa_handler = usr_handler;
    exit.sa_handler = exit_handler;
    
    // when received SIGALRM, go to sig_handler
    sigaction(SIGALRM, &act, NULL);
    // when received SIGUSR1, go to usr_handler
    sigaction(SIGUSR1, &usr, NULL);
    // when ctrl-c is typed, go to exit_handler
    sigaction(SIGINT, &exit, NULL);

    // send alarm signals every t seconds
    alarm(t);

    // keep infinite looping until interrupted 
    while (1) {
    }
    
    // return after loop is done
    return 0;
}
