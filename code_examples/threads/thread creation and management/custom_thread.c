#define _GNU_SOURCE
#include "sched.h" //CLONE syscall

#include "sys/types.h"
#include "sys/wait.h"

#include "unistd.h"
#include "stdio.h" //printf
#include "stdlib.h" //malloc

#define STACK_SIZE (1ULL<<22) //4MB

/*
 * Example showing how clone is used to implement 'threads'.
 * Actual Pthreads (NPTL) provided by Linux are more complex.
 */

/* global value */
int val = 10;

/* function for our custom thread */
int threadfunc(void *param)
{
	val++;
	return 0;
}

int main()
{
	/* 'clone' requires that we give the child its own stack */
	char *child_stack = malloc(STACK_SIZE);
	if (child_stack == NULL)
	{
		perror("malloc");
		return EXIT_FAILURE;
	}

	/* Clone this process with special flags to act like a new thread.
	 *
	 * Actual threads (as implemented in pthreads) use some different
	 * flags that make it difficult to synchronize on the 'thread' exit.
	 * Pthreads do a lot more book-keeping on currently running threads.
	 *
	 * Those different flags (like CLONE_THREAD) are necessary to implement
	 * POSIX compliant threads as expected in Linux. However, they can muck
	 * with basic C library functionality like printf that expect */
	int child_pid = clone(&threadfunc, child_stack+(STACK_SIZE),
						  CLONE_VM|SIGCHLD, NULL);
	if (child_pid < 0)
	{
		perror("clone");
		return EXIT_FAILURE;
	}

	if (waitpid(child_pid, 0, 0) < 0)
	{
		perror("waitpid");
		return EXIT_FAILURE;
	}

	/* clean up */
	free(child_stack);

	/* normally val in this address space would be untouched,
	 * except our CLONE_* flags shared our virtual memory */
	printf("val is: %d\n", val);

	return EXIT_SUCCESS;
}
