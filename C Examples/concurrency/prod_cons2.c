/*
 * prod_cons1.c - Shows the producer and consumer problem
 *                Access is controlled using semaphores
 *                One semaphore counts the number of filled spots
 *                One semaphore counts the number of empty spots
 *                Producers wait for at least one free spot before writing
 *                Consusmers wait for at least one filled spot before reading
 */

#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
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

// Semaphores for controlling access
sem_t *fullCount;
sem_t *emptyCount;

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
        // Wait for a free spot in the buffer
        sem_wait(emptyCount);

        // Add an element to the buffer
        unsigned char value = (i % 100) + 1; // Make sure the value isn't zero
        buffer_insert(&buffer, value);
        printf("Producer added: %d\n", value);

        // Signal the consumer that there something to consume
        sem_post(fullCount);
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
        // Wait for element to consume
        sem_wait(fullCount);

        // Remove an element from the buffer
        printf("Consumer removed: %d\n", buffer_remove(&buffer));

        // Signal the producer that there is a free spot
        sem_post(emptyCount);
    }

    return NULL;
}

// Main process
int main()
{
    // sem_init(&fullCount, 0, 0);
    fullCount = sem_open("sem1", O_CREAT, 0777, 0);
    // sem_init(&emptyCount, 0, BUFFER_SIZE);
    emptyCount = sem_open("sem2", O_CREAT, 0777, BUFFER_SIZE);
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

    sem_unlink("sem1");
    // sem_destroy(&fullCount);
    sem_unlink("sem2");
    // sem_destroy(&emptyCount);

    return EXIT_SUCCESS;
}
