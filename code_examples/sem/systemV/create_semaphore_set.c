/* 
 * Program creates a new semaphore set containing a user specified number of semaphores 
 * that is supplied via the command line.
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
		  int semid;

		  if(argc < 2 || strcmp(argv[1], "--help") == 0){
					 printf("%s num semaphores\n", argv[0]);
					 exit(0);
		  }

		  if(argc == 2){
					 int num_semaphores = atoi(argv[1]);
					 semid = semget(IPC_PRIVATE, num_semaphores, S_IRUSR | S_IWUSR); /* Obtain semaphore set. */
					 if(semid == -1)
						 errExit("semid");
					 

		  }
		  exit(EXIT_SUCCESS);
}

