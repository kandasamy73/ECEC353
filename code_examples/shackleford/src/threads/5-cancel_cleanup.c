/* A simple example demonstrating how to set up cleanup functions.
 * Cleanup functions are executed:
 *    * upon thread cancellation
 *
 *    * when popped off the cancellation stack using
 *      pthread_cleanup_pop(arg) when arg is a non-zero value
 *
 *    * when a thread exits using pthread_exit()
 *
 * NOTE: As shown below, pthread_cleanup_push() and
 *       pthread_cleanup_pop() mush be deployed in pairs!
 *
 *       Otherwise, you will get cryptic compiler errors.
 *       (Try removing a pthread_cleanup_pop() line and compiling)
 *
 *   Author: James A. Shackleford
 *     Date: January 28th, 2016
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

void cleanup_a(void* arg)
{
    unsigned int** matrix = arg;

    printf("cleanup_a: freeing array of pointers\n");
    free (matrix);
}

void cleanup_b(void* arg)
{
    int i;
    unsigned int** matrix = arg;

    printf("cleanup_b: free()ing arrays of ints\n");

    for (i=0; i<10; i++)
        free(matrix[i]);
}

void* thread (void* arg)
{
    int ret, i, j;
    unsigned int** matrix;
    char* tid_ret;

    /* This thread allocates dynamic memory and uses cleanup functions
     * to ensure that the heap memory gets released!
     *
     * Cleanup functions get run in a FILO fashion if we get cancelled!
     * NOTE: We must define pthread_cleanup_push() and
     *       pthread_cleanup_pop() in PAIRS!!!
     */

    printf("!! Thread -- setting cancellation state: DISABLED\n");
    ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcancelstate()\n");
        exit(EXIT_FAILURE);
    }

    matrix = malloc (10 * sizeof(*matrix));
    if (!matrix)
        exit(EXIT_FAILURE);

    pthread_cleanup_push(cleanup_a, matrix);

    for (i=0; i<10; i++) {
        matrix[i] = malloc (5 * sizeof(*matrix[i]));
        if (!matrix[i])
            exit(EXIT_FAILURE);
    }

    pthread_cleanup_push(cleanup_b, matrix);

    printf("!! Thread -- setting cancellation state: ENABLED\n");
    ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcancelstate()\n");
        exit(EXIT_FAILURE);
    }

    /* Do some work on matrix elements */
    while (1) {
        for (j=0; j<5; j++) {
            for (i=0; i<10; i++) {
                matrix[i][j]++;
            }
        }
        /* Cancellation mode is not async! We have to check! */
        pthread_testcancel();
    }

    /* Thread should be canceled by now */
    printf("!! Thread -- Pointless Matrix Calculations Complete!\n");

    pthread_cleanup_pop(1); /* cleanup_b() */
    pthread_cleanup_pop(1); /* cleanup_a() */

    /* The thread's stack will be destroyed when we return, so we cannot
     * return the address of a local variable.  Let's get some heap
     * memory and put a return value in there instead.  Joiner is
     * responsible for free()ing this. */
    tid_ret = malloc (strlen("Success!")+1);
    strcpy (tid_ret, "Success!");

    return tid_ret;
}


int main (int argc, char** argv)
{
    pthread_t tid;
    void* tid_ret;
    int ret;

    ret = pthread_create(&tid, NULL, thread, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }
    printf ("Created new thread (tid: %x)\n", (unsigned int)tid);

    /* Give the thread a chance to get into an interesting state */
    sleep(2);

    printf("Sending cancellation request\n");
    ret = pthread_cancel(tid);
    if (ret) {
        fprintf(stderr, "error -- pthread_cancel()\n");
        pthread_join(tid, NULL);
        exit(EXIT_FAILURE);
    }

    ret = pthread_join(tid, &tid_ret);
    if (tid_ret == PTHREAD_CANCELED) {
        printf ("Thread (tid: %x) successfully cancelled.\n", (unsigned int)tid);
    } else {
        printf ("Thread (tid: %x) was not cancelled!\n", (unsigned int)tid);
        printf ("  Returned: %s\n", (char*)tid_ret);
        free (tid_ret);
    }

    return 0;
}
