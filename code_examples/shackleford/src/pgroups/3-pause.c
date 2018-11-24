/* This is a simple modification of the previous example.  It is setup
 * to trigger a SIGTTOU, which is caused by a process not in the
 * foreground process group attempting to write to stdio.  However,
 * instead of getting stopped this time, we install a signal handler to
 * to intelligently wait until we are in the foreground again.
 *
 *  Author: James A. Shackleford
 *    Date: Feb 14th, 2018
 *
 * Before running this example first turn on strict SIGTTOU adherance at
 * the command prompt via:
 *
 *   $ stty tostop
 *
 * To later re-enable loose adherance:
 *
 *   $ stty -tostop
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>


sig_atomic_t child_dead = 0;


void set_fg_pgid (pid_t pgid)
{
    void (*old)(int);

    old = signal (SIGTTOU, SIG_IGN);
    tcsetpgrp (STDIN_FILENO, pgid);
    tcsetpgrp (STDOUT_FILENO, pgid);
    signal (SIGTTOU, old);
}


void safe_print (char* str)
{
    pid_t fg_pgid;

    fg_pgid = tcgetpgrp (STDOUT_FILENO);

    set_fg_pgid (getpgrp());
    printf ("%s", str);
    set_fg_pgid (fg_pgid);
}


void handler (int sig)
{
    pid_t child;
    int status;

    while ((child = waitpid (-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        if (WIFSTOPPED (status)) {
            safe_print ("Child Stopped\n");
            continue;
        }
        else if (WIFCONTINUED (status)) {
            safe_print ("Child Continued\n");
            continue;
        }
        else {
            safe_print ("Child terminated\n");
            child_dead = 1;

            /* child is done
             * set foreground process group to parent's */
            set_fg_pgid (getpgrp());
            continue;
        }
    }
}


void handler_sigttou (int sig)
{
    /* we got here because we tried to write to stdout while in the
     * background.  let's wait (blocking) for a signal to come in
     * and then check again if we have become the foreground process
     * group.  ideally, we will catch a SIGCHLD in here, which will give
     * us the foreground again, which should cause use to exit this
     * handler */
    while (tcgetpgrp(STDOUT_FILENO) != getpid ())
        pause ();
}


int main (int argc, char** argv)
{
    pid_t pid;

    signal (SIGCHLD, handler);
    signal (SIGTTOU, handler_sigttou);

    pid = fork ();
    if (pid < 0) {
        fprintf (stderr, "Failed to create child\n");
        exit (EXIT_FAILURE);
    }

    /* Here we put the child in its own process group */
    setpgid (pid, pid);

    /* CHILD */
    if (pid == 0) {
        printf ("Child says, \"You have 60 sec to send me a signal...\"\n");
        execlp ("sleep", "sleep", "60", NULL);
    }

    printf ("Created child -- PID: %i\n", pid);

    /* set the child's process group to foreground */
    set_fg_pgid (pid);

    /* generates a SIGTTOU directed towards the parent */
    printf ("HI MOM!\n");

    /* parent just waits for signals until the child dies */
    while (!child_dead)
        pause();

    set_fg_pgid (getpgrp());
    printf ("parent -- have FG: exiting\n");

    return 0;
}
