/*  Program shows the use of memory mapping and persistency with POSIX objects. The server process generates a message 
 *  and leaves it in a shared segment. The segment is mapped in a persistent object meant to be subsequently open by a shared 
 *  memory client. The shared object is created and stored in the file system under /dev/shm
 *
 *  Author: Naga Kandasamy
 *  Date modified: July 16, 2018
 *  
 *  Compile as follows: gcc -o sv_xfr sv_xfr.c -std=c99 -Wall -lrt
 */

#define _BSD_SOURCE

#include "sv_cl_xfr.h"

/* Message structure for messages in the shared segment */
struct msg {
    int type;
    char buf[BUF_LEN];
};


int 
main (int argc, char **argv) 
{
    int fd;
    int shared_seg_size = (sizeof(struct msg));   /* We want a shared segment capable of storing one message */
    struct msg *shared_msg;      /* the shared segment, and head of the messages list */
    
    /* Create the shared memory object using shm_open(). On Linux, by default it is created inside of /dev/shm. */
    fd = shm_open (SHARED_OBJECT_PATH, O_CREAT | O_EXCL | O_RDWR, S_IRWXU | S_IRWXG);
    if (fd < 0) {
        perror ("shm_open()");
        exit (EXIT_FAILURE);
    }
    fprintf (stderr, "Created shared memory object %s\n", SHARED_OBJECT_PATH);
    
    /* Adjust mapped file size (make room for the whole segment to map) using ftruncate(). */
    ftruncate (fd, shared_seg_size);

    /* Request the shared segment using mmap(). */    
    shared_msg = (struct msg *) mmap (NULL, shared_seg_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_msg == NULL) {
        perror ("mmap()");
        exit (EXIT_FAILURE);
    }
    fprintf (stderr, "Shared memory segment allocated correctly (%d bytes).\n", shared_seg_size);

    /* Produce a message with random contents and store it in the shared memory segment. */
    srandom (time (NULL));
    long int random_number = random ();
    shared_msg->type = random_number % MESSAGE_TYPES;
    printf ("Writing message of type %d with number %ld in shared memory \n", shared_msg->type, random_number);
    snprintf (shared_msg->buf, BUF_LEN, "My message, type %d, num %ld", shared_msg->type, random_number);
   
    /* [uncomment if you wish] requesting the removal of the shm object     --  shm_unlink() */
	/*
    if (shm_unlink (SHARED_OBJECT_PATH) != 0) {
        perror ("shm_unlink()");
        exit (EXIT_FAILURE);
    }
	*/

    exit (EXIT_SUCCESS);
}


