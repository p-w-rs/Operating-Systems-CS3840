/*
 * pthreads1.c - Shows how to create and wait for single
 *               thread (beyond the main thread)
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_value = 10;

// thread_routine
//   writes to a global variable - shows that
//   global data is shared between threads
//
// NOTE: that parameters and return value are pointers
void *thread_routine()
{
    for (int i = 0; i < 10; i++)
    {
        printf("THREAD READS GLOBAL VALUE: %d\n", global_value);
        global_value++;
        sleep(1);
    }
    return NULL;
}

int main()
{
    // Create a thread
    pthread_t thr_id;
    if (pthread_create(&thr_id, NULL, thread_routine, NULL) == -1)
    {
        printf("COULD NOT CREATE A THREAD\n");
        exit(EXIT_FAILURE);
    }

    // Write to global data
    for (int i = 0; i < 10; i++)
    {
        printf("PARENT READS GLOBAL: %d\n", global_value);
        sleep(1);
    }

    // Wait for thread to finish
    pthread_join(thr_id, NULL);

    // Return success
    return 0;
}
