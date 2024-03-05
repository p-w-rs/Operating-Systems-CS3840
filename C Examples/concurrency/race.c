/*
 * race.c - Shows the race condition that can happen
 *          then threads try to modify multiple global
 *          values at the same time
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

// start flag
volatile int start = 0;

// shared global
static int counter1 = 0;
static int counter2 = 0;

void *thread_routine(void *args)
{
    int me = *((int *)args);
    int you = me ? 0 : 1;

    printf("Worker thread: %d ready, you are %d\n", me, you);

    // wait for start from master thread
    while (!start)
        ;

    for (int j = 0; j < 100000000; j++)
    {
        // this is the critical section
        counter1++;
        counter2++;
        // leaving critical section
    }

    printf("Worker thread: %d done\n", me);

    return NULL;
}

int main()
{
    int val1 = 0;
    int val2 = 1;

    pthread_t thr1;
    pthread_t thr2;
    if (pthread_create(&thr1, NULL, thread_routine, (void *)&val1) == -1)
    {
        printf("COULD NOT CREATE A THREAD\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thr2, NULL, thread_routine, (void *)&val2) == -1)
    {
        printf("COULD NOT CREATE A THREAD\n");
        exit(EXIT_FAILURE);
    }

    start = 1;

    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

    printf("counter1: %d\n", counter1);
    printf("counter2: %d\n", counter2);
    return EXIT_SUCCESS;
}
