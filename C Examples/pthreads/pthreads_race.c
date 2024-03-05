/*
 * pthreads_race.c - Shows a "race condition" when multiple threads
 *                   try to race to modify a global value
 *                   Due to unpredictable behavior in context switching
 *                   the value of global_value does not result in what is
 *                   expected
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int global_value[5] = {0};

void *thread_routine(void *arg)
{
    int *idx = (int *)(arg);
    for (int i = 0; i < 1000000; i++)
    {
        global_value[*idx]++;
    }
    return NULL;
}

int main()
{
    // Create 5 threads
    pthread_t thr_id[5];
    int args[5] = {0, 1, 2, 3, 4};
    for (int i = 0; i < 5; i++)
    {
        if (pthread_create(&thr_id[i], NULL, thread_routine, (void *)&args[i]) == -1)
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
    for (int i = 0; i < 5; i++)
        printf("PARENT READS GLOBAL: %d\n", global_value[i]);

    // Return success
    return 0;
}
