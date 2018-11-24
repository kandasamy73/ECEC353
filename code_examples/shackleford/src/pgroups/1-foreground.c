/* This example demonstrates:
 *   1. how to create a child process
 *   2. putting a child process in its own process group
 *   3. setting a process group to the foreground in the terminal
 *   4. handling SIGCHLD events using status
 *
 *  Author: James A. Shackleford
 *    Date: Feb 14th, 2018
 *
 * SIGTTIN is always sent if a background process tries to read from
 * stdin.
 *
 * SIGTTOU is a funny signal, though -- it is not *ALWAYS* sent when a
 * background process trys to write to stdout.  That behavior is
 * configurable in the user's tty settings.  It is always best to assume
 * that the user does not allow this. Changing this setting is simple.
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
            continue;
        }
    }
}


int main (int argc, char** argv)
{
    pid_t pid;

    signal (SIGCHLD, handler);

    pid = fork ();
    if (pid < 0) {
        fprintf (stderr, "Failed to create child\n");
        exit (EXIT_FAILURE);
    }

    /* Here we put the child in its own process group */
    setpgid (pid, pid);

    /*******
     * NOTE:
     *   Both the parent and the child are trying to call setpgid().
     *   This is so that the child will be removed from the parent's
     *   process group as soon as possible (i.e. we don't know if the
     *   parent or the child will be scheduled 1st after the fork).
     *
     *   - If the child's PID is 123, the parent is calling:
     *        tcsetpgrp (123, 123);
     *     + The program group will have the same PGID as the child's PID.
     *     + It is customary for the PID of the 1st process added to a process
     *       group to be used as the group's PGID.
     *
     *   - Meanwhile, the child is calling:
     *        tcsetpgrp (0, 0);
     *     + This has special meaning:
     *       + Supplying a PID of 0 means "use the caller's PID"
     *       + Supplying a PGID of 0 means "use the supplied PID as the PGID"
     ***************/

    /* CHILD */
    if (pid == 0) {
        printf ("Child says, \"You have 60 sec to send me a signal...\"\n");
        execlp ("sleep", "sleep", "60", NULL);
    }

    printf ("Created child -- PID: %i\n", pid);

    /* set the child's process group to foreground */
    set_fg_pgid (pid);

    /* parent just waits for signals until the child dies */
    while (!child_dead)
        pause();

    set_fg_pgid (getpgrp());
    printf ("parent -- have FG: exiting\n");

    return 0;
}
