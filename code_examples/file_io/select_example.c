/* This example program demonstrates the use of the select system call. This simple example blocks waiting for user input on stdin for 
 * five seconds. We only watch a single file descriptor; so we are not actually multplexing I/O here.
 * Source: Robert Love, Linux System Programming, O'Reilly Press.
 *
 * Date created: May 27, 2018
 * Date modified: 
 *
 * Author: Naga Kandasamy 
 *
 */

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TIMEOUT 5   /* Time out parameter in seconds */
#define BUF_LEN 256 /* Length of the read buffer in bytes */

int main(void)
{
    struct timeval tv;  /* Structure to hold the specified TIMEOUT value. */
    fd_set readfds;     /* The set of specified file descriptors. */
    int status;         /* Variable to hold the value returned by select */
    int num_fds = 0;

    /* Before calling select, we must first remove all file descriptors from the set. 
     * Once that's done, we add the file descriptors of interest to the set. */
    FD_ZERO (&readfds);
    FD_SET (STDIN_FILENO, &readfds);
    num_fds = STDIN_FILENO + 1;   

    /* Set up the timeval structure to hold five seconds. */
    tv.tv_sec = TIMEOUT;
    tv.tv_usec = 0;

    /* Invoke the select system call to block on the file descriptors supplied in readfds. */
    status = select (num_fds, &readfds, NULL, NULL, &tv);

    /* Select has unblocked. Check for status. */
    if (status == -1) {
        perror ("select");
        return 1;
    } else if (!status) {
        printf ("select timed out after %d seconds \n", TIMEOUT);
        return 0;
    }

    /* Check if our file descriptor is ready to read. 
     * In this examlpe, it should be since it is the only 
     * file descriptor in the set provided to select.
     */

    if (FD_ISSET (STDIN_FILENO, &readfds)) {
        char buf[BUF_LEN + 1];
        int len;

        /* The call to read is guaranteed not to block since we have data associated with the 
         * file descriptor. 
         */
        len = read (STDIN_FILENO, buf, BUF_LEN); 
        if (len == -1) {
            perror ("read");
            return 1;
        }

        if (len) {
            buf[len] = '\0';    /* Null terminate our string. */
            printf ("read: %s \n", buf);
        }

        return 0;
    }

    fprintf (stderr, "Something went wrong with FD_ISSET. We should not be here. \n");
    return 1;
}

