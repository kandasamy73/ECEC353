/* Header file used by both the server and client programs */

#ifndef _SHM_XFR_H_
#define _SHM_XFR_H_

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1
#define SHM_KEY 3228 /* This is the well-known key for the shared memory area */
#define SEM_KEY 32281 /* This semaphore synchronizes access to shared memory */

/* Definition of the structure stored in shared memory. */
#define BUF_LEN 1000

typedef struct shm_struct_t {
int client_id;
char buf[BUF_LEN];

} SHM_STRUCT;

#endif
