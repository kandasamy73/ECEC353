/* A simple example demonstrating how to create a detached thread using the
 * POSIX threading library pthreads.
 *
 *   Author: James A. Shackleford
 *     Date: January 15th, 2015
 *
 * Similar to process management, threads that have run to completion must be
 * reaped.  In thread lingo, this is known as "joining."
 *
 * The reason for joining is similar to the reason for wait()ing for a process
 * to complete: the threading implementation must maintain an entry in the
 * thread table containing (1) the thread's tid and (2) the return code of the
 * thread until the join is performed.  Once the return code is retrieved via
 * the joining operation, the thread can be reaped from the thread table; thus
 * freeing up resources.
 *
 * As we talked about in class, in many pthreads implementations the return
 * code is stored in the thread's attribute block, which is stored in the
 * beginning of the thread's stack.  As a result, not reaping the thread
 * (i.e. neglecting to join) will result in the thread's entire stack not
 * being freed, which can be a huge waste of resources.
 *
 * However, in many cases, the return code of the thread either doesn't matter
 * or even exist (void functions don't have return codes).  Unlike processes,
 * you can specify that you don't care about the return code of the thread.
 * In such cases, upon completion the thread will be automatically removed
 * from the thread table -- meaning that you don't have to perform and wait
 * for the join.  Such threads are called detached threads.
 *
 * By default, newly created threads are not detached.  To create a detached
 * thread, you must modify the default values in the thread's attribute block.
 * I show two methods for doing this here.
 *
 * Compile using:
 *   gcc -o 2-detach 2-detach.c -lpthread
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

void print_attachment (pthread_t tid, unsigned int tnum)
{
    switch (pthread_join(tid, NULL)) {
        case EINVAL:
            printf ("Thread #%u: DETACHED\n", tnum);
            break;
        case 0:
            printf ("Thread #%u: JOINABLE\n", tnum);
            break;
        default:
            printf ("error -- pthread_join()\n");
    }
}


void* thread (void* arg)
{
    /* Threads do nothing but sleep for 1 second */
    sleep (1);

    return NULL;
}


int main (int argc, char** argv)
{
    pthread_t tid1;
    pthread_t tid2;
    pthread_t tid3;
    pthread_attr_t attr;
    int ret;

    /* tid1 will be JOINABLE */
    ret = pthread_create(&tid1, NULL, thread, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }
    print_attachment(tid1, 1);

    /* tid2 will be DETACHED using pthread_detach() */
    ret = pthread_create(&tid2, NULL, thread, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }
    ret = pthread_detach(tid2);
    if (ret) {
        fprintf(stderr, "error -- pthread_detach()\n");
        exit(EXIT_FAILURE);
    }
    print_attachment(tid2, 2);

    /* tid3 will be DETACHED by editing the thread attributes */
    ret = pthread_attr_init(&attr);
    if (ret) {
        fprintf(stderr, "error -- pthread_attr_init()\n");
        exit(EXIT_FAILURE);
    }
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    ret = pthread_create(&tid3, &attr, thread, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }
    print_attachment(tid3, 3);

    pthread_attr_destroy(&attr);

    return 0;
}
