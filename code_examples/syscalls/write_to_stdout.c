/*
 * Author: Michael Lui
 *
 * Demonstrates multiple ways to write to stdout.
 * A syscall is different than a typical userspace function.
 * Userspace code 'stops' at the syscall and the kernel takes over.
 *
 * Will only work as expected on 64-bit Linux
 */

#include "stdlib.h"
#include "stdio.h"       // printf
#include "unistd.h"      // most I/O syscalls
#include "sys/syscall.h" // __NR_xxx defs

int main()
{
    /* MUST REQUEST A SWITCH TO KERNEL MODE FOR I/O TO THE TERMINAL 
     * 4 ways to perform a 'write' syscall */

    /* my data to pass to the kernel for I/O */
    char buf[] = "Hello World!\n";

    /* Using C standard library (alternatively puts)
     * Always prefer this method */
    printf("%s", buf);

    /* Using syscall wrappers, still high level API but more general
     * (we could also write to an open file or stderr) */
    write(STDOUT_FILENO, buf, sizeof(buf));

    /* Essentially the same as 'write'
     * Only useful if using nonstandard syscalls
     * (what if the kernel changes its implementation?) */
    syscall(__NR_write, STDOUT_FILENO, buf, sizeof(buf));

    /* What a syscall actually looks like.
     * This implementation is only valid on 64-bit Linux w/ x86_64 */
    int ret;
    asm(
        "mov %1, %%rax\n" // syscall number (write in this case)
        "mov %2, %%rdi\n" // stdout
        "mov %3, %%rsi\n" // buf
        "mov %4, %%rdx\n" // sizeof(buf)
        "syscall\n"       // Switch to kernel mode and do syscall
        "mov %%eax, %0\n" // Check the return value

        /* Don't sweat the assembly syntax, it's not remotely a requirement
         * If interested: http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Extended-Asm.html */

        : "=g" (ret)
        : "i" (__NR_write), "i" (STDOUT_FILENO), "g" (buf), "i" (sizeof(buf))
        : "%rax", "%rdi", "%rsi", "%rdx"
       );

	/* Make sure our assembly worked */
	if (ret != sizeof(buf))
	{
		printf("'write' error!\n"); // ...how meta...
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}
