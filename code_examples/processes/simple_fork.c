/*
 * This program illustrates the use of the fork system call to create a child process.
 * Author: Naga Kandasamy
 * Date created: 12/22/2008
 * Date modified: 6/23/2018
 *
 * Compile the code as follows: gcc -o simple_fork -std=c99 simple_fork.c
 *
 * Execute as follows:
 * $ ./simple_fork
*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int 
main (int argc, char **argv)
{
    int pid; /* Process ID returned by fork. */
    int status;
  
    printf ("Process ID of the parent is %d. \n", getpid ()); /* Print parent's process ID. */
    
    if ((pid = fork ()) < 0){ /* Fork the process. */
        printf ("Fork error \n");
        exit (-1);
    }
  
    if (pid > 0){ /* Parent code. */
        printf ("Process ID of the child is %d. \n", pid); /* Print child's process ID. */
    }
    else if (pid == 0){ /* Child code. */
        printf ("Process ID of the child is %d. \n", getpid ()); /* Print child's process ID. */
        sleep (3); /* Sleep for a few seconds to simulate some processing. */
        exit (0); /* Child exits. */
    }

    /* Parent code. */
    pid = waitpid (pid, &status, 0); /* Wait for child to terminate and write the status. */
    printf ("Child %d has terminated with status %d. Parent terminating. \n", pid, status); 
    exit (0);
}
