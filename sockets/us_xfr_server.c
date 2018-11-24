/* Server side of the client-server application that uses stream 
 * sockets in the UNIX domain. 
 *
 * The server program accepts client connections and transfers 
 * all data sent on the connection by the client to standard output. 
 *
 * The server is an example of an iterative server; that is, it one 
 * client at a time before moving on to the next one.
 *
 * Author: Naga Kandasamy
 * Date created: July 15, 2018
 *
 * Source: M. Kerrisk, The Linux Programming Interface
 *
 */
#include "us_xfr.h"

/* Define the maximum backlog allowed in terms of client connections */
#define BACKLOG 5

int
main (int argc, char **argv)
{
    struct sockaddr_un addr;     /* Structure for stream socket */
    int sfd;

    /* Open a stream socket in the UNIX domain */
    sfd = socket (AF_UNIX, SOCK_STREAM, 0);
    if (sfd == -1){
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    /* Construct server socket address and bind it to the previously 
     * created socket. make it a listening socket. First, remove 
     * any existing file with the same pathname at that to which we 
     * want to bind the socket */
    if (remove (SERVER_SOCKET_PATH) == -1 && errno != ENOENT){
        perror ("remove");
        exit (EXIT_FAILURE);
    }

    memset (&addr, 0, sizeof (struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy (addr.sun_path, SERVER_SOCKET_PATH, sizeof (addr.sun_path) -1);

    if (bind (sfd, (struct sockaddr *)&addr, sizeof (struct sockaddr_un)) == -1){
        perror ("bind");
        exit (EXIT_FAILURE);
    }

    if (listen (sfd, BACKLOG) == -1){
        perror ("listen");
        exit (EXIT_FAILURE);
    }


    /* Execute an infinite loop to handle incoming client requests. The loop performs
     * the following functions:
     *  - Accept a connection, obtaining a new socket, cfd, for the connection 
     *  - Real all the data from the connected socket and write it to stdout
     *  - Close the connected socket, cfd
     *  */

    int cfd;
    ssize_t nr;
    char buf[BUF_SIZE];
    while (1){
        /* Accept a connection which is returned on a new socket cfd. 
         * Note that the listening socket, sfd, remains open, and can 
         * be used to accept further incoming connections.
         *
         * Note: the server must be terminated manually by sending it a
         * signal.
         */

        cfd = accept (sfd, NULL, NULL);
        if (cfd == -1){
            perror ("accept");
            exit (EXIT_FAILURE);
        }

        /* Transfer data from cfd to stdout until EOF is received */
        while ((nr = read (cfd, buf, BUF_SIZE)) > 0){
            if (write (STDOUT_FILENO, buf, nr) != nr){
                perror ("write");
                exit (EXIT_FAILURE);
            }
        }

        if (nr == -1){
            perror ("read");
            exit (EXIT_FAILURE);
        }

        if (close (cfd) == -1){
            perror ("close");
        }
    }

    exit (EXIT_SUCCESS);
}


