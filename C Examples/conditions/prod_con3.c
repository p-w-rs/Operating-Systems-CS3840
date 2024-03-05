/*
 * prod_con3.c - Producer and consumer with unlimited sized buffer
 *               Fixes the problem when multiple producers and
 *               consumers modify a shared space by using 
 *               a mutex lock to prevent threads from modifying
 *               the space at the same time
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

#define ELEMENTS 100
#define PRODUCER_COUNT 10
#define CONSUMER_COUNT 5

int count = 0;

// Semaphores for controlling access
sem_t fullCount;

pthread_mutex_t element_mutex = PTHREAD_MUTEX_INITIALIZER;

// Start flag
volatile int start = 0;

// Producer Routine
void* producer()
{
    // wait for start from master thread
    while(!start);

    for(int i = 0; i < ELEMENTS; i++) {

        pthread_mutex_lock(&element_mutex);

        // Add an element
        count++;
        printf("Producer %lu added, count: %d\n", pthread_self(), count);

        // Signal the consumer that there something to consume
        sem_post(&fullCount);
        pthread_mutex_unlock(&element_mutex);
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
        pthread_mutex_lock(&element_mutex);
        if(count == 0) {
            printf("ERROR: Customer %lu removed with no elements\n", pthread_self());
            exit(EXIT_FAILURE);
        }
        count--;
        pthread_mutex_unlock(&element_mutex);
        usleep(50);
    }

    return NULL;
}


// Main thread
int main()
{
    sem_init(&fullCount, 0, 0);

    pthread_t prod[PRODUCER_COUNT];
    pthread_t cons[CONSUMER_COUNT];
    for(int i = 0; i < PRODUCER_COUNT; i++) {
        if(pthread_create(&prod[i], NULL, producer, NULL) == -1) {
            printf("COULD NOT CREATE PRODUCER\n");
            exit(EXIT_FAILURE);
        }
    }
    for(int i = 0; i < CONSUMER_COUNT; i++) {
        if(pthread_create(&cons[i], NULL, consumer, NULL) == -1) {
            printf("COULD NOT CREATE CONSUMER\n");
            exit(EXIT_FAILURE);
        }
    }

    start = 1;

    for(int i = 0; i < PRODUCER_COUNT; i++) {
        pthread_join(prod[i], NULL);
    }
    for(int i = 0; i < CONSUMER_COUNT; i++) {
        pthread_join(cons[i], NULL);
    }

    int expected_final_count = PRODUCER_COUNT*ELEMENTS - CONSUMER_COUNT*ELEMENTS;

    printf("Final count is: %d, expected %d\n", count, expected_final_count);

    sem_destroy(&fullCount);
    pthread_mutex_destroy(&element_mutex);

    return EXIT_SUCCESS;
}
