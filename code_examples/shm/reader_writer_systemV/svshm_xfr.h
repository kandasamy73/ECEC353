#ifndef _SVSHM_XFR_H
#define _SVSHM_XFR_H


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include "binary_sems.h" 				/* Header file for the binary semaphore operations. */

#define SHM_KEY 0x1234					/* Key for shared memory segment. */ 
#define SEM_KEY 0x5678					/* Key for semaphore set. */

#define OBJ_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)	
												/* Permissions for our IPC objects. */

#define WRITE_SEM 0						/* Semaphore numbers for the writer and the reader. */
#define READ_SEM 1

#define BUF_SIZE 1024				/* Size of the transfer buffer in bytes. */

struct shmseg {								/* Define the structure of our shared memory segment. */
    int count; 						/* Number of bytes used in the shared buffer. */
    char buf[BUF_SIZE];		/* Data being transferred. */
};

#endif





