#ifndef SEMUN_H
#define SEMUN_H	/* Prevent accidental double inclusion. */

#include <sys/types.h>
#include <sys/sem.h>

/* Used in calls to semctl() */
union semun {
		  int val;
		  struct semid_ds *buf;
		  unsigned short *array;
};

#endif 
