/* Program creates a shared memory area in the address space of the parent process that the child can access. In effect, the child and 
 * parent share that area of memory, and can communicate with each other via the shared-memory area.
 *
 * Author: Naga Kandasamy
 * Date created: Jamuary 14, 2009
 * Date modified: July 16, 2018 
 *
 * Compile as follows: 
 * gcc -o shared_memory shared_memory.c -std=c99 -Wall
*/ 

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define BUF_LEN 256
#define NUM_ITEMS 10
#define SIZE_OF_FIRST_SHARED_MEMORY NUM_ITEMS * sizeof(int)   /* Size of the shared memory area; it can store 10 integers */
#define SIZE_OF_SECOND_SHARED_MEMORY 256 * sizeof(char) /* Size of shared memory area; it can store 256 characters */


/* Create a shared memory area in the parent's address space of size size_of_shared_memory, and return a void* pointer to it */ 
void *
create_shared_memory (int size_of_shared_memory)
{
    int fd;
    void *area;
 
    /* Note /dev/zero is a special file that provides as many null characters (0x00) as are read from it. 
     * We use it here to initialize the allocated shared memory area */
    fd = open("/dev/zero", O_RDWR); 
 
    /* mmap() creates a new mapping in the virtual address space of the calling process.  The starting address for the new mapping is 
     * specified in the first argument. Since addr is ITENULL, the kernel chooses the address at which to create the mapping. If addr is not NULL, 
     * then the kernel takes it as a hint about where to place the mapping; on Linux, the mapping will be created at a nearby page boundary.  
     * The address of the new mapping is returned as the result of the call. The second argument specifies the length of the mapping.  
     * The contents of the shared memory are initialized using bytes referred to by the file descriptor fd.
       */
    area = mmap(NULL, size_of_shared_memory, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
    close(fd);
    return area;
}


int 
main (int argc, char **argv)
{
    /* Create the shared area and type cast the pointer to point to integers */
    int *first_area = (int *)create_shared_memory(SIZE_OF_FIRST_SHARED_MEMORY);     
    
    /* Create the shared area and type cast the pointer to point to characters */
    char *second_area = (char *)create_shared_memory(SIZE_OF_SECOND_SHARED_MEMORY);     
    
    int out[NUM_ITEMS];
    char buffer[BUF_LEN];
    int i;

    /* Write some integers to the shared memory area */
    for (i = 0; i < NUM_ITEMS; i++) 
        first_area[i] = 10;  

    /* Write some string to the second shared memory area */
    strcpy(second_area, "Hello World");   
    
    /* Copy the contents of the shared-memory area to a buffer */
    strcpy(buffer, second_area);   
 
    switch (fork ()){
        case -1: 
            perror ("fork");
            exit (EXIT_FAILURE);

        case 0: 
            printf ("Parent says: %s \n", second_area); 
    
            /* Increment the integers that were previously written to shared memory */
            for (i = 0; i < NUM_ITEMS; i++) 
                first_area[i]++;
    
            exit (EXIT_SUCCESS);

        default:
            break;
    }

    /* Parent code */
    wait (NULL); /* Wait for child */
  
    /* Read the integers from shared memory */
    for (i = 0; i < NUM_ITEMS; i++){
        out[i] = first_area[i];
        printf ("%d \n", out[i]);
    }
  
    exit (EXIT_SUCCESS);
}
