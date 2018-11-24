/*
 * The client program that communicates with the server using shared memory.
 *
 * Author: Naga Kandasamy
 * Date modified: 01/14/2014
 *
 * Compile as follows: gcc -o client client_code.c -std=c99
*/

#define _SVID_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <stdlib.h>
#include "shared_memory_struct.h"

#define FALSE 0
#define TRUE 1
#define SHARED_MEMORY_KEY 3228 // This is the well-known key for the shared memory area
#define SEMAPHORE_KEY 32281 // This semaphore synchronizes access to shared memory

/* The probe or wait operation on the semaphore */
void P(int key){
	int id;
	struct sembuf operations[1]; // An array of one operation to perform on the semaphore

	/* Get the index of the semaphore using the provided key. */
  	if ((id = semget(key, 1, 0666)) < 0){
		printf("Program cannot find the semaphore. Exiting. \n");
		exit(0);
  	}

 	 /* Perform the semaphore P operation. */
  	operations[0].sem_num = 0; // We are operating on semaphore 0 in the array of semaphores
  	operations[0].sem_op = -1; // Subtract 1 to the semaphore value 
  	operations[0].sem_flg = 0; // We will wait until the OS completes the requested semaphore operation 

  	/* Perform the P operation. */
  	if(semop(id, operations, 1) == 0)
		printf("Client P operation performed on semaphore %d. \n", key);
  	else 
		printf("Client: P operation on semaphore %d failed. \n", key);

}

/* The signal operation on the semaphore. */
void V(int key){
	int id;
	struct sembuf operations[1]; // An array of one operation to perform on the semaphore

	/* Get the index of the semaphore using the provided key. */
  	if ((id = semget(key, 1, 0666)) < 0){
		printf("Program cannot find the semaphore. Exiting. \n");
		exit(0);
  	}

 	 /* Perform the semaphore V operation. */
  	operations[0].sem_num = 0; // We are operating on semaphore 0 in the array of semaphores
  	operations[0].sem_op = 1; // Add 1 to the semaphore value 
  	operations[0].sem_flg = 0; // We will wait until the OS completes the requested semaphore operation 

  	/* Perform the V operation. */
  	if(semop(id, operations, 1) == 0)
		printf("Client: V operation performed on semaphore %d. \n", key);
  	else 
		printf("Client: V operation on semaphore %d failed. \n", key);
}

/* Locate the shared memory segment previously created by the server and attach the segment to our data space */
SHARED_MEMORY_STRUCT *locate_shared_memory(key_t key){
	int shmid;
    	SHARED_MEMORY_STRUCT *ptr;

    	/* Locate the segment previously created by the server. */
    	if ((shmid = shmget(key, sizeof(SHARED_MEMORY_STRUCT), 0666)) < 0) {
        	perror("shmget");
        	exit(1);
    	}

    	/* Attach the segment to our data space. */
    	if ((ptr = shmat(shmid, NULL, 0)) == NULL) {
        	perror("shmat");
        	exit(1);
	}
	return ptr;
}

int main(int argc, char **argv){

	if(argc < 2){
		printf("Usage client <client id> <string> \n");
		exit(0);
	}

	int client_id = atoi(argv[1]); // Obtain the client ID
	
	/* Locate shared memory */
	SHARED_MEMORY_STRUCT *ptr = locate_shared_memory(SHARED_MEMORY_KEY);

    	/* Write to shared memory. */
    	ptr->client_id = client_id;
	strcpy(ptr->buffer, argv[2]);

	V(SEMAPHORE_KEY); // Signal the server

    	exit(0);
}

