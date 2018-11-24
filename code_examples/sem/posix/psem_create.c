/* Program illustrating the steps involved in creating a named semaphore. 
 *
 * Source: Michael Kerrisk, The Linux Programming Interface
 *
 * Author: Naga Kandasamy
 * Date created: July 22, 2018
 *
 * Compile as follows:
 * gcc -o psem_create psem_create.c -std=c99 -Wall -lpthread
 *
 * To create a semaphore called demo: ./psem_create -cx /demo
 *
 * */

#define _POSIX_C_SOURCE 2 /* Enable definitions for getopt() */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>


static void 
usage_error (const char *program_name) {
    fprintf (stderr, "Usage: %s [-cx] sem-name [octal-perms [value]]\n", program_name);
    fprintf (stderr, "\t -c \t Create semaphore (O_CREAT)\n");
    fprintf (stderr, "\t -x \t Create exclusively (O_EXCL)\n");
    exit (EXIT_FAILURE);
}

int 
main (int argc, char **argv)
{
    int flags, opt;

    flags = 0;
    while ((opt = getopt (argc, argv, "cx")) != -1) {
        switch (opt) {
            case 'c': 
                flags |= O_CREAT;
                break;

            case 'x':
                flags |= O_EXCL;
                break;

            default:
                usage_error (argv[0]);
        }
    }

    if (optind >= argc)
        usage_error (argv[0]);

    /* Default permissions are rw------- and the semaphore initialization value is 0 */
    mode_t perms = S_IRUSR | S_IWUSR;
    if (argc > optind + 1)
        perms = atoi (argv[optind + 1]);
    
    unsigned int value = 0;
    if (argc > optind + 2)
        value = atoi (argv[optind + 2]);

    sem_t *sem = sem_open (argv[optind], flags, perms, value);
    if (sem == SEM_FAILED) {
        perror ("sem_open");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}

