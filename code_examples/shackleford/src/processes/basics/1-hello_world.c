/* The famous "Hello World!" program
 * 
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * Here we are creating a single process that writes to stdout (the terminal)
 * and terminates with return code 0.
 *
 * Note that all processes running on the system have a parent process (except
 * for the init process, whose pid is 1).  The parent process for our "Hello
 * World!" process here will be the process that started it.  In most cases,
 * this will be the terminal shell process that you use to enter commands.
 * These days this is usually the bash command shell.
 *
 * Note that when you run this program from the bash terminal prompt using:
 *   $ ./hello_world
 *
 * The bash process forks to create a child process, which will be a "copy" of
 * the bash process. The bash parent process then repopulates this child
 * process's address space with the content of our hello world program using
 * the exec() system call provided by the kernel.
 *
 * Also note that the operating system's kernel has control over the display.
 * Consequently, the printf() function is a wrapper around the write() system
 * call provided by the kernel, which is used to write to file descriptors.
 * Note that in UNIX style operating systems, devices are exposed to the
 * userspace as files.  Here, the primary display is the stdout file
 * descriptor and printf() is a wrapper function provided by the C Standard
 * Library via stdio.h
 *
 * Compile using:
 *   gcc -o 1-hello_world 1-hello_world.c
 */

#include <stdlib.h>
#include <stdio.h>

int main (int argc, char** argv)
{
    printf ("Hello world!\n");
    exit (1);
}
