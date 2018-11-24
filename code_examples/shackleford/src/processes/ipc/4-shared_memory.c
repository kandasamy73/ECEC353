/* Example using shared memory to communicate between parent and child
 * processes.
 *
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * Compile using:
 *   gcc -o 3-shared_memory 3-shared_memory.c -lrt
 *
 * Note:
 *   As shown above, the usage of shm_open() and friends requires that we link
 *   against the POSIX Realtime Extensions library
 */

#include <sys/mman.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    int i;
    int child_ret;
    pid_t pid;

    const char* smem_name = "/SHARE";  /* name of the shared memory area */
    int smem_fd;                       /* shared memory file descriptor  */
    size_t smem_size = 1024;           /* size in bytes */
    int* smem_ptr;

    /* Create the shared memory area and get a file descriptor to it */
    smem_fd = shm_open(smem_name, O_CREAT | O_RDWR, 0666);
    
    /* set the size as you normally would with a file via ftruncate */
    ftruncate(smem_fd, smem_size);

    /* map the file (i.e. shared memory) into memory */
    smem_ptr = mmap(0, smem_size, PROT_WRITE | PROT_READ, MAP_SHARED, smem_fd, 0);

    /* now that the shared memory area is mapped into our process's address space
     * we no longer need the file descriptor */
    close(smem_fd);

    /* write some data into shared memory */
    for (i=0; i<10; i++) {
        smem_ptr[i] = i;
    }

    /* Fork a child process */
    pid = fork();

    if (pid < 0) {
        fprintf(stderr, "error -- fork() failed\n");
        exit(1);
    }

    if (pid == 0) {
        /* only the CHILD executes this */

        for (i=0; i<10; i++) {
            /* read values from shared memory and display */
            printf("child -- smem_ptr[%i]: %i\n", i, smem_ptr[i]);

            /* also increment each value by 5 */
            smem_ptr[i] += 5;
        }

        exit(0);
    }

    wait(&child_ret);


    /* have the parent read and display the contents of shared memory */ 
    printf("---------------------\n");
    for (i=0; i<10; i++) {
        /* read values from shared memory and display */
        printf("parent -- smem_ptr[%i]: %i\n", i, smem_ptr[i]);
    }


    /* tear down shared memory region */
    munmap(smem_ptr, smem_size);
    shm_unlink(smem_name);
}

