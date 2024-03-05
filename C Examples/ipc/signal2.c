/*
 * signal2.c - Shows how to set up a signal handler
 *             for the segmentation fault signal
 *             Unfortunately does not fix the segmentation
 *             fault and essentially loops forever
 */

#include <signal.h>    // needed for signal system call
#include <stdio.h>     // needed for printf
#include <sys/types.h> // needed for pid_t
#include <unistd.h>    // needed for getpid, sleep system calls

// Signal handler
//    Prints the PID of the process and the received signal
void signal_handler(int sig)
{
    pid_t pid = getpid();
    printf("Process %d received signal %d\n", pid, sig);
}

int main()
{
    // Set up a signal handler for the segmentation fault signal
    signal(SIGSEGV, signal_handler);

    // Cause a segmentation fault
    int *i = NULL;
    return *i + 10;
}
