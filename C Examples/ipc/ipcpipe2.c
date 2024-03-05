/*
 * ipcpipe2.c - Pipes are unidirectional and require
 *              two pipes to send data in two directions
 */

#include <stdio.h>     // needed for printf, perror
#include <stdlib.h>    // needed for exit
#include <sys/types.h> // needed for pid_t
#include <sys/wait.h>  // needed for wait system call
#include <unistd.h>    // needed for fork, read, write, close system calls

int main()
{
    /* Create pipes */
    int pipe_to_child[2];
    if (pipe(pipe_to_child) == -1)
    {
        printf("PIPE FAILURE\n");
        exit(EXIT_FAILURE);
    }
    int pipe_from_child[2];
    if (pipe(pipe_from_child) == -1)
    {
        printf("PIPE FAILURE\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // fork into 2 processes

    if (pid == -1) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) //
    {
        /* Close unused pipe ends */
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);

        /* Write to parent */
        write(pipe_from_child[1], "CHELLO", 6);
        close(pipe_from_child[1]);

        /* Read from parent */
        char data[32];
        read(pipe_to_child[0], data, sizeof(data));
        printf("Child received %s from parent\n", data);

        /* Close pipe */
        close(pipe_to_child[0]);

        return 0; // Return success
    }
    else // parent
    {
        /* Close unused pipe ends */
        close(pipe_to_child[0]);
        close(pipe_from_child[1]);

        /* Write to child */
        write(pipe_to_child[1], "PHELLO", 6);
        close(pipe_to_child[1]);

        /* Read from child */
        char data[32];
        read(pipe_from_child[0], data, sizeof(data));
        printf("Parent recieved %s from child\n", data);

        /* Close pipe */
        close(pipe_from_child[0]);

        /* Wait for child */
        wait(0);
    }
    return 0; // Return success
}
