/*  Program to illustrate basic thread management operations. 
 *
 *  Author: Naga Kandasamy
 *  Date created: January 21, 2009
 *  Date modifed: August 20, 2018
 *
 *  Compile as follows: gcc -o simple_thread simple_thread.c -std=c99 -Wall -lpthread 
*/

#define _REENTRANT // Make sure the library functions are MT (muti-thread) safe
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/* Function prototypes for the thread routines */
void *func_a(void *);
void *func_b(void *);

/* Global variables */
pthread_t thr_a, thr_b; /* Thread IDs */
int a = 10;

int 
main (int argc, char **argv)
{
    /* Create a new thread and ask it to execute func_a that takes no arguments */
    if ((pthread_create (&thr_a, NULL, func_a, NULL)) != 0){
        perror ("pthread_create");
        exit (EXIT_FAILURE);
    }
	
    printf ("Main thread %u is creating thread %u \n", (unsigned int) pthread_self (), (unsigned int) thr_a);
    pthread_join (thr_a, NULL); /* Wait for thread to finish */
		  
    printf ("Value of a is %d\n", a);
		  
    printf ("Main thread exiting \n");
    pthread_exit ((void *) pthread_self ());
}

/* Function A */
void *
func_a (void *arg)
{
    thr_a = pthread_self ();
    int args_for_thread = 5;
	
    /* Create a new thread and ask it to execute func_b */
    if ((pthread_create (&thr_b, NULL, func_b, (void *) args_for_thread)) != 0){
        perror ("pthread_create thr_a");
        exit (EXIT_FAILURE);
    }

    /* Simulate some processing */
    printf ("Thread %u is processing. \n", (unsigned int) thr_a);
    int i;
    for (i = 0; i < 5; i++) {
        a = a + 1;
        sleep (1);
    }
    pthread_join (thr_b, NULL); // Wait for thread B to finish 

    printf ("Thread %u is exiting \n", (unsigned int) thr_a);
    pthread_exit ((void *) thr_a);
}

/* Function B */
void *
func_b (void *arg)
{
    int args_for_me = (int) arg;
    pthread_t thr_b = pthread_self ();

    /* Simulate some processing */
    printf ("Thread %u is using args %d to process... \n", (unsigned int) thr_b, args_for_me);
    int i;
    for (i = 0; i < 5; i++) {
        printf (". \n");
        sleep (2);
    }
		
    printf ("Thread %u is exiting \n", (unsigned int) thr_b);
    pthread_exit ((void *) thr_b);
}







