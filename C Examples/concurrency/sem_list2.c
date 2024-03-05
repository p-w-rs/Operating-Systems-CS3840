/*
 * sem_list2.c - Uses a semaphore as a way to control access
 *               to a critical section for a singly linked list
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct node
{
    int val;
    struct node *next;
} node;

typedef struct list
{
    node *head;
} list;

// shared global
static list mylist;

// start flag
volatile int start = 0;

// flag semaphore
sem_t flag;

// The thread process
void *thread_routine()
{
    printf("Worker thread: %lu ready\n", pthread_self());

    // wait for start from master thread
    while (start == 0)
        ;

    for (int j = 0; j < 1000000; j++)
    {
        sem_wait(&flag);
        node *new_node = malloc(sizeof(node));
        new_node->val = j;
        new_node->next = mylist.head;
        mylist.head = new_node;
        sem_post(&flag);
    }

    printf("Worker thread: %lu done\n", pthread_self());

    return NULL;
}

// Main process
int main()
{
    sem_init(&flag, 0, 1);

#define THREAD_COUNT 10
    pthread_t thr_ids[THREAD_COUNT];

    mylist.head = NULL;

    // Create the threads
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (pthread_create(&thr_ids[i], NULL, thread_routine, NULL) == -1)
        {
            printf("COULD NOT CREATE A THREAD\n");
            exit(EXIT_FAILURE);
        }
    }

    // Signal threads to start
    start = 1;

    // Wait for all threads to finish
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(thr_ids[i], NULL);
    }

    // Count the elements in the list
    int length = 0;
    node *itr = mylist.head;
    while (itr != NULL)
    {
        length++;
        itr = itr->next;
    }
    printf("List contains %d elements\n", length);

    // Free the list
    while (mylist.head != NULL)
    {
        node *to_delete = mylist.head;
        mylist.head = mylist.head->next;
        free(to_delete);
    }

    sem_destroy(&flag);

    return EXIT_SUCCESS;
}
