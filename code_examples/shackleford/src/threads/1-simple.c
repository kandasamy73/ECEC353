/* A simple example demonstrating how to create a thread using the POSIX
 * threading library pthreads.
 *
 *   Author: James A. Shackleford
 *     Date: January 15th, 2015
 *
 * Threads provide a "light weight" context for performing computation.  By
 * contrast, processes are "heavyweight" in nature.  The "weight" referred to
 * is the amount of resources that are dedicated to the context.  Processes
 * generally consist of:
 *
 *   -- a register file state
 *   -- a stack (containing function call stack, local variables, etc)
 *   -- a virtual address space
 *   -- a text segment (containing the code)
 *   -- a file descriptor table (list of open files)
 *   -- a signal table (containing signal handlers)
 *
 * So, a process is fairly "heavyweight," both in its creation and in its
 * management.  Recall that the operating system's scheduler is in charge of
 * managing the CPU, which generally consists of dividing processor time into
 * slices.  The scheduler then assigns contexts to these short time slices and
 * is in charge of constantly swapping these contexts in and out of the CPU
 * very rapidly so as to create the illusion of parallel task execution on
 * single core systems.  This ensures that processing resources are used
 * efficiently.  This process of swapping processing jobs in and out of the
 * CPU is called "context switching" and can be time consuming for
 * "heavyweight" processes since there is more state information to save and
 * restore with each context switch.
 *
 * Threads, on the other hand, are more "lightweight" in that they share a
 * majority of their resources with a process.  Threads generally have their
 * own:
 *
 *   -- register file (including the program counter and stack pointer)
 *   -- stack
 *
 * and share the following provided by the process:
 *
 *   -- virtual address space
 *   -- text segment (containing the code)
 *   -- file descriptor table (list of open files)
 *
 * Note that a process always consists of at least one thread of execution.
 *
 * Consequently, the CPU scheduler is able to context switch between threads
 * in a process more rapidly than it can between processes since the only
 * necessary operations are to modify the program counter and stack pointer in
 * the CPU register file.
 *
 * Note that these are generalizations.  As we have discussed in class,
 * threading implementations can vary greatly between operating systems
 * and different versions of the same operating system
 *
 * Compile using:
 *   gcc -o 1-simple 1-simple.c -lpthreads
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/* Let's define a structure to contain the parameters that we will pass to
 * the worker thread we will be creating
 */
struct foo_args {
    int a;
    float b;
    unsigned int c;
};


/* Unlike process creation, threads are provided with the address of a
 * function to execute upon creation.  This is known as a "function pointer"
 * or "callback function," depending on context.
 *
 * The following is a callback function definition that can be specified as
 * the entry point for threads we will create within main()
 */
void* foo (void* arg)
{
    pthread_t tid_self;
    struct foo_args* params;

    tid_self = pthread_self();

    /* Our arguments pointer was cast from a struct foo_args* to a void*
     * by the pthreads API in order to provide a generic argument passing
     * interface.  We need to cast it back before we can dereference it!
     */
    params = (struct foo_args*)arg;

    printf("---------------------\n");
    printf("tid(%x) reporting in:\n", (unsigned int)tid_self);
    printf("  a: %i\n", params->a);
    printf("  b: %g\n", params->b);
    printf("  c: %u\n", params->c);
    printf("---------------------\n");

    /***************
     * C Tip: Note that the following two statements are equivalent:
     *    parms->c = 100;
     *    (*parms).c = 100;
     ***/

    return NULL;
}


int main (int argc, char** argv)
{
    pthread_t tid_main;
    pthread_t tid_foo;
    struct foo_args args;
    int ret;

    /* Get the thread id for this thread */
    tid_main = pthread_self();

    /* Populate the arguments for our thread */
    args.a = -42;
    args.b = 12.34;
    args.c = 10;

    /***************
     * C Tip: Note that the following two statements are equivalent:
     *    args.a = -42;
     *    (&args)->a = -42;
     ***/

    /* Create a new thread
     *   Once the thread begins execution, it will begin execution in function
     *   foo() with a pointer to our foo_args struct (args) as the parameter
     *   to foo().
     */
    ret = pthread_create(&tid_foo, NULL, foo, &args);
    if (ret) {
        fprintf(stderr, "error -- pthread_create() failed.\n");
        exit(EXIT_FAILURE);
    }

    /* Provide the user will some feedback printed to stdout */
    printf("tid(%x) has created a thread with tid(%x)\n",
            (unsigned int)tid_main,
            (unsigned int)tid_foo);
    printf("Waiting for tid(%x) to complete...\n", (unsigned int)tid_foo);

    /* Wait for thread foo to finish */
    ret = pthread_join(tid_foo, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_join() failed.\n");
        exit(EXIT_FAILURE);
    }

    printf("done.\n\n");
    return 0;
}
