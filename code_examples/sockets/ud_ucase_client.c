/* Client side of the client-server application that uses datagrams in 
 * the UNIX domain. 
 *
 * Client loops sending each of its command-line arguments as a 
 * separate message to the server. After sending each message, client reads 
 * the response from the server and displays it to stdout.
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
    struct sockaddr_un svaddr, claddr;
    int sfd;

    /* Parse command-line arguments */
    if (argc < 2 || strcmp (argv[1], "--help") == 0){
        printf ("Usage: %s msg...\n", argv[0]);
        exit (EXIT_FAILURE);
    }

    /* Create the client socket and bind to unique pathname based on process ID*/
    sfd = socket (AF_UNIX, SOCK_DGRAM, 0);
    if (sfd == -1){
        perror ("socket");
        exit (EXIT_FAILURE);
    }

    /* Create client socket based on its PID */
    memset (&claddr, 0, sizeof (struct sockaddr_un));
    claddr.sun_family = AF_UNIX;
    snprintf (claddr.sun_path, sizeof (claddr.sun_path), \
              "/tmp/ud_ucase_client.%ld", (long) getpid());

    if (bind (sfd, (struct sockaddr *)&claddr, sizeof (struct sockaddr_un)) == -1){
        perror ("bind");
        exit (EXIT_FAILURE);
    }
    
    /* Construct the address of the server */
    memset (&svaddr, 0, sizeof (struct sockaddr_un));
    svaddr.sun_family = AF_UNIX;
    strncpy (svaddr.sun_path, SERVER_SOCKET_PATH, sizeof (svaddr.sun_path) - 1);

    /* Send message to the server. Echo the responses to stdout */
   
    int msg_len, j;
    ssize_t num_bytes;
    char resp[BUF_SIZE];

    for (j = 1; j < argc; j++){
        msg_len = strlen (argv[j]);
        if (sendto (sfd, argv[j], msg_len, 0, (struct sockaddr *)&svaddr, \
                    sizeof (struct sockaddr_un)) != msg_len){
            perror ("sendto");
            exit (EXIT_FAILURE);
        }

        num_bytes = recvfrom (sfd, resp, BUF_SIZE, 0, NULL, NULL);
        if (num_bytes == -1){
            perror ("recvfrom");
            exit (EXIT_FAILURE);
        }

        printf ("Response %d: %.*s\n", j, (int) num_bytes, resp);
    }

    /* Remove client socket pathname */
    remove (claddr.sun_path);

    exit (EXIT_SUCCESS);
}

