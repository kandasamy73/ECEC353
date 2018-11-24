#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_WAIT_THREADS 5

struct signal {
    int active;

    pthread_mutex_t mutex;
    pthread_cond_t condition;
};

struct signal sig = {
    .active = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER,
    .condition = PTHREAD_COND_INITIALIZER
};


static void* signal_thread (void* param)
{
    /* Give the waiting threads some time to check the predicate
     * (be unhappy with the value) and block on pthread_cond_wait() */
    sleep(5);

    pthread_mutex_lock(&sig.mutex);
    /* --- CRITICAL SECTION ------------- */
    sig.active = 1;

    printf ("Signal Thread -- I changed the shared resource! Signaling all other threads!\n");
    pthread_cond_signal (&sig.condition);
    /* ---------------------------------- */
    pthread_mutex_unlock(&sig.mutex);


    pthread_exit(0);
}

static void* wait_thread (void* param)
{
    unsigned int* thread_num = (unsigned int*)param;

    pthread_mutex_lock(&sig.mutex);
    /* --- CRITICAL SECTION ------------- */
    while (!sig.active) {
        /* Note the use of the while() loop to check the predicate
         * instead of an if statement.  This is necessary due to the
         * "Spurious Wakeup Problem". */
        printf("Thread #%u -- predicate not in desirable state\n", *thread_num);
        pthread_cond_wait(&sig.condition, &sig.mutex);
    }

    printf("Thead #%u -- I like the predicate! (and I'm the only unblocked thread!)\n", *thread_num);

    pthread_cond_signal (&sig.condition);

    /* ---------------------------------- */
    pthread_mutex_unlock(&sig.mutex);

    pthread_exit(0);
}

int main (int argc, char** argv)
{
    int i;
    pthread_t sthread_id;
    pthread_t wthread_id[NUM_WAIT_THREADS];
    unsigned int wthread_num[NUM_WAIT_THREADS];

    /* Create some threads that will wait for our signal */
    for (i=0; i<NUM_WAIT_THREADS; i++) {
        wthread_num[i] = i;
        pthread_create(&wthread_id[i], NULL, wait_thread, &wthread_num[i]);
    }

    /* Create a thread that will send the signal */
    pthread_create(&sthread_id, NULL, signal_thread, NULL);

    /* Wait for thread to finish & reap */
    for (i=0; i<NUM_WAIT_THREADS; i++)
        pthread_join(wthread_id[i], NULL);

    pthread_join(sthread_id, NULL);

    return 0;
}
