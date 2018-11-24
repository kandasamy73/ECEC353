/* 
 * This program creates a semaphore for use by programs sem_probe.c and sem_signal.c.
 * Author: Naga Kandasamy
 * Date created: 1/25/2009. Modified: 7/12/2015
*/

#define _SVID_SOURCE
#define _XOPEN_SOURCE 

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

#define KEY 2010									/* External identifier for the semaphore. */

int 
main(int argc, char **argv){
  int semid;										/* Semphore ID assigned by the kernel. */

  /* We now create a C union structure which we will use to interact with the semctl() function that 
	* can perform different operations on the semaphore depending on which arguments are passed 
	* to it. We will first use it to make sure that our semaphore is initialized to 0. */

  union semun{
    int val;
    struct semid_ds *buf;
    ushort *array;
  } argument;

  argument.val = 0; 

  /* Create the semaphore and assign it the value of KEY. Give permissions to the everyone 
	* in the system to be able to access the semaphore. We typically can get an array of semaphores 
	* and the second argument specifies it. We are requesting a single semaphore here. */
  if ((semid = semget(KEY, 1, 0666 | IPC_CREAT)) < 0) {
			 perror("semget");
			 exit(EXIT_FAILURE);
  }
  
    if(semctl(semid, 0, SETVAL, argument) < 0){		/* Initialize semaphore 0. */
				perror("semctl");
				exit(EXIT_FAILURE);
	 } 
	 
	 printf("Semaphore %d initialized. \n", KEY);
	 exit(EXIT_SUCCESS);
}

