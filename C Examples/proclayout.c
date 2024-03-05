/*
 * proclayout.c - Prints the location of variables stored
 *                at different locations in a process's
 *                address space
 */
#include <stdio.h>  // needed for printf
#include <stdlib.h> // needed for malloc, free

int i = 0;
int main()
{
    int j = 0;
    int *k = malloc(sizeof(int));
    printf("&main = %x\n", main);
    printf("&i = %x\n", &i);
    printf("k = %x\n", k);
    printf("&j = %x\n", &j);
    free(k);
    return 0;
}