/*
Author: Naga Kandasamy, 12/22/2008

This program illustrates the use of pipes as a synchronization mechanism between processes.

Compile the code as follows.

gcc main.c sync_with_pipes.c -o sync_with_pipes -std=c99

*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

extern int CREATE_SYNC_CHANNEL(void);
extern int TELL_PARENT(void);
extern int WAIT_FOR_PARENT(void);
extern int TELL_CHILD(void);
extern int WAIT_FOR_CHILD(void);

#define MAX_BUFFER_SIZE 256

int main(int argc, char **argv){

  int pid; /* Process ID */
  int status;
  FILE *fp; 
  char buffer[MAX_BUFFER_SIZE]; 

  CREATE_SYNC_CHANNEL(); // Create two pipes for full-duplex communication between the parent and child 

  if((pid = fork()) < 0){ /* Fork the process */
     printf("Fork error \n");
     exit(-1);
   }
  else if(pid > 0){ /* Parent code */
     printf("Process ID of the parent is %d. \n", getpid()); /* Print parent's process ID */
   }
  else if(pid == 0){ /* Child code */
    printf("Child: Waiting for parent to give OK signal. \n");
    
    WAIT_FOR_PARENT(); // Block here until the parent signals that it is OK to proceed
    printf("Child: Reading message from parent. \n");
    fp = fopen("message.txt", "rt"); // Open the file in read mode
    fscanf(fp, "%s", buffer); // Read the string
    printf("Message says: %s \n", buffer);
    fclose(fp);
    TELL_PARENT(); // Tell the parent that we are done
    
    exit(0); /* Child exits */
    }

  /* Parent code */
  sleep(5);
  fp = fopen("message.txt", "wt"); // Open file in write mode
  if(fp == NULL){
    printf("Parent: Error opening file. Exiting. \n");
    exit(-1);
  }
  fprintf(fp, "Hello_there. \n"); // Print string to the file
  fclose(fp); // Close the file pointer

  TELL_CHILD(); // Tell the child process that it can now read the file
  WAIT_FOR_CHILD(); // Wait for the child to finish reading the file
  printf("Parent: The chid is done reading the file. \n");

  exit(0);
}
