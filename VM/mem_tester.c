#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "memory_manager.h"

extern unsigned short int memory[];
extern unsigned short int page_tble_ptr;

word get_mem(unsigned short int virtual_addr)
{
    word addr = mmu(virtual_addr, 0);
    return memory[addr];
}

void set_mem(unsigned short int virtual_addr, unsigned short int data)
{
    word addr = mmu(virtual_addr, 1);
    // printf("\nCurrent value is:%d, setting to:%d", memory[addr], data);
    memory[addr] = data;
    // printf(", now value is:%d", memory[addr]);
}

int p1s = 0;
void p1()
{
    page_tble_ptr = 0 << 6;
    for (int i = 0; i < 32 * PAGE_SIZE; i++)
    {
        set_mem((word)i, (word)i);
    }

    for (int i = 0; i < 32 * PAGE_SIZE; i += 2)
    {
        assert(get_mem((word)i) == (word)i);
    }
    printf("p1-%d done\n", p1s++);
}

int p2s = 0;
void p2()
{
    page_tble_ptr = 1 << 6;
    for (int i = 0; i < 32 * PAGE_SIZE; i++)
    {
        set_mem((word)i, (word)i);
    }

    for (int i = 0; i < 32 * PAGE_SIZE; i += 2)
    {
        assert(get_mem((word)i) == (word)i);
    }
    printf("p2-%d done\n", p2s++);
}

int p3s = 0;
void p3()
{
    page_tble_ptr = 2 << 6;
    for (int i = 0; i < 32 * PAGE_SIZE; i++)
    {
        set_mem((word)i, (word)i);
    }

    for (int i = 0; i < 32 * PAGE_SIZE; i += 2)
    {
        assert(get_mem((word)i) == (word)i);
    }
    printf("p3-%d done\n", p3s++);
}

int main()
{
    init_memory();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    p1();
    p2();
    p3();
    return 0;
}