/* A simple "hello world" program. 
 * 
 * The program takes two integers provided to it as command-line arguments, adds them, and displays the result.
 * 
 * Compile as: gcc -o hello_world hello_world.c -std=c99 -Wall 
 * 
 * */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int 
main (int argc, char *argv[])
{
    /* Check for the correct number of arguments. */
    if (argc < 3){
        printf ("Usage: hello_world <integer 1> <integer 2> \n");
        exit (0);
    }

    /* Read in the command-line arguments and convert them to integers. 
     * Note: We are being lazy here by not perfroming any error checking.
     * */
    int a = atoi (argv[1]);
    int b = atoi (argv[2]);
    pid_t process_id = getpid ();
    
    printf ("Hello from process %d. \n", process_id);
    printf ("I have been asked to add integers %d and %d. \n", a, b);
    printf ("The answer is %d. \n", a + b);
    printf ("Bye bye. \n");
  
    exit (0);
}
