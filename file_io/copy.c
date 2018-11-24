/* Program copies the contents of an existing file to a new file. 
 * It illustrates the use of the open(), read(), and write() system calls. 
 *
 * Author: Naga Kandasamy
 * Date created: June 28, 2018
 * Date modified: 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#define BUF_SIZE 1024

int 
main (int argc, char **argv)
{
    int read_fd, write_fd, open_flags;
    mode_t file_perms;
    ssize_t num_bytes_read;
    char buf[BUF_SIZE];

    if (argc != 3 || !strcmp(argv[1], "--help"))
        printf ("%s old-file new-file \n", argv[0]);


    /* Open the input and output files */
    read_fd = open (argv[1], O_RDONLY);
    if (read_fd == -1){
        perror ("open");
        exit (EXIT_FAILURE);
    }

    /* Create the appropriate flags and permissions for the output file, 
     * and create it. 
     */
    open_flags = O_CREAT | O_WRONLY | O_TRUNC;
    file_perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | \
                 S_IROTH | S_IWOTH; /* rw-rw-rw */

    write_fd = open (argv[2], open_flags, file_perms);
    if (write_fd < 1){
        perror ("open");
        close (read_fd);
        exit (EXIT_FAILURE);
    }

    /* Copy data until we encounter EOF or an error */
    while ((num_bytes_read = read (read_fd, buf, BUF_SIZE)) > 0)
        if (write (write_fd, buf, num_bytes_read) != num_bytes_read){
            printf ("Something went wrong...couldn't write the whole buffer. \n");
            close (read_fd);
            close (write_fd);
            exit (-1);
        }


    if (num_bytes_read == -1){
        perror ("read");
        exit (EXIT_FAILURE);
    }

    if (close (read_fd) == -1){
        perror ("close");
        exit (EXIT_FAILURE);
    }

    if (close (write_fd) == -1){
        perror ("close");
        exit (EXIT_FAILURE);
    }

    exit (EXIT_SUCCESS);
}
