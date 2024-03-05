/*
 * prod_con1.c - Producer and consumer with unlimited sized buffer
 *               Works with a single semaphore to count full slots
 *               when only a single producer and consumer is used
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define ELEMENTS 100

int count = 0;

// Semaphores for controlling access
sem_t fullCount;

// Start flag
volatile int start = 0;

// Producer Routine
void* producer()
{
    // wait for start from master thread
    while(!start);

    for(int i = 0; i < ELEMENTS; i++) {

        // Add an element
        count++;
        printf("Producer %lu added, count: %d\n", pthread_self(), count);

        // Signal the consumer that there something to consume
        sem_post(&fullCount);
        usleep(200);
    }

    return NULL;
}

// Consumer Routine
void* consumer()
{
    // wait for start from master thread
    while(!start);

    for(int i = 0; i < ELEMENTS; i++) {
        // Wait for element to consume
        sem_wait(&fullCount);

        // Remove an element from the buffer
        if(count == 0) {
            printf("ERROR: Customer %lu removed with no elements\n", pthread_self());
            exit(EXIT_FAILURE);
        }
        count--;
        usleep(50);
    }

    return NULL;
}


// Main thread
int main()
{
    sem_init(&fullCount, 0, 0);

    pthread_t prod;
    pthread_t cons;

    if(pthread_create(&prod, NULL, producer, NULL) == -1) {
        printf("COULD NOT CREATE PRODUCER\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&cons, NULL, consumer, NULL) == -1) {
        printf("COULD NOT CREATE CONSUMER\n");
        exit(EXIT_FAILURE);
    }

    start = 1;

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    printf("Final count is: %d, expected 0\n", count);

    sem_destroy(&fullCount);

    return EXIT_SUCCESS;
}
