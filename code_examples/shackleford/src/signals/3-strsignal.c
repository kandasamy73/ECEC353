/* This simple example show how you can retrieve a string containing
 * the name of a signal by using the strsignal() function supplied by
 * string.h
 *
 * This example installs signal handlers for SIGINT, SIGQUIT, and SIGSEGV
 *
 *   Author: James A. Shackleford
 *     Date: February 18th, 2016
 */

#include <unistd.h>
#include <limits.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void handler(int sig)
{
    printf("\nCaught Signal: %s\n", strsignal(sig));
    exit(EXIT_SUCCESS);
}


int main (int argc, char** argv)
{
    unsigned int array[3];
    unsigned int i;

    signal(SIGINT, handler);
    signal(SIGQUIT, handler);
    signal(SIGSEGV, handler);

    /* This will eventually write past the end of the array and into a
     * region of memory that we do not own. */
    for (i=0; i<4000; i++) {
        array[i] = 1;
        printf("%u\n", i);
        sleep(1);
    }

    return 0;
}
