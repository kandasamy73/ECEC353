/* A basic example of how function pointers work.
 *
 * This example consists of the following three files:
 *   -- function_pointers.c  (this file)
 *   -- array_ops.c
 *   -- array_ops.h
 * 
 *   Author: James A. Shackleford
 *     Date: January 19th, 2015
 *
 * This program demonstrates the use of function pointers in C.  Although not
 * strictly manditory to understand for this course, it does provide quite a
 * bit of insight as to the internal workings of functions such as
 * pthread_create().
 *
 * Compile using:
 *   gcc -o function_pointers function_pointers.c -L. -larray_ops
 */

#include "array_ops.h"
#include <stdlib.h>
#include <stdio.h>


/*--------------------------------------------------------------------
 * Define some "callback functions" to pass as function pointers.
 *   These are our array operations.
 *--------------------------------------------------------------------*/
int add (int a, int b)
{
    return a + b;
}

int mul (int a, int b)
{
    return a * b;
}

int n_choose_k (int a, int b)
{
    /* here N = a and k = b */

    if (!b)
        return 1;

    return (a * n_choose_k(a-1, b-1)) / b;
}
/*--------------------------------------------------------------------*/

void print_result(int* A, int* B, int N, int* C, char* op)
{
    int i;

    for (i=0; i<N; i++) {
        printf("%i %s %i = %i\n", A[i], op, B[i], C[i]);
    }
    printf("------------------------------\n");
}


int main(int argc, char *argv[])
{
    int A[] = {1, 2, 3, 4, 5, 6};
    int B[] = {6, 5, 4, 3, 2, 1};
    int N  = sizeof(A)/sizeof(int);
    int NB = sizeof(B)/sizeof(int);

    int* result_add;
    int* result_mul;
    int* result_nck;

    int largest_mul;
    int largest_nck;

    /* Arrays A and B must contain the same number of elements */
    if (N != NB) {
        fprintf(stderr, "error -- arrays A[] and B[] are different sizes\n");
        exit(EXIT_FAILURE);
    }

    /* Initialize our API */
    init_my_api();

    /* Use the API to generate some arrays using our callback functions */
    result_add = my_api_create(A, B, N, add);
    if (!result_add) {
        fprintf(stderr, "error -- my_api_create() failed\n");
        exit(EXIT_FAILURE);
    }
    result_mul = my_api_create(A, B, N, mul);
    if (!result_mul) {
        fprintf(stderr, "error -- my_api_create() failed\n");
        exit(EXIT_FAILURE);
    }
    result_nck = my_api_create(A, B, N, n_choose_k);
    if (!result_nck) {
        fprintf(stderr, "error -- my_api_create() failed\n");
        exit(EXIT_FAILURE);
    }
        
    /* Give some user feedback */
    print_result(A, B, N, result_add, "+");
    print_result(A, B, N, result_mul, "*");
    print_result(A, B, N, result_nck, "choose");

    /* Destroy the result arrays, reap their entries from the
     * largest_number_table, and store the largest values from
     * the table for our multiply and n choose k arrays
     * (don't store the largest value from addition array)
     */
    my_api_reap(result_add, NULL);
    my_api_reap(result_mul, &largest_mul);
    my_api_reap(result_nck, &largest_nck);

    printf ("Largest value from  \"multiply\"  array was: %i\n", largest_mul);
    printf ("Largest value from \"n choose k\" array was: %i\n", largest_nck);

    return 0;
}
