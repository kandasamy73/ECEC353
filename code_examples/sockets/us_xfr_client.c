/* Client side of the client-server application that uses stream sockets in 
 * the UNIX domain. 
 *
 * The client connects to the server and uses the connection to transfer 
 * data from its stdin to the server. 
 *
 * Author: Naga Kandasamy
 * Date created: July 15, 2018
 *
 * Source: M. Kerrisk, The Linux Programming Interface
 *
 */
#include "us_xfr.h"

int 
main (int argc, char **argv)
{
    struct sockaddr_un addr;
    int sfd;

    /* Create the client socket */
    sfd = socket (AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1){
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    /* Construct the server-socket address and make the connection */
    memset (&addr, 0, sizeof (struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy (addr.sun_path, SERVER_SOCKET_PATH, sizeof (addr.sun_path) - 1);

    if (connect (sfd, (struct sockaddr *)&addr, sizeof (struct sockaddr_un)) == -1){
        perror ("connect");
        exit (EXIT_FAILURE);
    }

    /* Copy stdin to the socket. Upon encountering EOF, the child terminates. 
     * When the child terminates, its socket is closed and 
     * the server sees EOF when reading from the other side of the connection */

    ssize_t nr;
    char buf[BUF_SIZE];
    while ((nr =  read (STDIN_FILENO, buf, BUF_SIZE)) > 0){
        if (write (sfd, buf, nr) != nr){
            perror ("write");
            exit (EXIT_FAILURE);
        }
    }

    if (nr == -1){
        perror ("read");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}

