/* Shows how to write a simple command-line interpreter. The parent accepts user comands and 
 * forks a child process to execute those commands. 
 *
 * Author: Naga Kandasamy
 * Date created: 1/4/2009
 * Date modified: 6/23/2018
 *
 * Compile the code as follows: gcc simple_interpreter.c -o simple_interpreter -std=c99 -Wall

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_LEN 256

int 
main (int argc, char **argv)
{
    char buffer[BUF_LEN];
    int pid;
    int status; 
  
    printf ("$ "); /* Simulate a shell prompt */
    while ((fgets(buffer, BUF_LEN, stdin)) != NULL){
        buffer[strlen(buffer) - 1] = '\0'; /* Null terminate the string */

        if (!strcmp(buffer, "quit")){
            printf ("Bye bye. \n");
            exit (0);
        }

        if ((pid = fork ()) < 0){
            perror ("Fork");
            exit (0);
        } 
        else if (pid == 0){ /* Child code */
            /* Execute the program supplied by the user. 
             * Note: our ultra-simple design won't work if the supplied program requires command-line arguments
             * */
            execlp (buffer, buffer, (char *) NULL);       
            exit (0);
        }
    
        /* Parent code */
        if((pid = waitpid (pid, &status, 0)) < 0)
            printf ("Error executing shell command! \n");
        
        printf ("$ ");
    }
}
