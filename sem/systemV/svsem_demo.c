/* 
 * Program creates a new semaphore set containing a single semaphore and intializes it to the 
 * value supplied in the command line.
 *
 * If two command-line arguments are provided, the program interprets them in the following
 * order: identifier of an existing semaphore set and the value to be added to the 
 * first semaphore (semaphore 0) in that set.
 *
 * Author: Naga Kandasamy
 * Date: 7/7/2015
 *
 */


#define _SVID_SOURCE
#define _XOPEN_SOURCE 


#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "semun.h"

void 
errExit(char *message)
{
		  perror(message);
		  exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
		  int semid;

		  if(argc < 2 || argc > 3 || strcmp(argv[1], "--help") == 0){
					 printf("%s init-value or %s semid operation\n", argv[0], argv[0]);
					 exit(EXIT_SUCCESS);
		  }

		  if(argc == 2){
					 union semun arg;
					 semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR); /* Obtain a semaphore set from the OS. */
					 if(semid == -1)
						 errExit("semid");
					 

					 arg.val = atoi(argv[1]);
					 /* Initialize semaphore 0 wihtin the set with the command-line value. */
					 if(semctl(semid, /* Semaphore number */ 0, SETVAL, arg) == -1)
								errExit("semctl");
					 printf("Semaphore obtianed. ID = %d\n", semid);
		  }
		  else{
					 struct sembuf sop; 
					 semid = atoi(argv[1]);
					 sop.sem_num = 0; /* First semaphore in the set. */
					 sop.sem_op = atoi(argv[2]); /* Add or subtract the semaphore value. */
					 sop.sem_flg = 0; /* No special options for this operation. */
					 printf("%ld about to semop.\n", (long)getpid());
					 if(semop(semid, &sop, 1) == -1)
								errExit("semop");
					 printf("%ld: semop completed.\n", (long)getpid());
		  }

		  exit(EXIT_SUCCESS);
}

