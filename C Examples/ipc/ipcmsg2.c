/*
 * ipcmsg2.c - Uses message queues to send multiple messages
 *             between two processes.  Shows what can happen
 *             with blocking queues when the queue gets full
 *             NOTE: message queues are persistent
 */

#include <sys/types.h> // needed for pid_t
#include <sys/wait.h>  // needed for wait system call
#include <unistd.h>    // needed for fork system call
#include <stdlib.h>    // needed for exit
#include <stdio.h>     // needed for printf, perror
#include <mqueue.h>    // needed for mq system calls
#include <string.h>    // needed for strcpy

/* Struct for the queue message */
typedef struct message {
    int message_id;
    char string[10];
} message;

int main()
{
    // Create attributes for new queue
    struct mq_attr queue_attr;
    queue_attr.mq_flags = 0;    // Ignored by kernel
    queue_attr.mq_maxmsg = 10;  // Max messages the queue supports
    queue_attr.mq_msgsize = sizeof(message);
    queue_attr.mq_curmsgs = 0;  // Not used

    // Create and open a queue
    mqd_t mqdes = mq_open("/CS3841QUEUE", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR, &queue_attr);
    if(mqdes == -1) {
        printf("COULD NOT OPEN QUEUE\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // fork into 2 processes

    if(pid < 0) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(pid == 0) // child
    {
        // Child receives message from parent
        message from_parent;
        mq_receive(mqdes, (char*)&from_parent, sizeof(message), NULL);
        printf("Child got %d: %s from parent\n", from_parent.message_id, from_parent.string);

        // Close the queue
        mq_close(mqdes);

        return 0; // Return success
    }
    else // parent
    {
        // Parent sends messages to child
        message to_child;
        for(int i = 0; i < 10; i++) {
            to_child.message_id = i;
            strcpy(to_child.string, "HELLO");
            mq_send(mqdes, (char*)&to_child, sizeof(message), 0);
        }

        // Wait for child
        wait(0);

        // Close the queue
        mq_close(mqdes);
    }
    return 0; // Return success
}
