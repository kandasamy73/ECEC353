/* Author: James A. Shackleford
 *   Date: Feb. 2nd, 2018
 *
 * Automatic pssh tester
 *
 * compile using:
 *  $ gcc -c -Wall -Werror -fpic grader.c
 *  $ gcc -shared -o grader.so grader.o
 *
 * copy grader.so into the same folder as pssh and run:
 *  $ LD_PRELOAD=./grader.so ./pssh
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* [-5pts] Program crashes/exits unexpectedly */

static char* test[] = {
/* [+5pts] TEST 0: Does the prompt show the current working directory? */

/* [+3pts] TEST 1: Single command */
    "ls",

/* [+3pts] TEST 2: Single command w/ args */
    "ls -l --classify --human",

/* [+3pts] TEST 3: Single command w/ args: redirect stdout to file */
    "ls -l > test3.txt",
/* TEST 4: Display test3.txt*/
    "cat test3.txt",

/* [+3pts] TEST 5: Single command w/ args: Redirect file to stdin */
    "grep -n \"\\.c\" < test3.txt",

/* [+3pts] TEST 6: Single command w/ args: Redirect stdin AND stdout */
    "grep -n \"\\.c\" < test3.txt > test6.txt",
/* TEST 7: Display test6.txt*/
    "cat test6.txt",

/* [+3pts] TEST 8: 2 level Pipeline w/ args */
    "ls -l | wc -l",

/* [+3pts] TEST 9: 3 level Pipeline w/ args */
    "ls -l | wc | awk '{print$3}'",

/* [+3pts] TEST 10: 3 level Pipeline w/ args: redirect file to stdin */
    "grep \"\\.c\" < test3.txt | wc | awk '{print$3}'",

/* [+3pts] TEST 11: 3 level Pipeline w/ args: redirect stdin AND stdout */
    "grep \"\\.c\" < test3.txt | wc | awk '{print$3}' > test11.txt",
/* TEST 12: Display test11.txt */
    "cat test11.txt",

/* [+3pts] TEST 13: Builtin which (no arguments) */
    "which",

/* [+3pts] TEST 14: Builtin which: ls */
    "which ls",

/* [+3pts] TEST 15: Builtin which: which */
    "which which",

/* [+3pts] TEST 16: Builtin which: exit */
    "which exit",

/* [+3pts] TEST 17: Builtin which: man : redirect stdout to file */
    "which man > test17.txt",

/* TEST 18: Display test17.txt */
    "cat test17.txt",

/* [+3pts] TEST 19: Builtin: exit */
    "exit",
/**********/
    NULL
};

void fake_type (const char* c)
{
    while (*c) {
        printf ("%c", *(c++));
        nanosleep (&(struct timespec){0, 40000000}, NULL);
        fflush (stdout);
    }
}

char* readline (const char *prompt)
{
    static unsigned int i;
    unsigned int j;
    char* simput;

    sleep(1);

    if (test[i]) {
        printf ("\n===[ TEST %u ]", i+1);
        for (j=0; j<58; j++) printf ("=");
        printf ("\n");

        simput = strdup (test[i]);
        printf ("%s", prompt);
        fake_type (simput);
        printf ("\n");
        i++;
    } else {
        printf ("\n\n");
        fake_type ("===[ BUILTIN COMMAND 'exit' FAILURE ]===");
        simput = NULL;
    }

    fflush(stdout);
    return simput;
}
