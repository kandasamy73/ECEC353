/* This program illustrates that after a child process is created using the fork() system call, 
 * the parent and the child execute in separate address spaces.  
 * 
 * Author: Naga Kandasamy
 * Created: 12/22/2008
 * Last modified: 6/23/2018
 *
 * Compile the code as follows: gcc simple_fork_v2.c -o simple_fork_v2 -std=c99 -Wall
 * Execute as follows: ./simple_fork_v2
*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int global_variable = 10;

int 
main (int argc, char **argv)
{
    int pid; /* Process ID */
    int status;
    int local_variable = 100; 
  
    if ((pid = fork ()) < 0){ /* Fork the process */
        perror ("Fork");
        exit (-1);
    }
  
    else if (pid == 0){ /* Child code */
        global_variable++; /* Increment global variable */
        local_variable++; /* Increment local variable */
        printf ("Child: Global variable = %d, Local variable = %d \n", global_variable, local_variable);
        exit (0); /* Child exits */
    }

    /* Parent code */
    global_variable--; /* Decrement global and local variables that are stored in the parent's data segment */
    local_variable--;
    pid = waitpid (pid, &status, 0); /* Wait for child to terminate */
    printf ("Parent: Global variable = %d, Local variable = %d \n", global_variable, local_variable);
    exit (0);
}
