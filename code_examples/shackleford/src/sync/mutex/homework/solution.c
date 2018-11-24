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

static int result[NUM_WORKER_THREADS];


void* worker_thread (void* param)
{
    struct thread_data* tdata = (struct thread_data*)param;

    result[tdata->num] = tdata->num * tdata->num;

    pthread_mutex_lock(&wstate.mutex);
    wstate.still_working--;
    pthread_mutex_unlock(&wstate.mutex);

    pthread_cond_signal(&wstate.signal);

    pthread_exit(0);
}

int main (int argc, char** argv)
{
    unsigned int i, total;
    struct thread_data *threads;

    threads = malloc(NUM_WORKER_THREADS * sizeof(struct thread_data));

    for (i=0; i<NUM_WORKER_THREADS; i++) {
        threads[i].num = i;
        pthread_create(&threads[i].tid, NULL, worker_thread, &threads[i]);
        pthread_detach(threads[i].tid);
    }

    pthread_mutex_lock(&wstate.mutex);
    while (wstate.still_working != 0) {
        printf(".");
        pthread_cond_wait(&wstate.signal, &wstate.mutex);
    }
    pthread_mutex_unlock(&wstate.mutex);
    printf("\n");

    free(threads);

    total = 0;
    for (i=0; i<NUM_WORKER_THREADS; i++) {
        total += result[i];
    }

    printf("Final Total: %u\n", total);

    total = 0;
    for (i=0; i<NUM_WORKER_THREADS; i++) {
        total += i*i;
    }

    printf("Check: %u\n", total);

    return 0;
}
