#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

#define OPS 1000000

int val;
int num;

/* MUTEX FOR VAL */
static pthread_mutex_t mtx_val = PTHREAD_MUTEX_INITIALIZER;

/* MUTEX FOR NUM */
static pthread_mutex_t mtx_num = PTHREAD_MUTEX_INITIALIZER;

void* thread (void* p)
{
    int i;

    for (i=0; i<OPS; i++) {
        pthread_mutex_lock(&mtx_num);
        pthread_mutex_lock(&mtx_val);
        val--;
        num++;
        pthread_mutex_unlock(&mtx_val);
        pthread_mutex_unlock(&mtx_num);
    }
}



int main (int argc, char** argv)
{
    int ret, i;
    pthread_t worker;

    val = 10;
    num = 20;

    ret = pthread_create (&worker, NULL, thread, NULL);
    if (ret) {
        fprintf (stderr, "error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }


    for (i=0; i<OPS; i++) {
        pthread_mutex_lock(&mtx_num);
        pthread_mutex_lock(&mtx_val);
        val++;
        num--;
        pthread_mutex_unlock(&mtx_val);
        pthread_mutex_unlock(&mtx_num);
    }


    ret = pthread_join(worker, NULL);
    if (ret) {
        fprintf (stderr, "error -- pthread_join()\n");
        exit(EXIT_FAILURE);
    }

    printf("val = %i\n\n", val);
    return 0;
}
