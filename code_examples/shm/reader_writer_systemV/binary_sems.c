/* Implementation of the binary semaphore functions. Each function takes as inputs the semaphore ID 
 * and the semaphore number within the semaphore set. The creation and deletion of the semaphores 
 * must be done in the writer program. 
 *
 * Author: Naga Kandasamy
 * Date: 7/9/2015
 *
 * Notes: Code taken from: Michael Kerrisk, The Linux Programming Interface, No Starch Press, 2010.
 *
 */

#define _XOPEN_SOURCE

#include <sys/types.h>
#include <sys/sem.h>
#include "semun.h"
#include "binary_sems.h"


int						/* Initialialize the semphore to 1, that is, to "available". */
initSemAvailable (int semid, int semNum)
{
    union semun arg;
    arg.val = 1;
    return semctl(semid, semNum, SETVAL, arg);
}

int						/* Initialialize the semphore to 0, that is, to "in use". */
initSemInUse (int semid, int semNum)
{
    union semun arg;
    arg.val = 0;
    return semctl(semid, semNum, SETVAL, arg);
}

int						/* Reserve a semaphore (blocking). Decrement value by 1. */ 
reserveSem (int semid, int semNum)
{
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = -1; 
    sops.sem_flg = 0;
    return semop(semid, &sops, 1);
}

int						/* Release a semaphore. Increment value by 1. */ 
releaseSem (int semid, int semNum)
{
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = 1; 
    sops.sem_flg = 0;
    return semop(semid, &sops, 1);
}



 
