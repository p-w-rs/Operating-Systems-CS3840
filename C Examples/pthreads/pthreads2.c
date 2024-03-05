/*
 * pthreads2.c - Shows how to create and wait for multple
 *               threads (beyond the main thread)
 *               Threads prints out its thread ID and process ID.
 *               Shows that threads share the same process ID
 *               but all have their own thread ID
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
void* thread_routine()
{
    for(int i = 0; i < 10; i++) {
        printf("%lu %d: THREAD READS GLOBAL VALUE: %d\n", pthread_self(), getpid(), global_value);
        global_value++;
        sleep(1);
    }
    return NULL;
}

int main()
{
    // Create 5 threads
    pthread_t thr_id[5];
    for(int i = 0; i < 5; i++) {
        if(pthread_create(&thr_id[i], NULL, thread_routine, NULL) == -1) {
            printf("COULD NOT CREATE A THREAD\n");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all threads to finish
    for(int i = 0; i < 5; i++) {
        pthread_join(thr_id[i], NULL);
    }

    // Print out final value of global data
    printf("%lu %d PARENT READS GLOBAL: %d\n", pthread_self(), getpid(), global_value);

    // Return success
    return 0;
}
