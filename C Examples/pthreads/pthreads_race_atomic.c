/*
 * pthreads_race_atomic.c - Uses an atomic add instruction to prevent
 *                          the race condition that can happen when
 *                          multiple threads modify a global value at the
 *                          same time.
 */

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

atomic_int global_value = 0;

void *thread_routine()
{
    for (int i = 0; i < 1000000; i++)
    {
        atomic_fetch_add(&global_value, 1);
    }
    return NULL;
}

int main()
{
    // Create 5 threads
    pthread_t thr_id[5];
    for (int i = 0; i < 5; i++)
    {
        if (pthread_create(&thr_id[i], NULL, thread_routine, NULL) == -1)
        {
            printf("COULD NOT CREATE A THREAD\n");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < 5; i++)
    {
        pthread_join(thr_id[i], NULL);
    }

    // Print out final value of global value
    // 5 threads * 100000000 iterations - expected value is 500000000
    printf("PARENT READS GLOBAL: %d\n", global_value);

    // Return success
    return 0;
}
