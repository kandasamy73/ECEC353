#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_WORKER_THREADS 500

struct thread_data {
    pthread_t tid;
    unsigned int num;
};

struct workers_state {
    int still_working;

    pthread_mutex_t mutex;
    pthread_cond_t signal;
};


static struct workers_state wstate = {
    .still_working = NUM_WORKER_THREADS,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .signal = PTHREAD_COND_INITIALIZER
};

static unsigned int result[NUM_WORKER_THREADS];


void* worker_thread (void* param)
{

}

int main (int argc, char** argv)
{

}
