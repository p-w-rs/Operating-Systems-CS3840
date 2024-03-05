/*
 * pthreads6.c - Shows the problems that can happen due to the fact
 *               that thread routine parameters are pointers to memory
 *               locations.  Threads can easily affect other threads
 *               memory.
 *
 *               NOTE: Context switches between threads can happen when unexpected
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Function to check if a number is prime
//   Returns 1 if prime, 0 if not prime
int is_prime(int v)
{
    for (int i = 2; i < v; i++)
    {
        if (v % i == 0)
        {
            return 0;
        }
    }
    return 1;
}

// Thread routine
//    calls is_prime to check if the input argument is prime
//    Modifies the input parameter by dereferencing the pointer
//
// NOTE: that parameters and return value are pointers
void *thread_routine(void *args)
{
    int *val = (int *)args;
    if (is_prime(*val))
    {
        printf("THREAD %lu FOUND that %d is prime %p\n", pthread_self(), *val, val);
        *val = 1;
    }
    else
    {
        printf("THREAD %lu FOUND that %d is not prime %p\n", pthread_self(), *val, val);
        *val = 0;
    }
    return NULL;
}

int main()
{
    int val = 0;

    // Create 10 threads
    pthread_t thr[10];
    for (int i = 0; i < 10; i++)
    {

        // NOTE: there is a single memory location for val which is passed to all threads
        val = i + 2;
        if (pthread_create(&thr[i], NULL, thread_routine, (void *)&val) == -1)
        {
            printf("COULD NOT CREATE A THREAD\n");
            exit(EXIT_FAILURE);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < 10; i++)
    {
        pthread_join(thr[i], NULL);
    }

    // Return success
    return 0;
}
