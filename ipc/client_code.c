/*
Author: Naga Kandasamy, 12/28/2008

This program illustrates the use of the mkfifo system call to create a named pipe. Named pipes 
allow two unrelated processes to communicate with each other. These pipes are also known as 
FIFOs (first-in first-out) and can be used to establish a one-way or half-duplex flow of data. 

In the following example, a client and server use named pipes for one-way communication. The server 
first creates a FIFO, and opens it for reading. It then waits for input from the client on the read end of 
the pipe. Named-pipe reads are blocking, and so the server waits for the client to send a string to it. It 
then converts the string from the client into all upper-case and displays the string to the user. 

The client opens the named pipe that was previously created by the server in write mode and writes a 
user-specified string to the pipe.

Compile the client code as follows.

gcc client_code.c -o client -std=c99

*/ 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h> // Needed for the mkfifo system call
#include <fcntl.h>

#define HALF_DUPLEX_FILE "/tmp/half_duplex_2"
#define MAX_BUFFER_SIZE 256

int main(int argc, char *argv[])
{
  int fd;

  if(argc < 2){
    printf("Usage: client <string> \n");
    exit(0);
  }

  fd = open(HALF_DUPLEX_FILE, O_WRONLY); // Open the writing end of the FIFO

  printf("Client: Writing %d bytes to the FIFO. \n", strlen(argv[1]));
  write(fd, argv[1], strlen(argv[1])); // Write the string to the writing end; non blocking

  exit(0);
}
