/*
 * prod_cons1.c - Shows the producer and consumer problem
 *                When access to a limited shared space is not
 *                controlled there are chances where a producer thread
 *                might attempt to write when the space is full and
 *                a consumer might attempt to read when the space is empty
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFFER_SIZE 30
#define ELEMENTS 100

// Shared Buffer
typedef struct circular_buffer
{
    unsigned char values[BUFFER_SIZE];
    int out_idx;
    int in_idx;
} circular_buffer;

circular_buffer buffer;

void buffer_init(circular_buffer *b)
{
    b->out_idx = 0;
    b->in_idx = 0;
    memset(b->values, 0, BUFFER_SIZE);
}

void buffer_insert(circular_buffer *b, unsigned char value)
{
    if (b->values[b->in_idx] != 0)
    {
        printf("ERROR: Inserting into buffer when an element exists.  Empty was expected\n");
        exit(EXIT_FAILURE);
    }
    b->values[b->in_idx] = value;
    b->in_idx = b->in_idx < BUFFER_SIZE - 1 ? b->in_idx + 1 : 0;
}

unsigned char buffer_remove(circular_buffer *b)
{
    unsigned char return_value = b->values[b->out_idx];
    if (return_value == 0)
    {
        printf("ERROR: Removing from a buffer at an empty element.  Something was expected\n");
        exit(EXIT_FAILURE);
    }
    b->values[b->out_idx] = 0;
    b->out_idx = b->out_idx < BUFFER_SIZE - 1 ? b->out_idx + 1 : 0;
    return return_value;
}

// Start flag
volatile int start = 0;

// Producer Routine
void *producer()
{
    // wait for start from master thread
    while (!start)
        ;

    for (int i = 0; i < ELEMENTS; i++)
    {
        // Add an element to the buffer
        unsigned char value = (i % 100) + 1; // Make sure the value isn't zero
        buffer_insert(&buffer, value);
        printf("Producer added: %d\n", value);
    }

    return NULL;
}

// Consumer Routine
void *consumer()
{
    // wait for start from master thread
    while (!start)
        ;

    for (int i = 0; i < ELEMENTS; i++)
    {
        // Remove an element from the buffer
        printf("Consumer removed: %d\n", buffer_remove(&buffer));
    }

    return NULL;
}

// Main process
int main()
{
    buffer_init(&buffer);

    pthread_t prod;
    pthread_t cons;
    if (pthread_create(&prod, NULL, producer, NULL) == -1)
    {
        printf("COULD NOT CREATE PRODUCER\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&cons, NULL, consumer, NULL) == -1)
    {
        printf("COULD NOT CREATE CONSUMER\n");
        exit(EXIT_FAILURE);
    }

    start = 1;

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    return EXIT_SUCCESS;
}
