/*
Author: Naga Kandasamy, 12/28/2008

This program illustrates the use of the mkfifo system call to create a named pipe. Named pipes 
allow two unrelated processes to communicate with each other. These pipes are also known as 
FIFOs (first-in first-out) and can be used to establish a one-way or half-duplex flow of data. 

In the following example, a client and server use named pipes for one-way communication. The server 
first creates a FIFO, and opens it for reading. It then waits for input from the client on the read end of 
the pipe. Named-pipe reads are blocking, and so the server waits for the client to send a string to it. It 
then converts the string from the client into all upper-case and displays the string to the user. 

Compile the server code as follows.

gcc server_code.c -o server -std=c99

The client code can be found in client_code.c 

*/ 

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h> // Needed for the mkfifo system call
#include <fcntl.h>
#include <stdlib.h>

#define HALF_DUPLEX_FILE "/tmp/half_duplex_2"
#define MAX_BUFFER_SIZE 256

static void control_c_handler(int); /* Signal handler to trap Control+C */ 

int main(int argc, char *argv[])
{
  int n, fd, count;
  char buffer[MAX_BUFFER_SIZE];

  /* Set up signal handler to catch the Control+C signal. */
  if(signal(SIGINT, control_c_handler) == SIG_ERR){ 
    printf("Cannot catch SIGINT. \n");
  }

  if((mkfifo(HALF_DUPLEX_FILE, 0666) < 0)){ // Create the named pipe with read and write permissions 
    printf("Error creating FIFO. Exiting. \n");
    exit(-1);
  }

  fd = open(HALF_DUPLEX_FILE, O_RDONLY); // Open the reading end of the FIFO

  n = read(fd, buffer, MAX_BUFFER_SIZE); // Read n bytes from the reading end; block if no data is available
  buffer[n] = '\0'; // Terminate the string
  printf("Server: Read %d bytes from the FIFO: %s. \n", n, buffer);
  count = 0;
  while(count < n){
			 buffer[count] = toupper(buffer[count]); // Convert each character to upper case
			 count++;
  } 
  printf("Server: The converted string is %s. \n", buffer);

  unlink(HALF_DUPLEX_FILE); // Remove the pipe
  exit(0);
}


/* Signal handler to capture the quit (Control+C) signal */
static void control_c_handler(int signalNumber){
  printf("Caught the quit (Control+C) signal. \n");
  printf("Deleting the FIFO. \n");
  unlink(HALF_DUPLEX_FILE); // Remove the pipe
  exit(0);
}
