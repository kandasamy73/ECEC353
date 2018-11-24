/* Server side of the client-server application that uses datagrams 
 * in the UNIX domain. 
 *
 * The server program enters an infinite loop. It receives datagrams from 
 * the client, converts the received text to upper-case and returns 
 * it back to the client. 
 * 
 * Author: Naga Kandasamy
 * Date created: July 15, 2018
 *
 * Source: M. Kerrisk, The Linux Programming Interface
 *
 */
#include "ud_ucase.h"

int
main (int argc, char **argv)
{
    struct sockaddr_un svaddr;
    int sfd;

    /* Create server socket to receive datagrams */
    sfd = socket (AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1){
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    /* Construct the well-known address and bind server socket to it */
    if (remove (SERVER_SOCKET_PATH) == -1 && errno != ENOENT){
        perror ("remove");
        exit (EXIT_FAILURE);
    }

    memset (&svaddr, 0, sizeof (struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy (svaddr.sun_path, SERVER_SOCKET_PATH, sizeof (svaddr.sun_path) -1);

    if (bind (sfd, (struct sockaddr *)&svaddr, sizeof (struct sockaddr_un)) == -1){
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    /* Execute an infinite loop to perform the following steps:
     *   - Receive messages from a client
     *   - Convert the contents to uppercase
     *   - Return the contents to client
     *  */

    struct sockaddr_un claddr;
    socklen_t len;
    int num_bytes;
    char buf[BUF_SIZE];

    while (1){
        len = sizeof (struct sockaddr_un);
        num_bytes = recvfrom (sfd, buf, BUF_SIZE, 0,\
                              (struct sockaddr *)&claddr, &len);
        if (num_bytes == -1){
            perror ("recvfrom");
            exit (EXIT_FAILURE);
        }

       
        printf ("Server received %ld bytes from %s\n", (long) num_bytes, \
                                                       claddr.sun_path);
        
        for (int j = 0; j < num_bytes; j++)
            buf[j] = toupper ((unsigned char) buf[j]);

      
        if (sendto (sfd, buf, num_bytes, 0, (struct sockaddr *)&claddr, len) != \
                num_bytes){
            perror ("sendto");
            exit (EXIT_FAILURE);
        }

    }
    exit (EXIT_SUCCESS);
}


