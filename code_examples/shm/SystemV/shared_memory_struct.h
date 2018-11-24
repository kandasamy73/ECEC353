#ifndef _SHARED_MEMORY_STRUCT_H_
#define _SHARED_MEMORY_STRUCT_H_

/* Definition of the structure stored in shared memory. */

#define MAX_LENGTH 1000

typedef struct shared_memory_struct{
int client_id;
char buffer[MAX_LENGTH];

} SHARED_MEMORY_STRUCT;

#endif
