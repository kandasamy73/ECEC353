#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <errno.h>

/* Make true and false situations more readable */
#define FALSE 0
#define TRUE !FALSE

/* Make calls to sem_init() more readable */
#define THREAD_LEVEL_SHARING 0
#define PROCESS_LEVEL_SHARING 1

/* Range of time it takes to travel & cut hair */
#define MIN_TIME 4
#define MAX_TIME 8

/* Data structure containing Barber's state information */
struct barber_data {
    pthread_t tid;       /* Barber's thread id                          */
    sem_t done_cutting;  /* Signal to Customers that Barber is finished */
    sem_t awake;         /* Signal from Customer to wakeup Barber       */
};

/* Data structure containing the Shop's state information */
struct shop_data {
    unsigned int num_chairs;    /* # chairs in waiting room                  */
    unsigned int num_customers; /* # customers to serve today                */
    unsigned int empty;         /* True when all customers are serviced      */
    sem_t waiting_room;         /* Throttle for waiting room seats           */
    sem_t barber_chair;         /* Throttle for the barber's chair           */

    struct barber_data* barber; /* Pointer to the barber's state information */
};

/* Data structure defining a customer */
struct customer_data {
    unsigned int id;         /* ID # of the customer                    */
    pthread_t tid;           /* Customer's thread id                    */

    struct shop_data* shop;  /* Pointer to the shop's state information */
};


/* This function prints the usage for the program and details what the various
 * command line parameters do */
void print_usage (char* prog)
{
    printf("Usage: %s customers chairs\n\n", prog);
    printf("   customers   -      # of customers visiting the barber shop\n");
    printf("   chairs      -      # of chairs in the barber shop waiting room\n\n");
}


/* Utility function returning a random number between 0 and max */
unsigned int random_uint(unsigned int max)
{
    float tmp = (float)rand()/(float)RAND_MAX * (float)max;
    return (unsigned int)ceilf(tmp);
}


/* Block a random amount of time between min seconds and max seconds */
void sleep_within (int min, int max)
{
    unsigned int cut_time = random_uint(max - min) + min;
    sleep(cut_time);
}


/* Thread defining Barber's behavior */
void* thread_barber (void* param)
{
    struct shop_data* shop;
    struct barber_data* barber;
    void(*cut_hair)(int min, int max);

    shop = (struct shop_data*)param;
    barber = shop->barber;
    cut_hair = sleep_within;

    while (!shop->empty) {
        printf("Barber -- sleeping\n");
        sem_wait(&barber->awake);

        printf("Barber -- cuttting hair\n");
        cut_hair(MIN_TIME, MAX_TIME);

        sem_post(&barber->done_cutting);
    }
}


/* Thread defining Customer behavior */
void* thread_customer (void* param)
{
    struct customer_data* customer;
    struct shop_data* shop;
    struct barber_data* barber;
    void(*travel_to_barber)(int min, int max);

    customer = (struct customer_data*)param;
    shop = customer->shop;
    barber = shop->barber;
    travel_to_barber = sleep_within;

    printf("[Customer #%u]  Traveling to barber shop\n", customer->id);
    travel_to_barber (MIN_TIME, MAX_TIME);
    printf("[Customer #%u]  Arrived outside barber shop.\n", customer->id);

    sem_wait(&shop->waiting_room);
    printf("[Customer #%u]  Entered waiting room\n", customer->id);

    sem_wait(&shop->barber_chair);
    sem_post(&shop->waiting_room);
    printf("[Customer #%u]  Exited waiting room. Sat in barber's chair.\n", customer->id);

    sem_post(&barber->awake);
    printf("[Customer #%u]  Hit barber on the back. Waiting for haircut to finish.\n", customer->id);

    sem_wait(&barber->done_cutting);
    printf("[Customer #%u]  Haircut complete.\n", customer->id);

    sem_post(&shop->barber_chair);
    printf("[Customer #%u]  Left barber's chair.  Going home.\n", customer->id);
}


/* This function parses the command line arguments and populates the
 * shop_data and barber_data data structures.  Also creates the
 * Barber's thread */
