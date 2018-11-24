/* Header file used by both the client and the server */

#ifndef _SV_CL_XFR_H_
#define _SV_CL_XFR_H_

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#define SHARED_OBJECT_PATH         "/sv_shm"
#define BUF_LEN                    50
#define MESSAGE_TYPES               8

/* Message structure for messages in the shared segment */
struct msg {
    int type;
    char buf[BUF_LEN];
};

#endif

