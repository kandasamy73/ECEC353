/* Shows how to implement synchronization between a parent and child using 
 * pipes. (Code derived from "Advanced UNIX Programming", Richard Stevens) 
 *
 * Author: Naga Kandasamy
 * Date created: January 14, 2009 
 * Date modified: July 4, 2018
 *
 * Compile as follows: gcc -o sync_with_pipes sync_with_pipes.c -std=c99 -Wall
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define BUF_SIZE 256


/* File descriptors for full-duplex communication between parent and child */
int pfd1[2], pfd2[2];   

int 
create_sync_channel (void)          /* Create the communication channel */
{                
    if (pipe (pfd1) < 0 || pipe (pfd2) < 0)
        return FALSE;
  
    return TRUE;
}

int 
tell_parent (void)                  /* Tell the parent that child is done */
{
    if (write (pfd2[1], "c", 1) != 1)
        return FALSE;
  
    return TRUE;
}

int 
wait_for_parent (void)              /* Wait for parent */
{
    char c;
    if (read (pfd1[0], &c, 1) != 1){
        printf ("WAIT_PARENT: Read error; incorrect number of bytes \n");
        return FALSE;
    }
  
    if (c != 'p'){
        printf("WAIT_PARENT: Incorrect data from pipe \n");
        return FALSE;
    }
  
    return TRUE;
}

int 
tell_child (void)                   /* Tell child that parent is done */
{
    if (write (pfd1[1], "p", 1) != 1)
        return FALSE;
  
    return TRUE;
}

int 
wait_for_child (void)               /* Wait for the child process */
{
    char c;
    if (read (pfd2[0], &c, 1) != 1){
        printf("WAIT_PARENT: Read error; incorrect number of bytes \n");
        return FALSE;
    }
  
    if(c != 'c'){
        printf("WAIT_PARENT: Incorrect data from pipe \n");
        return FALSE;
    }
  
    return TRUE;
}

int 
main(int argc, char **argv)
{
    int pid, status;
    FILE *fp; 
    char buffer[BUF_SIZE]; 

    /* Create two pipes for full-duplex communication between parent and child */ 
    if (create_sync_channel () == FALSE){
        printf ("PARENT: cannot create the commnication channel. \n");
        exit (EXIT_FAILURE);
    } 
   
    switch (pid = fork ()){
        case -1: 
            perror ("fork");
            exit(EXIT_FAILURE);
        
        case 0:                                                             /* Child code */
            printf ("CHILD: Waiting for parent to give OK signal. \n");
            
            wait_for_parent ();                                             /* Block until the parent signals it is OK to proceed */
            printf ("CHILD: Reading message from parent. \n");
            fp = fopen ("message.txt", "rt");                               /* Open the file in read mode */
            fscanf (fp, "%s", buffer);                                      /* Read the string */
            printf ("CHILD: Message says: %s \n", buffer);
            fclose (fp);
    
            tell_parent ();                                                 /* Tell the parent that child is done */

            sleep (5);                                                      /* Simulate some processing */
            exit(EXIT_SUCCESS);                                             /* Child exits */

        default:                                                            /* Parent code */
            sleep (5);                                                      /* Simulate some processing */
            fp = fopen ("message.txt", "wt");                               /* Open file in write mode */
            if(fp == NULL){
                perror ("open");
                exit (EXIT_FAILURE);
            }
            fprintf (fp, "Hello_there. \n");                                 /* Print string to the file */
            fclose (fp);                                                     /* Close the file pointer */

            tell_child ();                                                  /* Tell child that it can now read the file */
            wait_for_child ();                                              /* Block until child finishes reading the file */
  
            printf ("PARENT: Child is done reading the file. \n");
    }

    waitpid (pid, &status, 0);                                              /* Wait for child to exit */
    exit (EXIT_SUCCESS);
}





