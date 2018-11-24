/* Shows a race condition between concurrently executing parent 
   and child processes. Both parent and child try to write a string to 
   screen one character at a time. However, if not properly synchronized, 
   the final output is randomly interleaved. (Code derived from "Advanced
   UNIX Programming", Richard Stevens) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static void charatatime(char *);

int main(int argc, char **argv){
  
  int pid;

  if((pid = fork()) < 0){
    printf("Fork error \n");
    exit(0);
  } 
  else if(pid == 0)
    charatatime("Output from Child \n");
  else
    charatatime("Output from Parent \n");

  exit(0);
}

static void charatatime(char *str){
  char *ptr;
  int c;

  setbuf(stdout, NULL);
  for(ptr = str; c = *ptr++;)
    putc(c, stdout);
}
