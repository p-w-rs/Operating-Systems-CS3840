/*
 * ipcshm2.c - Uses shared memory to send data from
 *             a child process to a parent process
 *             Uses shm_unlink to remove the shared memory when done
 *             NOTE: named shared memory segments are persistent
 */

#include <fcntl.h>     // needed for parameter values for shm_open
#include <signal.h>    // needed for signal system call
#include <stdio.h>     // needed for printf, perror
#include <stdlib.h>    // needed for exit
#include <string.h>    // needed for strcpy
#include <sys/mman.h>  // needed for mmap, munmap, shm system calls
#include <sys/types.h> // needed for pid_t
#include <sys/wait.h>  // needed for wait system call
#include <unistd.h>    // needed for fork, getpid, getppid, kill system calls

#define MAPPED_SIZE 128

int main()
{
    // Create and open a shared memory segment
    int shmfd = shm_open("/CS3841MEMORY", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (shmfd == -1)
    {
        printf("COULD NOT OPEN SHARED MEMORY SEGMENT\n");
        exit(EXIT_FAILURE);
    }

    // Set the size of the shared memory segment
    ftruncate(shmfd, MAPPED_SIZE);

    // Map the segment into the processes address space
    //    NOTE: protection is set to allow reading and writing with a shared mapping
    void *mapped_space = mmap(NULL, MAPPED_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmfd, 0);
    if (mapped_space == MAP_FAILED)
    {
        printf("COULD NOT MMAP\n");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork(); // fork into 2 processes

    if (pid < 0) // error
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child
    {
        // Child writes to shared memory segment
        strcpy(mapped_space, "HELLO");

        // Unmap the shared memory
        munmap(mapped_space, MAPPED_SIZE);

        // Close the shared memory segment
        close(shmfd);

        return 0; // Return success
    }
    else // parent
    {
        // Wait for child to finish
        wait(0);

        // Parent reads from shared memory segment
        printf("Parent reads %s from shared mapped segment\n", (char *)mapped_space);

        // Unmap the shared memory
        munmap(mapped_space, MAPPED_SIZE);

        // Close the shared memory segment
        close(shmfd);

        // Unlink the shared memory
        shm_unlink("/CS3841MEMORY");
    }
    return 0; // Return success
}
