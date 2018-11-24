/* Revises the code in race.c, removing the race condition using the 
   synchronization mechanisms in sync_with_pipes.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

extern int TELL_WAIT();
extern int TELL_CHILD(int);
extern int TELL_PARENT(int);
extern int WAIT_CHILD();
extern int WAIT_PARENT();

static void charatatime(char *);

int main(int argc, char **argv){
  
  int pid;

  TELL_WAIT();
  if((pid = fork()) < 0){
    printf("Fork error \n");
    exit(0);
  } 
  else if(pid == 0){ /* Child process */
    WAIT_PARENT(); /* Wait for parent process */
    charatatime("Output from Child \n");
    // TELL_PARENT(pid); /* Tell parent process we are done */
  }
  else{ /* Parent process */
    // WAIT_CHILD(); /* Wait for child process */
    charatatime("Output from Parent \n");
    TELL_CHILD(pid); /* Tell child process we are done */
  }
  exit(0);
}

static void charatatime(char *str){
  char *ptr;
  int c;

  setbuf(stdout, NULL);
  for(ptr = str; c = *ptr++;)
    putc(c, stdout);
}
