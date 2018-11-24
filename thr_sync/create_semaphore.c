/* 
This program creates a semaphore for use by programs sem_a.c and sem_b.c.
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

  /* We know create an argument which we will use to interact with the semctl() function. semctl() can perform different operations 
     on the semaphore depending on which arguments are passed to it. We will first use it to make sure that our semaphore is 
     initialized to 0. */

  union semun{

    int val;
    struct semid_ds *buf;
    ushort *array;
  } argument;

  argument.val = 0; 

  /* Create the semaphore and assign it the value of KEY. Give permissions to the everyone in the system to be able to access 
     the semaphore. We typically can get an array of semaphores and the second argument specifies it. We are requesting a single value. */
  if ((id = semget(KEY, 1, 0666 | IPC_CREAT)) < 0) {
	printf("Unable to obtain semaphore from the operating system. \n");
     	exit(0);
  }
  
  /* Set the value of the zeroth element in the array of semaphores to 0. */
  if(semctl(id, 0, SETVAL, argument) < 0){
	printf("Cannot set semaphore value. \n");
  } else {
	printf("Semaphore %d initialized. \n", KEY);
  }
  exit(0);
}

