/* 
This program performs the V operation on a semaphore that has been created for us a priori using create_semaphore.c
Author: Naga Kandasamy, 1/25/2009.
*/
#define _SVID_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdlib.h>

/* The semaphore key is an arbitrary long integer serving as the external identifier by which the semaphore 
is known to any program that wishes to use it */
#define KEY 2010

int main(int argc, char **argv){
  int id; // Number by which the semaphore is known inside this program
  struct sembuf operations[1]; // An array of one operation to perform on the semaphore

  /* Get the index of the semaphore using the external name KEY. */
  if ((id = semget(KEY, 1, 0666)) < 0){
	printf("Program cannot find the semaphore. Exiting. \n");
	exit(0);
  }

  /* Perform the semaphore V operation. */
  operations[0].sem_num = 0; // We are operating on semaphore 0 in the array of semaphores
  operations[0].sem_op = 1; // Add 1 to the semaphore value 
  operations[0].sem_flg = 0; // We will wait until the OS completes the requested semaphore operation 

  /* Do the operation. */
  if(semop(id, operations, 1) == 0){
	printf("Successful V operation performed on semaphore %d by sem_a. \n", KEY);
  } else {
	printf("V operation on semaphore %d by sem_a did not succeed. \n", KEY);
  }
  exit(0);
}

