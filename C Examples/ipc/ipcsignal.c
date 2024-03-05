/*
 * ipcsignal.c - Shows how to use the kill system call
 *               to send a signal from one process to another
 */

#include <signal.h>    // needed for signal system call
#include <stdio.h>     // needed for printf, perror
#include <stdlib.h>    // needed for exit
#include <sys/types.h> // needed for pid_t
#include <sys/wait.h>  // needed for wait system call
#include <unistd.h>    // needed for fork, getpid, getppid, kill system calls

void signal_handler(int sig)
{
    pid_t pid = getpid();
    printf("Process %d received signal %d\n", pid, sig);
}

int main()
{
    // Set a signal handler for a user defined signal number 1
    signal(SIGUSR1, signal_handler);

    pid_t pid = fork(); // fork into 2 processes

    if (pid < 0) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child
    {
        // Send a signal to the parent by its PID
        pid_t parent_pid = getppid();
        printf("Sending SIGUSR1(%d) to %d\n", SIGUSR1, parent_pid);
        kill(parent_pid, SIGUSR1);

        return 0; // Return success
    }
    else // parent
    {
        // Wait for child
        wait(0);
    }
    return 0; // Return success
}
