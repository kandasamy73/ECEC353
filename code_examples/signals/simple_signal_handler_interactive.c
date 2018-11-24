/* This code illustrates the correct way to implement a simple signal handler that catches the Control+C signal.
 * The signal handler uses no non-reentrant functions. Rather, it justs sets a global flag that is read by the 
 * main function.
 *
 * Author: Naga Kandasamy
 * Date created: 6/26/2018
 *
 * */

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void custom_signal_handler (int); 
static int sigFlag;

int 
main (int argc, char **argv)
{
    sigFlag = 0; /* Set the signal flag */
  
    /* Sets up our signal handler to catch SIGINT */
    signal (SIGINT, custom_signal_handler);
    
    /* Main procesing loop. */
    for (int i = 0; i < 10; i++){
        printf ("Working. \n");
        sleep (2); /* Sleep for 2 seconds each iteration to simulate some processing in the loop */
        
        if(sigFlag == 1){
            printf ("SigFlag set...quiting \n");
            /* Do clean up; close open files, sockets, etc. */
            exit (0);
        }
    }
    exit (0);
}

/* The user-defined signal handler */
static void 
custom_signal_handler (int signalNumber)
{
    if (signalNumber == SIGINT)
        sigFlag = 1;
  
    else 
        printf ("Error: Unknown Signal received \n");
 
    /* Restablish handler for next occurrence */
    signal (SIGINT, custom_signal_handler); 

    return;

}
