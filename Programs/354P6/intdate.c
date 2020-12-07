////////////////////////////////////////////////////////////////////////////////
// Main File:        intdate.c
// This File:        intdate.c
// Other Files:      sendsig.c
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
#include <time.h>

// global variables
int sec = 3; // time interval of the alarm signal
int counter = 0; // keep track of total number of SIGUSR1

/* Handler when SIGALRM is sent, print out pid and current time
 * Send alarm signal again
 */
void handler_SIGALRM() {

    time_t currtime = time(NULL);
    char *time_str = ctime(&currtime);

    // check if error occurs when trying to get current time
    if (currtime == -1 || time_str == NULL) {
        printf("Error calculating time or formatting time!");
        exit(1);
    }
    pid_t pid = getpid();

    printf("PID: %d | Current Time: %s", pid, time_str);

    alarm(sec);
}

/* Handler when SIGUSR1 is sent
 * Print info and increment counter
 */
void handler_SIGUSR1() {
    printf("SIGUSR1 caught!\n");
    counter++;
}

/* Handler when press control-c
 * Print info and exit program
 */
void handler_SIGINT() {
    printf("\nSIGINT received.\nSIGUSR1 was received %d times. Exiting now.\n", counter);
    exit(0);
}

/* Main method of the program
 * Send an alarm signal first and calls handler to handle it
 * Infinite loop of alarm signals will be sent
 * Respond to SIGUSR1 and SIGINT
 */
int main() {

    printf("Pid and time will be printed every 3 seconds.\nEnter ^C to end the program.\n");

    // set up sigaction for SIGALRM
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    alarm(sec); // triger the first SIGALRM

    act.sa_handler = handler_SIGALRM;

    // handles SIGALRM
    if (sigaction(SIGALRM, &act, NULL) != 0){
        printf("ERROR: registering sigalarm\n");
        exit(1);
    }

    // set up sigaction for SIGUSR1
    struct sigaction act2;
    memset(&act2, 0, sizeof(act2));

    act2.sa_handler = handler_SIGUSR1;

    // handles SIGUSR1
    if (sigaction(SIGUSR1, &act2, NULL) != 0){
        printf("ERROR: registering sigusr1\n");
        exit(1);
    }

    // set up sigaction for SIGINT
    struct sigaction act3;
    memset(&act3, 0, sizeof(act3));

    act3.sa_handler = handler_SIGINT;

    // handles SIGINT
    if (sigaction(SIGINT, &act3, NULL) != 0){
        printf("ERROR: registering sigint\n");
        exit(1);
    }

    // infinite loop
    while (1) {
    }

    // return 0;
}

