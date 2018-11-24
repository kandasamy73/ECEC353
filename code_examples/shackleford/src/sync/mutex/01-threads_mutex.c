#include <stdio.h>
#include <pthread.h>

#define OPS 10000

int val = 10;
pthread_mutex_t mtx_val = PTHREAD_MUTEX_INITIALIZER;

void* thread (void* p)
{
    int i;

    for (i=0; i<OPS; i++) {
        pthread_mutex_lock (&mtx_val);
        val--;
        pthread_mutex_unlock (&mtx_val);
    }
}

int main (int argc, char** argv)
{
    int i;
    pthread_t worker;

    pthread_create (&worker, NULL, thread, NULL);

    for (i=0; i<OPS; i++) {
        pthread_mutex_lock (&mtx_val);
        val++;
        pthread_mutex_unlock (&mtx_val);
    }

    pthread_join (worker, NULL);

    printf ("val = %i\n\n", val);

    return 0;
}
