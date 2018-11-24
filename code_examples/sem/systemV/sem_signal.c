/* 
 * This program performs the V or signal operation on a semaphore that has been created for 
 * us a priori using create_semaphore.c.
 *
 * Author: Naga Kandasamy, 1/25/2009. Modified: 7/12/2015.
*/

#define _SVID_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#define KEY 2010										/* External identifier for the semaphore. */

int 
main(int argc, char **argv){
  int semid;
  struct sembuf sops; 								/* Data structure to specify the semaphore operation. */

  if ((semid = semget(KEY, 1, 0666)) < 0){		/* Obtain an existing semaphore tagged with identifier KEY. */
	perror("semget");
	exit(EXIT_FAILURE);
  }

  sops.sem_num = 0; 									/* Operate on semaphore 0 in the array of semaphores. */
  sops.sem_op = 1; 									/* Perform the V operation. Add 1 to the semaphore value. */ 
  sops.sem_flg = 0; 									/* Block until the OS completes requested operation. */ 

  if(semop(semid, &sops, 1) < 0){
			 perror("semop");
			 exit(EXIT_FAILURE);
  }

  printf("Successful V operation performed on semaphore %d by process %ld. \n", KEY, (long)getpid());
  exit(EXIT_SUCCESS);
}

