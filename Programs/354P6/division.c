////////////////////////////////////////////////////////////////////////////////
// Main File:        division.c
// This File:        division.c
// Other Files:      none
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
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

// global variable to keep track of total successful operations
int counter = 0;

/* handler when divide by zero
 */
void handler_SIGFPE() {
    printf("Error: a division by 0 operation was attempted.\n");
    printf("Total number of operations completed successfully: %d\n", counter);
    printf("The program will be terminated.\n");
    exit(1);
}

/* handler when press control-c
 */
void handler_SIGINT() {
    printf("\nTotal number of operations successfully completed: %d\n", counter);
    printf("The program will be terminated.\n");
    exit(0);
}

/* main function of the program, infinite loop to prompt user to enter two numbers
 * and then do division operation, and handles SIGFPE and SIGINT is needed
 */
int main() {
// set up sigaction for SIGFPE
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = handler_SIGFPE;

    // set up sigaction for SIGINT
    struct sigaction act2;
    memset(&act2, 0, sizeof(act2));
    act2.sa_handler = handler_SIGINT;

    if (sigaction(SIGFPE, &act, NULL) != 0){
        printf("ERROR: registering sigfpe\n");
        exit(1);
    }

    if (sigaction(SIGINT, &act2, NULL) != 0){
        printf("ERROR: registering sigint\n");
        exit(0);
    }

    // infinite loop that prompts user to enter two numbers and do division operation
    while (1) {
        char buf[100];

        // prompt user and read input
        printf("Enter first integer: ");
        int num1 = atoi(fgets(buf, 100, stdin));
        printf("Enter second integer: ");
        int num2 = atoi(fgets(buf, 100, stdin));

        // do the calculation
        int quotient = num1 / num2;
        int remainder = num1 % num2;

        // increment counter
        counter++;
        // print output
        printf("%d / %d is %d with a remainder of %d\n", num1, num2, quotient, remainder);
    }
}
