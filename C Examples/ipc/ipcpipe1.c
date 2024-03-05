/*
 * ipcpipe1.c - Using a pipe to communicate data from
 *              a child process to a parent process
 */

#include <stdio.h>     // needed for printf, perror
#include <stdlib.h>    // needed for exit
#include <sys/types.h> // needed for pid_t
#include <sys/wait.h>  // needed for wait system call
#include <unistd.h>    // needed for fork, read, write, close system calls

int main()
{
    /* Create a pipe */
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // fork into 2 processes

    if (pid == -1) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child
    {
        /* Close unused write end */
        close(pipefd[1]);

        /* Read from parent */
        char data[32];
        read(pipefd[0], data, sizeof(data));
        printf("Child received %s from parent\n", data);

        /* Close pipe */
        close(pipefd[0]);

        return 0; // Return success
    }
    else // parent
    {
        /* Close unused read end */
        close(pipefd[0]);

        /* Write to child */
        write(pipefd[1], "HELLO WORLD!", 12);

        /* Close pipe */
        close(pipefd[1]);

        /* Wait or child */
        wait(0);
    }
    return 0; // Return success
}
