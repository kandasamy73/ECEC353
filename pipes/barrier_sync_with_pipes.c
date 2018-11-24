/* Code how to implement barrier synchronization between a parent and multiple children using 
 * pipes. 
 *
 * The program creates multiple child processes, one for each command line argument, each of which 
 * is intended to accomplish some work. The parent waits until all of its children have comleted 
 * their tasks.
 *
 * Author: Naga Kandasamy
 * Date created: July 4, 2018
 *
 * Compile as follows: gcc -o barrier_sync_with_pipes barrier_sync_with_pipes.c -std=c99 -Wall
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256


int 
main (int argc, char **argv)
{
    int pfd[2];
    
    if (argc < 2 || !strcmp (argv[1], "--help")){
        printf ("Usage: %s sleep-time sleep-time ... sleep-time \n", argv[0]);
        exit (EXIT_SUCCESS);
    }

    /* Create the pipe */
    if (pipe (pfd) == -1){
        perror ("pipe");
        exit (EXIT_FAILURE);
    }

    /* Create the children, one for each sleep-time specified in the command line */
    for (int j =  1; j < argc; j++){
        switch (fork ()){
            case -1: 
                perror ("fork");
                exit (EXIT_FAILURE);

            case 0:                                     /* Child code */
                /* Close read end of the pipe */
                if (close (pfd[0]) == -1){
                    perror ("close");
                    exit (EXIT_FAILURE);
                }

                /* Simulate some processing by sleeping for sleep-time */
                printf ("CHILD %d working for %d seconds. \n", j, atoi (argv[j]));
                sleep (atoi (argv[j])); 

                /* Close the write end of the pipe */
                if (close (pfd[1]) == -1){
                    perror ("close");
                    exit (EXIT_FAILURE);
                }

                /* Child can do other things here */

                exit (EXIT_SUCCESS);

          default:
                break;      /* Parent goes back to create more children */

        }   /* End switch */
    }       /* End for */

   
    /* Close the write end of the pipe for the parent so that we can see EOF */
    if (close (pfd[1]) == -1){
        perror ("close");
        exit (EXIT_FAILURE);
    }

    /* Parent does other work here */

    /* Now, synchronize with the children */
    printf ("PARENT: Barrier reached; waiting for children to finish. \n");
    char dummy;
    if (read (pfd[0], &dummy, 1) != 0){
        printf ("PARENT: did not get EOF. \n");
        exit (EXIT_FAILURE);
    }
    printf ("PARENT: Barrier crossed; moving on to other things. \n");

    exit (EXIT_SUCCESS);
}
