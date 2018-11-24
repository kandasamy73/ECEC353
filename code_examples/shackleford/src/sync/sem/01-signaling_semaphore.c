#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

/* Make calls to sem_init() more readable */
#define THREAD_LEVEL_SHARING 0
#define PROCESS_LEVEL_SHARING 1

struct signal {
    int active;  /* predicate */

    pthread_mutex_t mutex; /* protects access to predicate */
    sem_t semaphore;       /* used to signal when predicate is modified */
};

/* Declare a global 'struct signal' named 'sig' */
static struct signal sig;

void* thread (void* param)
{
    /* Wait 5 seconds before sending signal */
    sleep(5);

    pthread_mutex_lock(&sig.mutex);
    sig.active = 1;
    pthread_mutex_unlock(&sig.mutex);

    /* increment the semaphore -- signals main thread */
    sem_post(&sig.semaphore); /* ups */
    printf("Signal Thread -- upped semaphore\n");

    pthread_exit(0);
}


int main (int argc, char** argv)
{
    pthread_t tid;

    /* Initialize sig */
    sig.active = 0;
    pthread_mutex_init(&sig.mutex, NULL);
    sem_init(&sig.semaphore, THREAD_LEVEL_SHARING, 0);

    pthread_create(&tid, NULL, thread, NULL);

    /* --- CRITICAL SECTION ----------- */
    pthread_mutex_lock(&sig.mutex);
    while (!sig.active) {
        printf("Main Thread   -- waiting for signal\n");

        /* sem_wait() will:
         *  -- Block if the semaphore is zero
         *  -- If not zero, sem_wait will decrement the semaphore and return */
        pthread_mutex_unlock(&sig.mutex);
        sem_wait(&sig.semaphore);  /* down */
        pthread_mutex_lock(&sig.mutex);

        printf ("Main Thread   -- received signal via binary semaphore\n");
    }

    printf ("Main Thread   -- we are active!\n");
    pthread_mutex_unlock(&sig.mutex);
    /* -------------------------------- */

    pthread_join(tid, NULL);

    /* free data structures internal to the semaphore */
    sem_destroy(&sig.semaphore);

    return 0;
}
