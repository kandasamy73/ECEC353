#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_SIZE 256
#define STDIN 0 
#define STDOUT 1

int 
main (int argc, char **argv)
{
    char buffer[BUF_SIZE];
    int number1, number2;
    int n;

    n = read (STDIN_FILENO, buffer, BUF_SIZE); /* Read n bytes from STDIN */
    buffer[n] = '\0'; /* Terminate the string */
    
    sscanf (buffer, "%d %d", &number1, &number2); /* Read the two integers */
    sprintf (buffer, "%d\n", (number1 + number2)); /* Write the sum as a string */
  
    write (STDOUT_FILENO, buffer, strlen(buffer)); /* Write strlen(buffer) bytes to STDOUT */
  
    exit (EXIT_SUCCESS);
}
