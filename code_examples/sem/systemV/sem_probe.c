/* 
 * This program performs the P or probe operation on a semaphore that has been created for 
 * us a priori using create_semaphore.c.
 *
 * Author: Naga Kandasamy, 1/25/2009. Modified: 7/12/2015.
*/

#define _XVID_SOURCE
#define _XOPEN_SOURCE 
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>

#define KEY 2010													/* External identifier for the semaphore. */

int 
main(int argc, char **argv){
  int semid; 
  struct sembuf sops; 								/* Structure to specify the semaphore operation. */

  if ((semid = semget(KEY, 1, 0666)) < 0){
	perror("semget");
	exit(EXIT_FAILURE);
  }

  while(1){															/* Perform the semaphore P operation. */
  	sops.sem_num = 0; 											/* Operate on semaphore 0 within the semaphore set. */
  	sops.sem_op = -1; 											/* Subtract 1 to the semaphore value. */ 
  	sops.sem_flg = 0; 											/* Wait until the OS completes requested operation. */ 

	if(semop(semid, &sops, 1) < 0){
			  perror("semop");
			  exit(EXIT_FAILURE);
	}
	
	printf("Successful P operation performed on semaphore %d by %ld. \n", KEY, (long)getpid());
  
  }
  
  exit(EXIT_SUCCESS);
}

