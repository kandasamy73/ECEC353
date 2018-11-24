/* 
   Author: Naga Kandasamy, 1/14/2009

   Shows how to implement synchronization between a parent and child using 
   pipes. (Code derived from "Advanced UNIX Programming", Richard Stevens) 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0

static int pfd1[2], pfd2[2];

/* Create the two pipes */
int CREATE_SYNC_CHANNEL(void){
  if(pipe(pfd1) < 0 || pipe(pfd2) < 0)
    return FALSE;
  return TRUE;
}

/* Tell the parent process that child is done */
int TELL_PARENT(void){
  if(write(pfd2[1], "c", 1) != 1)
    return FALSE;
  return TRUE;
}

/* Wait for the parent process */
int WAIT_FOR_PARENT(void){
  char c;
  if(read(pfd1[0], &c, 1) != 1){
    printf("WAIT_PARENT: Read error; incorrect number of bytes \n");
    return FALSE;
  }
  if(c != 'p'){
    printf("WAIT_PARENT: Incorrect data from pipe \n");
    return FALSE;
  }
  return TRUE;
}

/* Tell the child that the parent is done */
int TELL_CHILD(void){
 if(write(pfd1[1], "p", 1) != 1)
    return FALSE;
  return TRUE;
}

/* Wait for the child process */
int WAIT_FOR_CHILD(void){
 char c;
  if(read(pfd2[0], &c, 1) != 1){
    printf("WAIT_PARENT: Read error; incorrect number of bytes \n");
    return FALSE;
  }
  if(c != 'c'){
    printf("WAIT_PARENT: Incorrect data from pipe \n");
    return FALSE;
  }
  return TRUE;
}






