/* Program demonstrating the use of setjmp and longjmp. 
 *
 * Author: Naga Kandasamy
 * Date created: 6/28/2018
 *
 * Notes: Original code from The Linux Porgramming Inteface. pp. 134-135.
 *
 * */

#include <stdio.h>
#include <setjmp.h>

static jmp_buf env;

static void 
f2 (void)
{
    longjmp (env, 2);
}

static void 
f1 (int argc)
{
    if (argc == 1)
        longjmp (env, 1);
    f2 ();
}

int
main (int argc, char **argv)
{
    /* Establish a target for a later jump performed by longjmp(). This target is exactly the point in the program where the 
     * setjmp() call occurred. The setjmp() call returns 0 on the initial call; nonzero on return via 
     * longjmp().
     * */
    int ret = setjmp (env); /* Set return point for longjmp here */

    switch (ret){
        case 0: 
            printf ("Calling f1() after the initial setjmp(). \n");
            f1 (argc); 
            break;

        case 1: 
            printf ("We jumped back from f1(). \n");
            break;

        case 2: 
            printf ("We jumped back from f2(). \n");
            break;

        default:
            printf ("Unknown jump! \n");
    }
}
