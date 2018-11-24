/* Example using a named pipe for unidirectional communication between
 * two unrelated processes.
 *
 *   This example consists of two files:
 *     -- 3-named_pipes_writer.c  (this file)
 *     -- 3-named_pipes_reader.c
 *
 *   Author: James A. Shackleford
 *     Date: January 8th, 2015
 *
 * Compile using:
 *   gcc -o 3-named_pipes_writer 3-named_pipes_writer.c
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char* myfifo = "/tmp/fifo_example";
    char msg[] = "Hello World!";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0666);

    /* Note: this process will block (i.e. wait)
     * until the other end of the pipe is opened. */
    fd = open(myfifo, O_WRONLY);

    write(fd, msg, sizeof(msg));
    close(fd);

    unlink(myfifo);

    return 0;
}
