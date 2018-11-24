/* This is a simple modification of the previous example.  It is setup
 * to trigger a SIGTTOU, which is caused by a process not in the
 * foreground process group attempting to write to stdio.
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

    /* CHILD */
    if (pid == 0) {
        printf ("Child says, \"You have 60 sec to send me a signal...\"\n");
        execlp ("sleep", "sleep", "60", NULL);
    }

    printf ("Created child -- PID: %i\n", pid);

    /* set the child's process group to foreground */
    set_fg_pgid (pid);

    /*  The following line results in the kernel issuing a SIGTTOU to
     *  the parent process because it will be trying to write to stdout
     *  as a background process. */
    printf ("HI MOM!\n");

    /* parent just waits for signals until the child dies */
    while (!child_dead)
        pause();

    set_fg_pgid (getpgrp());
    printf ("parent -- have FG: exiting\n");

    return 0;
}
