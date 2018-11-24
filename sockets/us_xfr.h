/* Header file used by both the server and client programs 
 *
 * Author: Naga Kandasamy
 * Date created: July 15, 2018
 *
 * Source: M. Kerrisk, The Linux Programming Interface.
 *
 */

#ifndef _US_XFR_H_
#define _US_XFR_H_

#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* Well-known socket name for the server */
#define SERVER_SOCKET_PATH "/tmp/us_xfr"
/* Size of the buffer for the client stream */
#define BUF_SIZE 100

#endif /* _US_XFR_H_ */
