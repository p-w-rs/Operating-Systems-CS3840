/*
 * pthreads5.c - Shows a problem that can happen with parameters
 *               passed to thread routines due to the fact that
 *               the parameter must be a pointer.  The referenced
 *               location must be valid for the entire life of the
 *               thread.
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
//
// NOTE: that parameters and return value are pointers
void *thread_routine(void *args)
{
    int *val = (int *)args;
    if (is_prime(*val))
    {
        printf("THREAD %lu FOUND that %d is prime\n", pthread_self(), *val);
    }
    else
    {
        printf("THREAD %lu FOUND that %d is not prime\n", pthread_self(), *val);
    }
    return NULL;
}

// start_thread
//   Creates a new thread and returns the thread ID
//
// NOTE: the parameter to the thread is created on the stack
//       the address becomes invalid when the function returns
pthread_t start_thread(int val)
{
    pthread_t thr;
    printf("Before thread creation val=%d\n", val);
    if (pthread_create(&thr, NULL, thread_routine, (void *)&val) == -1)
    {
        printf("COULD NOT CREATE A THREAD\n");
        exit(EXIT_FAILURE);
    }
    return thr;
}

int main()
{
    pthread_t thr1 = start_thread(2);
    pthread_t thr2 = start_thread(100);
    pthread_join(thr1, NULL);
    pthread_join(thr2, NULL);
    return 0;
}
