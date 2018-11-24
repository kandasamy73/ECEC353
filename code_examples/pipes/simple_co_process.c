/* This program illustrates the use of the pipe system call to set up a UNIX filter. A UNIX filter is a 
 * program that reads from standard input and writes to standard output. Filters are normally connected 
 * linearly, as pipelines in the UNIX shell.
 * 
 * Author: Naga Kandasamy
 * Date created: December 22, 2008
 * Date modified: July 6, 2018
 *
 * Compile the code as follows. gcc -o simple_co_process simple_co_process.c -std=c99 -Wall
*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE 256

int 
main (int argc, char **argv)
{
    int fd0[2], fd1[2]; 
    int pid;
    int n;
  
    if (argc < 2){
        printf ("Usage: %s simple_co_process \"number1 number2\" \n", argv[0]);
        exit (EXIT_SUCCESS);
    }

    /* Create two pipes to create the communication channel */
    if ((pipe (fd0) < 0) || (pipe (fd1) < 0)){         
        perror ("pipe");;
        exit (EXIT_FAILURE);
    }
   
    if ((pid = fork ()) < 0){
        perror ("fork");
        exit (EXIT_FAILURE);
    } 
    else if (pid == 0){ /* Child process */
    close (fd0[1]); /* Close the writing end of fd0 */
    close (fd1[0]); /* Close the reading end of fd1 */

    dup2 (fd0[0], STDIN_FILENO); /* Make the standard input descriptor (STDIN) refer to fd0[0], the reading end of pipe fd0 */
    close (fd0[0]); /* Close the superflous decriptor */

    dup2 (fd1[1], STDOUT_FILENO); /* Make the standard output descriptor (STDOUT) refer to fd1[1], the writing end of pipe fd1 */
    close (fd1[1]);

    execl ("./add2", "add2", (char *)0); /* Execute the add2 program without any command-line arguments */
    exit (EXIT_FAILURE);
  }

  /* Parent process */
  close (fd0[0]); /* Close the reading end of pipe 0 */
  close (fd1[1]); /* Close the writing end of pipe 1 */
  
  /* Read the command line argument. */
  char buffer[BUF_SIZE];
  strcpy (buffer, argv[1]); /* Read in the string input by the user */
  printf ("Parent: writing %d bytes to the pipe. \n", (int) strlen(buffer));
  write (fd0[1], buffer, strlen(buffer)); /* Write strlen(buffer) bytes to the writing end of fd0 */

  n = read (fd1[0], buffer, BUF_SIZE); /* Read n bytes from the reading end of fd1 */
  printf ("Parent: reading %d bytes from the pipe. \n", n);
  buffer[n] = '\0'; // Terminate the string you just read
 
  printf ("Sum = %s \n", buffer); 
 
  exit (EXIT_SUCCESS);
}








