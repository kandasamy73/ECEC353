/*
Author: Naga Kandasamy, 1/14/2009

The function create_shared_memory creates a shared memory area in the address space of the parent 
process that the child can access. In effect, the child and parent share that area of memory, and can communicate with each other via the 
shared-memory area.

Compile as follows: 

	gcc -o shared_memory shared_memory.c -std=c99

*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_STRING_LENGTH 256
#define SIZE_OF_FIRST_SHARED_MEMORY 10*sizeof(int)   // Size of the shared memory area; it can store 10 integers
#define SIZE_OF_SECOND_SHARED_MEMORY 256*sizeof(char) // Size of shared memory area; it can store 256 characters


/* This function creates a shared memory area in the parent's address space of size size_of_shared_memory, and returns a void* pointer to it */ 
void *create_shared_memory(int size_of_shared_memory){
  int fd;
  void *area;

  fd = open("/dev/zero", O_RDWR); 
  area = mmap(0, size_of_shared_memory, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  close(fd);
  return area;

}

int main(int argc, char **argv){
  
  int *first_area = (int *)create_shared_memory(SIZE_OF_FIRST_SHARED_MEMORY); // Create the shared area and type cast the pointer to point to integers
  char *second_area = (char *)create_shared_memory(SIZE_OF_SECOND_SHARED_MEMORY); // Create the shared area and type cast the pointer to point to characters
  
  int out[10];
  char buffer[MAX_STRING_LENGTH];
  int i;
  int status;

  for(i = 0; i < 10; i++) first_area[i] = 10; // Write some integers to the shared memory area
  
  strcpy(second_area, "Hello World"); // Write some string to the second shared memory area
  strcpy(buffer, second_area); // Copy the contents of the shared-memory area to a buffer
  // printf("%s \n", buffer);

  int pid = fork(); 
  if(pid < 0){
    printf("Fork error. \n");
    exit(-1);
  }
  if(pid == 0){ // Child code
    printf("Parent says: %s \n", second_area); 
    /* Increment the integers that were previously written to shared memory */
    for(i = 0; i < 10; i++) first_area[i]++;
    exit(0);
  }

  /* Parent code */
  wait(&status); // Wait for child

  /* Read the integers from shared memory */
  for(i = 0; i < 10; i++){
    out[i] = first_area[i];
    printf("%d \n", out[i]);
  }
  exit(0);
}
