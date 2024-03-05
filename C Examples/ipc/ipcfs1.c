/*
 * ipcsfs1 - Using the file system for inter-process communication
 *           Shows a problem with the fact that the file position
 *           pointer is shared between parent and child
 */

#include <fcntl.h>     // needed for open system call
#include <stdio.h>     // needed for printf
#include <stdlib.h>    // needed for exit
#include <string.h>    // nneded for strlen
#include <sys/types.h> // needed for pid_t
#include <sys/wait.h>  // needed for wait system call
#include <unistd.h>    // needed for fork, read, write, close system calls

int main()
{
    // Open a file for reading and writing
    // Create it if it doesn't exist
    int myfile = open("MYFILE", O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

    pid_t pid = fork(); // fork into 2 processes

    if (pid < 0) // error
    {
        printf("ERROR: COULD NOT FORK\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child
    {
        /* Write to parent */
        char *data = "HELLO";
        write(myfile, data, strlen(data));
        printf("Child wrote %s\n", data);

        /* Close file */
        close(myfile);

        return 0; // Return success
    }
    else // parent
    {
        /* Wait for child */
        wait(0);

        /* Read data */
        char data[32] = {0};
        read(myfile, data, sizeof(data));
        printf("Parent received %s from child\n", data);

        /* Close file */
        close(myfile);
    }
    return 0;
}
