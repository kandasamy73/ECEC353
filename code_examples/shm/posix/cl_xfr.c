/* Program illustrates memory mapping and persistency, with POSIX objects. 
 * The client reads and displays a message left to it in the memory segment by the server, 
 * a file been mapped from a memory segment.
 *
 * Author: Naga Kandasamy
 * Date modified: July 16, 2018
 *
 * Compile as follws: gcc -o cl_xfr cl_xfr.c -std=c99 -Wall -lrt
 *
 */

#define _BSD_SOURCE

#include "sv_cl_xfr.h"


int 
main (int argc, char **argv) 
{
    int fd;

    /* We want a shared segment capable of storing one message */
    int shared_seg_size = (sizeof (struct msg));       
    struct msg *shared_msg;      /* The shared segment */
    

    /* Open the shared memory object using shm_open() in read-write mode */
    fd = shm_open (SHARED_OBJECT_PATH, O_RDWR, S_IRWXU | S_IRWXG);
    if (fd < 0) {
        perror ("shm_open");
        exit (EXIT_FAILURE);
    }
    printf ("Opened shared memory object %s\n", SHARED_OBJECT_PATH);
    
    /* Request the shared segment using mmap() */    
    shared_msg = (struct msg *) mmap (NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_msg == NULL) {
        perror ("mmap");
        exit (EXIT_FAILURE);
    }
    printf ("Shared memory segment allocated correctly (%d bytes).\n", shared_seg_size);


    printf ("Message type is %d, content is: %s\n", shared_msg->type, shared_msg->buf);

	  /* [uncomment if you wish] requesting the removal of the shm object     --  shm_unlink() */
    if (shm_unlink (SHARED_OBJECT_PATH) != 0) {
        perror ("shm_unlink");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}

