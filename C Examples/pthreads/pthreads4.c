/*
 * pthreads4.c - Shows how to pass parameter values to thread routines
 *               The threads dereference the pointer passed to the
 *               thread routine to demonstrate that memory is shared
 *               between all threads
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
        printf("THREAD %lu FOUND that %d is prime\n", pthread_self(), *val);
        *val = 1;
    }
    else
    {
        printf("THREAD %lu FOUND that %d is not prime\n", pthread_self(), *val);
        *val = 0;
    }
    return NULL;
}

int main()
{
    int val1 = 2;
    int val2 = 100;

    // Print out the values before creating threads
    printf("Parent val1=%d val2=%d\n", val1, val2);

    // Create 2 threads
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

    // Wait for the threads to finish
    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);

    // Print out the final values
    printf("Parent val1=%d val2=%d\n", val1, val2);

    // Return success
    return 0;
}
