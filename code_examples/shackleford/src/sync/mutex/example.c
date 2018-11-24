#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define LOOPS 1000000

static int val;

void* thread (void* userdata)
{
    int i;

    for (i=0; i<LOOPS; i++)
        val--;

    return NULL;
}


int main (int argc, char** argv)
{
    int i, ret;
    pthread_t worker;

    val = 10;

    ret = pthread_create (&worker, NULL, thread, NULL);
    if (ret) {
        fprintf (stderr, "Failed to create thread!\n");
        exit (EXIT_FAILURE);
    }

    for (i=0; i<LOOPS; i++)
        val++;

    ret = pthread_join (worker, NULL);
    if (ret) {
        fprintf (stderr, "Unable to join with thread!\n");
        exit (EXIT_FAILURE);
    }

    printf ("val = %i\n", val);


    return 0;
}
