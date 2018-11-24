/*  Solution to Programming Assignment 4
 *
 *    Author: James A Shackleford
 *      Date: Feb. 19th, 2016
 *   Updated: Feb. 22nd, 2016
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>

#define FALSE 0
#define TRUE !FALSE

#define NUM_PRIMES 10000000

static unsigned int setjmpset;
static unsigned int num_found;
static unsigned int primes[NUM_PRIMES];
static sigjmp_buf env;

void handler(int sig)
{
    alarm(5);
    siglongjmp(env, 1);
}

int is_prime(unsigned int num)
{
    int i;

    if (num < 2) {
        return FALSE;
    }
    else if (num == 2) {
        return TRUE;
    }
    else if (num % 2 == 0) {
        return FALSE;
    }
    else {
        for (i=3; (i*i)<=num; i+=2) {
            if (num % i == 0) {
                return FALSE;
            }
        }
        return TRUE;
    }
}

void report()
{
    int i;

    printf("Found %u primes.\n", num_found);
    printf("Last 5 primes found:\n   ");

    for (i=num_found-5; i<num_found; i++) {
        printf("%u ", primes[i]);
    }
    printf("\n\n");
}

int main(int argc, char** argv)
{
    unsigned int try = 0;
    sigset_t prev, block;

    sigemptyset(&block);
    sigaddset(&block, SIGALRM);

    if (sigsetjmp(env, TRUE)) {
        report();
    } else {
        num_found = 0;
        signal(SIGALRM, handler);
        handler(SIGALRM);
    }

    while (num_found < NUM_PRIMES) {
        sigprocmask(SIG_BLOCK, &block, &prev);
        if (is_prime(try)) {
            primes[num_found++] = try;
        }
        try++;
        sigprocmask(SIG_SETMASK, &prev, NULL);
    }

    return 0;
}
