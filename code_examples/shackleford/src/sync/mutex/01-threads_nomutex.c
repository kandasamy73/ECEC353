#include <stdio.h>
#include <pthread.h>

#define OPS 10000

int val = 10;

void* thread (void* p)
{
    int i;

    for (i=0; i<OPS; i++) {
        val--;
    }
}

int main (int argc, char** argv)
{
    int i;
    pthread_t worker;

    pthread_create (&worker, NULL, thread, NULL);

    for (i=0; i<OPS; i++) {
        val++;
    }

    pthread_join (worker, NULL);

    printf ("val = %i\n\n", val);

    return 0;
}
