/* A simple example demonstrating how to set the cancelability of a thread
 * using the POSIX threading library pthreads.
 *
 *   Author: James A. Shackleford
 *     Date: January 15th, 2015
 *
 * It is not uncommon to create a group of threads in order to perform a task
 * more quickly.  For example, you might create a number of threads to find a
 * file within the file system more quickly, where each thread searches a
 * different portion of the directory structure.  In such an example, it would
 * be a waste of system resources for the entire group to continue the search
 * once one of the threads has found the file.  To handle this need, the
 * pthread library offers a mechanism to terminate threads on demand.  This is
 * known as cancelability.
 *
 * The cancelability of a thread is defined by two parameters:
 *
 *   -- the cancelation state of the thread
 *        ** disabled
 *        ** enabled       (default)
 *   -- the cancellation type of the thread
 *        ** deferred      (default)
 *        ** asynchronous
 *
 * Both of these parameters are set in the thread's attribute block and can be
 * set using the pthreads API.
 *
 * Cancellation requests are sent using the pthread_cancel() API method
 *
 * Cancellation state is set by the thread itself using pthread_setcancelstate()
 *
 * Cancellation type is set by the thread itself using pthread_setcanceltype()
 *
 * Threads with the cancellation state of DISABLED ignore cancellation requests
 * sent to it by other threads.  Note, however, that these requests are
 * queued. So, if the thread later sets its cancellation state to ENABLED, it
 * will process the queued cancellation request at that time.
 *
 * Threads with DEFERRED cancellation will terminate once the thread executes
 * a special type of function, which is classified by the pthreads library as
 * a "cancellation point."  Cancellation points can be explicitly defined
 * using the pthread API method:
 *
 * pthread_testcancel()
 *
 * A comprehensive list of functions that act as cancellation points can be
 * found on the pthread_cancel man page:
 *
 * close(), creat(), fcntl(), fsync(), lockf(), nanosleep(), open(), pause(),
 * pthread_cond_timedwait(), pthread_cond_wait(), pthread_join(),
 * pthread_testcancel(), read(), readv(), select(), sigpause(), sigsuspend(),
 * sigwait(), sleep(), system(), tcdrain(), usleep(), wait(), waitpid(),
 * write(), writev()
 *
 * Furthermore the following function /may/ incur cancellation points:
 *
 * catclose(), catgets(), catopen(), closedir(), closelog(), ctermid(),
 * dlclose(), dlopen(), fclose(), fcntl(), fflush(), fgetc(), fgetpos(),
 * fgets(), fopen(), fprintf(), fputc(), fputs(), fread(), freopen(),
 * fscanf(), fseek(), fsetpos(), ftell(), fwrite(), getc(), getc_unlocked()
 * getchar(), getchar_unlocked(), getcwd(), getgrgid(), getgrgid_r(),
 * getgrnam(), getgrnam_r(), getlogin(), getlogin_r(), getpwnam(),
 * getpwnam_r(), getpwuid(), getpwuid_r(), gets(), getwd(), glob(), ioctl(),
 * lseek(), mkstemp(), opendir(), openlog(), pclose(), perror(), popen(),
 * printf(), putc(), putc_unlocked() putchar(), putchar_unlocked(), puts(),
 * readdir(), readdir_r(), remove(), rename(), rewind(), rewinddir(), scanf(),
 * semop(), strerror(), syslog(), tmpfile(), tmpnam(), ttyname(), ttyname_r(),
 * ungetc(), unlink(), vfprintf(), vprintf().
 *
 * Compile using: gcc -o 3-cancel-deferred 3-cancel-deferred.c -lpthread
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>

void* thread (void* arg)
{
    int ret;

    /* Let's set the cancellation state to DISABLE while we do some
     * initialization.  We don't want to get interrupted while we are
     * setting up the thread.  If a cancellation request is sent to the
     * thread at this time, it will be processed once the cancellation
     * state is switched to ENABLED */
    printf("!! Thread -- setting cancellation state: DISABLED\n");
    ret = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcancelstate()\n");
        exit(EXIT_FAILURE);
    }

    /* Simulate some initialization */
    sleep(5);

    printf("!! Thread -- setting cancellation state: ENABLED\n");
    ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_setcancelstate()\n");
        exit(EXIT_FAILURE);
    }

    /* Note that sleep() is a cancelation point (see above notes). */
    sleep(9001);

    /* Alternatively, we could have introduced a cancelation point
     * calling pthread_testcancel() instead. */

    printf("!! Thread -- I shouldn't be able to say this!\n");
    return NULL;
}


int main (int argc, char** argv)
{
    pthread_t tid;
    void* tid_ret;
    int ret;

    ret = pthread_create(&tid, NULL, thread, NULL);
    if (ret) {
        fprintf(stderr, "error -- pthread_create()\n");
        exit(EXIT_FAILURE);
    }
    printf ("Created new thread (tid: %x)\n", (unsigned int)tid);

    ret = pthread_cancel(tid);
    printf("Sent cancellation request\n");

    if (ret) {
        fprintf(stderr, "error -- pthread_cancel()\n");
        pthread_join(tid, NULL);
        exit(EXIT_FAILURE);
    }

    ret = pthread_join(tid, &tid_ret);
    if (tid_ret == PTHREAD_CANCELED)
        printf ("Thread (tid: %x) successfully cancelled.\n", (unsigned int)tid);
    else
        printf ("Thread (tid: %x) was not cancelled!\n", (unsigned int)tid);

    return 0;
}
