/* Program deletes a previously created message queue. 
 *
 * Author: Naga Kandsamy
 * Date created: july 12, 2018
 *
 * Source: M. Kerrisk, The Linux Programming Interface. 
 *
 * Compile as follows: gcc -o delete_mq delete_mq.c -std=c99 -Wall -lrt
 */

#define _POSIX_C_SOURCE 2

#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int 
main (int argc, char **argv)
{
    if (argc != 2 || strcmp (argv[1], "--help") == 0){
        printf ("Usage: %s mq-name \n", argv[0]);
        exit (EXIT_FAILURE);
    }

    if (mq_unlink (argv[1]) == -1){
        perror ("mq_unlink");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}


