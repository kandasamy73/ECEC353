/* Example using a named pipe for unidirectional communication between
 * two unrelated processes.
 *
 *   This example consists of two files:
 *     -- 3-named_pipes_writer.c
 *     -- 3-named_pipes_reader.c  (this file)
 *
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * Compile using:
 *   gcc -o 3-named_pipes_reader 2-named_pipes_reader.c
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main()
{
    int fd;
    char* myfifo = "/tmp/fifo_example";
    char buf[BUFFER_SIZE];

    fd = open(myfifo, O_RDONLY);
    if (fd == -1) {
        fprintf (stderr, "error -- could not open FIFO\n");
        exit(EXIT_FAILURE);
    }
    read(fd, buf, BUFFER_SIZE);
    printf("Received: %s\n", buf);
    close(fd);

    return 0;
}
