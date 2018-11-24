/* A simple forking with exec() example
 * 
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * This program demonstrates how to fork a process and overwrite it with the
 * image of a new program.  This is similar to the procedure performed by
 * a terminal shell when you execute a program from the command line.
 *
 * This example uses vfork() instead of fork().  Historically, the difference
 * between the two commands is that vfork will create a child process that
 * is a copy of the parent process without making a duplicate of the entire
 * parent process's address space.  This is perfect for us, because in this
 * example, we will be overwriting the address space of the child anyway with
 * that of a new program via the exec() syscall.
 *
 * INFORMED NERD NOTE:
 *   In Linux, however, vfork() and fork() are actually different names for the
 *   same forking implementation.  The Linux kernel is very smart and will have
 *   forked processes internally share memory pages until one of the two
 *   processes WRITES to a variable, which would then cause the contents of the
 *   parent and child address spaces to diverge.  So, Linux continues to share
 *   what is common between the two processes (internally, hidden from you) but
 *   will create copies in the child's address space of items that have been
 *   written to (and have, therefore, changed).  This is known as a
 *   copy-on-write technique and is designed to use RAM more efficiently
 *   by minimizing the storage of duplicate information.  So, technically,
 *   every fork() is the same as a vfork() on a modern Linux system.
 *   Consequently, vfork() is marked for eventual removal and its use is not
 *   recommended.  However, here we use it to academically demonstrate the
 *   idea of not needing to copy over the address space of the parent process
 *   if you are about to overwrite it using exec() anyway.
 *
 * Compile using:
 *   gcc -o 3-vfork-exec 3-vfork-exec.c
 *
 * NOTE:
 *   This program will attempt to execute the hello_world.c example, so please
 *   be sure it is compiled before attempting to run this example.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    pid_t pid;

    pid = vfork();

    if (pid < 0) {
        fprintf(stderr, "error -- failed to vfork()");
        return 1;
    }

    if (pid > 0) {
        /* only executed by the PARENT process */
        int child_ret;

        printf("parent -- pid: %i\n", getpid());
        waitpid(pid, &child_ret, 0);
        printf("parent -- child exited with code %i\n", child_ret);

        return 0;
    } else {
        /* only executed by the CHILD process */

        printf("child -- pid: %i\n", getpid());
        execlp("./1-hello_world", "hello_world", NULL);

        /* This will not execute since it no longer exists in the process
         * address space of the child after the exec*() call successfully
         * rewrites the address space with the contents of the hello_world
         * program. */
        printf("child -- Hello?  Still there World?");
    }
}
