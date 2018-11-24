/* This program demonstrates a zombie process.
 * 
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * A zombie process is a child process that has terminated but whose parent
 * process has not yet called wait(), which retrieves the child's return
 * code and removes the child process from the kernel's process table.
 *
 * Do not confuse a zombie process with an orphan process!
 *
 * An orphaned process is a child process whose parent has terminated before
 * the child has.  In Linux, orphaned processes are reassigned to the init
 * process (pid 1) by the kernel.  The init program periodically cleans up
 * orphan processes by performing the necessary wait() call for the orphans it
 * inherits.
 *
 * Compile using:
 *   gcc -o 4-zombie 4-zombie.c
 */

#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int 
main(int argc, char** argv)
{
    int local = 5;

    pid_t pid;

    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "error -- failed to fork()");
        return 1;
    }

    if (pid > 0) {
        /* only executed by the PARENT process */
        int child_ret;

        /* The parent will never get out of this infinite loop, so it will
         * never get to wait(), which removes the child from the process
         * table.*/
        while (1);

        wait(&child_ret);
        printf("parent -- child exited with code: %i.  Removed child from process table!\n", child_ret);

    } else {
        /* only executed by the CHILD process */
        printf("child -- blargh!  Becoming a zombie!\n");
        exit (0);
    }
}

