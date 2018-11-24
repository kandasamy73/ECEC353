/* Header file used by both the server and client programs for 
 * UNIX domain datagrams.
 *
 * Author: Naga Kandasamy
 * Date created: July 15, 2018
 *
 * Source: M. Kerrisk, The Linux Programming Interface.
 *
 */

#ifndef _UD_UCASE_H_
#define _UD_UCASE_H_

#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

/* Well-known socket name for the server */
#define SERVER_SOCKET_PATH "/tmp/ud_ucase"
/* Maximum size of messages exchanged between client and server */
#define BUF_SIZE 10

#endif /* _UD_UCASE_H_ */
