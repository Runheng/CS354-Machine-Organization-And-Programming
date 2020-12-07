////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        sendsig.c
// Other Files:      intdate.c
// Semester:         CS 354 Spring 2019
//
// Author:           Runheng Lei
// Email:            rlei5@wisc.edu
// CS Login:         runheng
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
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>

/* main function of the program, processes user's input and send specific signal,
 * which will be handled by intdate.c
 * It requires the user to input the signal and pid,
 * invalid input will cause exit of program
 */
int main(int argc, char *argv[]) {

    // check if input is valid
    if (argc != 3) {
        printf("Usage: <signal type> <pid>");
        exit(1);
    } 

    // get pid from user's input
    int pid = atoi(argv[2]);

    // check if signal is SIGUSR1
    if ((strcmp(argv[1], "-u")) == 0) {
        kill(pid, SIGUSR1);
    }

        // check if signal isSIGINT
    else if ((strcmp(argv[1], "-i")) == 0) {
        kill(pid, SIGINT);
    }

        // if not valid signal, show error message and exit program
    else {
        printf("Invalid signal type");
        exit(1);
    }
    return 0;
}
