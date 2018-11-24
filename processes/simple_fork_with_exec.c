/* This program illustrates the use of the fork system call to create a child process which then is 
 * used to execute another process.
 *
 * Author: Naga Kandasamy
 * Date created: 12/22/2008
 * Date modified: 6/23/2018
 *
 * Compile the code as follows: gcc simple_fork_with_exec.c -o simple_fork_with_exec -std=c99 - Wall
 * Execute as follows: ./simple_fork_with_exece
*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

int 
main (int argc, char **argv)
{
    int pid; /* Process ID returned by fork */
    int status;
  
    if ((pid = fork ()) < 0){ /* Fork the process */
        perror ("Fork");
        exit (-1);
    }
    
    if (pid > 0){ /* Parent code */
        printf ("Process ID of the parent is %d. \n", getpid()); /* Print parent's process ID */
    }
  
    else if (pid == 0){ /* Child code */
        printf ("Process ID of the child is %d. \n", getpid ()); /* Print child's process ID */
        sleep (2); /* Sleep for a few seconds */
        execlp ("./hello_world", "hello_world", "4", "6", (char *)NULL); /* Execute the hello world program. */
        exit (0); /* Child exits */
    }

    /* Parent code */
    pid = waitpid (pid, &status, 0); /* Wait for child to terminate */
    printf ("Child has terminated. Parent terminating. \n", pid); 
    exit (0);
}