int barbershop_create (struct shop_data* shop, int argc, char** argv)
{
    int ret;
    struct barber_data* barber;

    /* did the user specify both command line parameters? */
    if (argc < 3) {
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    /* sanity check user input */
    if (atoi(argv[1]) <= 0) {
        fprintf(stderr, "error -- # of customers must be positive\n");
        print_usage(argv[0]);
        return -1;
    }

    if (atoi(argv[2]) <= 0) {
        fprintf(stderr, "error -- # of chairs must be positive\n");
        print_usage(argv[0]);
        return -1;
    }

    /* Allocate the data structure that will store barber's state */
    barber = malloc(sizeof(*barber));

    /* initialize shop data structure */
    shop->barber = barber;
    shop->num_chairs = atoi(argv[2]);
    shop->num_customers = atoi(argv[1]);
    ret = sem_init(&shop->waiting_room, THREAD_LEVEL_SHARING, shop->num_chairs);
    if (ret)
        return ret;
    ret = sem_init(&shop->barber_chair, THREAD_LEVEL_SHARING, 1);
    if (ret)
        return ret;

    /* initialize barber data structure */
    ret = sem_init(&barber->done_cutting, THREAD_LEVEL_SHARING, 0);
    if (ret)
        return ret;
    ret = sem_init(&barber->awake, THREAD_LEVEL_SHARING, 0);
    if (ret)
        return ret;

    /* create the barber thread */
    pthread_create(&barber->tid, NULL, thread_barber, shop);

    return 0;
}


/* Cleans up any resources allocated by barbershop_create() */
void barbershop_destroy (struct shop_data* shop)
{
    struct barber_data* barber = shop->barber;

    sem_destroy(&barber->done_cutting);
    sem_destroy(&barber->awake);
    free(barber);

    sem_destroy(&shop->waiting_room);
    sem_destroy(&shop->barber_chair);
}


/* Allocates an array of customer_data pointers, and then allocates a
 * customer_data for each pointer in the array.  Also creates a thread of each
 * customer, which is passed the pointer to that specific customer's data */
int customers_create (
    struct customer_data*** customer,
    struct shop_data* shop
)
{
    int i;
    struct customer_data** C;

    /* I introduce this "C" so we can simplify the levels of pointer
     * indirection. We will allocate an array of pointers to customer data
     * structures using malloc and store the starting address of the allocated
     * block of memory into the memory location passed to us through customer,
     * thusly:
     */
    *customer = malloc(sizeof(struct customer_data*)*shop->num_customers);

    /* After an assigment like: */
    C = *customer;

    /* the following statements are equilivant:
     *
     *   C[i] .......... same as ............ (*customer)[i]
     *   &C[i]->tid .... same as ............ &((*customer)[i]->tid)
     *
     * We are simply storing the dereference into "C" so that we don't
     * have to do the dereference on "customer" everytime. This makes the
     * code easier to read.
     */

    /* Allocate a customer data structure for each pointer in our array of
     * pointers to customer data structures and populate the fields of the
     * structure */

    for (i=0; i<shop->num_customers; i++) {
        C[i] = malloc(sizeof(struct customer_data));
        C[i]->id = i;
        C[i]->shop = shop;
        pthread_create(&C[i]->tid, NULL, thread_customer, C[i]);
    }

    return 0;
}


/* Cleans up any resources allocated by customers_create() */
void customers_destroy (struct customer_data*** customer, struct shop_data* shop)
{
    int i;
    struct customer_data** C = *customer;

    /* Free the memory for each customer_data we allocated */
    for (i=0; i<shop->num_customers; i++) {
        pthread_join(C[i]->tid, NULL);
        free(C[i]);
    }

    /* Free the array of customer_data pointers */
    free(C);
}


int main (int argc, char** argv)
{
    struct shop_data shop;
    struct customer_data** customer;
    int ret;

    shop.empty = FALSE;
    ret = barbershop_create(&shop, argc, argv);
    if (ret) {
        fprintf(stderr, "error -- create_barbershop() failure\n");
        exit(EXIT_FAILURE);
    }

    ret = customers_create(&customer, &shop);
    if (ret) {
        fprintf(stderr, "error -- create_customers() failure\n");
        exit(EXIT_FAILURE);
    }

    customers_destroy(&customer, &shop);
    shop.empty = TRUE;

    barbershop_destroy(&shop);

    return 0;
}
