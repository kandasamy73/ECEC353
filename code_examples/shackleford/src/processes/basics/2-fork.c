/* A simple forking example
 *
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * This program demonstrates how to fork a process, thereby creating two
 * processes in total: a parent process and a child process.
 *
 * After forking, the child and parent processes are identical with the
 * exception of their process id numbers (pids) and the return value they
 * receive from the fork() system call.  Note that execution will continue
 * at the exact same point for both the child and parent processes.  This is a
 * consequence of being exact duplicates and, therefore, having the same
 * program counter value.
 *
 * fork() will return:
 *   negative value .... something went wrong (ex. not enough memory)
 *   0 ................. to the newly created child process
 *   child's pid ....... to the parent process
 *
 * Therefore, in order for the child and parent processes to perform different
 * functionality, the first thing that should be done after forking is for
 * each process to check its return value from the fork() syscall.
 *
 * This example attempts to explicitly demonstrate that the child receives a
 * COPY of the parent process's address space.  As a result, variable scoping
 * (i.e. global, local) does not have an affect with regards to sharing data
 * between the parent and child processes.  Each process gets its own COPY of
 * everything -- after forking, changing a variable in the parent's address
 * will not have an affect on the copy of that variable in the child's address
 * space.  Likewise, the converse is also true.  Variable scope continues to
 * only have meaning with respect to what can be read/written from within a
 * particular function or statement block.
 *
 * Compile using:
 *   gcc -o 2-fork 2-fork.c
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int global = 100;

int main(int argc, char** argv)
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

        waitpid(pid, &child_ret, 0);

        printf("fork -- %i\n", pid);
        printf("parent -- global: %i, local: %i\n", global, local);
        printf("parent -- child exited with code %i\n", child_ret);

        return 0;
    } else {
        /* only executed by the CHILD process */
        global++;
        local++;

        printf("child -- global: %i, local: %i\n", global, local);
        exit (0);
    }
}
