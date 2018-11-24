/* 
 * Program initializes all of the semaphores in an existing set. The first command-line argument is 
 * the identifier of the semaphore set to be initialized. The remaining command-line arguments specify 
 * the values to which the semaphores are to be initialized.
 * 
 * Author: Naga Kandasamy
 * Date: 7/8/2015
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
		  struct semid_ds ds; 
		  union semun arg;
		  int i, semid;

		  if(argc < 3 || strcmp(argv[1], "--help") == 0){
					 printf("Usage: %s semid value1, value2, ...\n",argv[0]);
					 exit(EXIT_SUCCESS);
		  }
		  
		  semid = atoi(argv[1]);

		  /* Obtain the size of the sempahore set to be initialized. */
		  arg.buf = &ds;
		  if(semctl(semid, 0, IPC_STAT, arg) == -1)
					 errExit("semctl");

		  if(ds.sem_nsems != (argc - 2)){
					 printf("Semaphore set contains %d semaphores but only %d initialization \
										  values were supplied. \n", ds.sem_nsems, argc - 2);
					 exit(EXIT_SUCCESS);
		  }
		  
		  /* Set up array of values and perform the semaphore initialization. */
		  arg.array = calloc(ds.sem_nsems, sizeof(arg.array[0]));
		  if(arg.array == NULL)
					 errExit("calloc");

		  for(i = 2; i < argc; i++)
					 arg.array[i - 2] = atoi(argv[i]);


		  if(semctl(semid, 0, SETALL, arg) == -1)
					 errExit("semctl-SETALL");


		  printf("%ld: semaphore values changed successfully.\n", (long)getpid());
		  exit(EXIT_SUCCESS);
}


