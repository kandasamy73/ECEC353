/*
 * Author: Michael Lui
 *
 * Demonstrates basic pthread creation with functions that
 * increment their input value.
 *
 * Atomic operations are required to prevent race conditions
 * that happen when two threads modify the same value.
 */

#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"
// #include "stdatomic.h" //missing in gcc 4.8.5

/* Dereference the pointer and increment.
 *
 * Multiple threads will cause a race condition and unexpected behavior */
void* race_func(void *ptr)
{
    for (int i=0; i<10000; ++i)
        (*(int*)(ptr))++;
    return NULL;
}

/* Dereference the pointer and atomically increment 
 *
 * Multiple threads are fine because our increment is atomic.
 * Using gcc atomics because the Xunil cluster has an old gcc version 
 * that does not support C11 atomics (gcc 4.9 is fine) */
void* atomic_func(void *ptr)
{
    for (int i=0; i<10000; ++i)
        __sync_fetch_and_add((int*)ptr, 1);
    return NULL;
}

int main(int argc, char *argv[])
{
    int race_val = 0;
    int atomic_val = 0;
    int num_threads = 0;

    /* expect number of threads for each method (so half the total threads)
     * as an argument to the executable */
    if (argc == 2)
    {
        /* we'll assume correct formatting (as a number) */
        num_threads = atoi(argv[1]);
    }

    /* Don't allow users to blow up the system
     * 100 threads should be sufficient to manifest race conditions */
    if (num_threads < 1 || num_threads > 100)
    {
        printf("Required argument: {1-100} threads\n");
        return EXIT_FAILURE;
    }

    /* thread 'ids' */
    pthread_t t_race[num_threads];
    pthread_t t_atom[num_threads];

    /* Create threads which run 'race condition' and 'atomic' 'increment functions. */
    for (int i=0; i<num_threads; ++i)
        pthread_create(&t_race[i], NULL, race_func, &race_val);
    for (int i=0; i<num_threads; ++i)
        pthread_create(&t_atom[i], NULL, atomic_func, &atomic_val);

    /* Wait until threads have completed */
    for (int i=0; i<num_threads; ++i)
        pthread_join(t_race[i], NULL);
    for (int i=0; i<num_threads; ++i)
        pthread_join(t_atom[i], NULL);

    printf("race_val is %d\n", race_val);
    printf("atomic_val is %d\n", atomic_val);

    return EXIT_SUCCESS;
}
