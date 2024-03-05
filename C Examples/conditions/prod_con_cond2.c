/*
 * prod_con_cond2.c - Producer and consumer with unlimited sized buffer
 *                     Uses a condition variable to ensure proper
 *                     order of the consumers when they wait for more
 *                     items to be consumed.
 *                     Also uses a mutex to control the ordering of consumers
 *                     This example provides correct behavior when there
 *                     are multiple producers and multiple consumers and each
 *                     consumer orders multiple items.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define ELEMENTS 100
#define PRODUCER_COUNT 10
#define CONSUMER_COUNT 5
#define ORDER_COUNT 2

int count = 0;

pthread_mutex_t consumer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t element_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t has_elements = PTHREAD_COND_INITIALIZER;

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

        // Signal the consumer that there is something to consume
        pthread_cond_signal(&has_elements);
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

        // All consumers line up in order
        pthread_mutex_lock(&consumer_mutex);

        // Only one consumer gets to be at the head of the line
        pthread_mutex_lock(&element_mutex);

        for(int j = 0; j < ORDER_COUNT; j++) {

            // Wait for element to consume
            if(count == 0) {
                pthread_cond_wait(&has_elements, &element_mutex);
            }

            // Remove an element 
            if(count == 0) {
                printf("ERROR: Customer %lu removed with no elements\n", pthread_self());
                exit(EXIT_FAILURE);
            }
            count--;
        }

        // Let more consumers and producers run
        pthread_mutex_unlock(&element_mutex);

        // Let the next consumer at the head of the line order items
        pthread_mutex_unlock(&consumer_mutex);

        usleep(50);
    }

    return NULL;
}


// Main thread
int main()
{
    pthread_t prod[PRODUCER_COUNT];
    pthread_t cons[CONSUMER_COUNT];
    for(int i = 0; i < PRODUCER_COUNT; i++) {
        if(pthread_create(&prod[i], NULL, producer, NULL) == -1) {
            printf("COULD NOT CREATE PRODUCER\n");
            exit(EXIT_FAILURE);
        }
    }
    for(int i = 0; i < 5; i++) {
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

    printf("Final count is: %d\n", count);

    pthread_cond_destroy(&has_elements);
    pthread_mutex_destroy(&element_mutex);
    pthread_mutex_destroy(&consumer_mutex);

    return EXIT_SUCCESS;
}
