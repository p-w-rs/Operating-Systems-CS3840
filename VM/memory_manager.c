#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "memory_manager.h"

// Amount of ram/main memory
unsigned short int memory[MAX_FRAME * FRAME_SIZE];
// Amount of disk/secondary memory
unsigned short int disk[MAX_DISK_PAGE * PAGE_SIZE];
// Will be set to the index beginning of the current tasks page table
unsigned short int page_tble_ptr;

// This is a binary array where a 1 represents that the corresponding frame in main memory is currently occupied
unsigned short int FBT[MAX_FRAME];
// This is a binary array where a 1 represents that the corresponding swappage in disk is currently occupied
unsigned short int DBT[MAX_DISK_PAGE];

unsigned short int swapClockPtr = PAGE_TABLE;

void init_memory()
{
    memset(&memory, 0, MAX_FRAME * FRAME_SIZE);
    memset(&disk, 0, MAX_DISK_PAGE * PAGE_SIZE);

    memset(&FBT, 0, MAX_FRAME);
    memset(&DBT, 0, MAX_DISK_PAGE);
}

void swap_in(word swappage, word frame)
{
    word disk_addr = swappage << 10;
    word mem_addr = frame << 10;
    for (word i = 0; i < 1024; i++)
        memory[mem_addr + i] = disk[disk_addr + i];
    DBT[swappage] = 0;
}

void swap_out(word frame, word swappage)
{
    word disk_addr = swappage << 10;
    word mem_addr = frame << 10;
    for (word i = 0; i < 1024; i++)
        disk[disk_addr + i] = memory[mem_addr + i];
    DBT[swappage] = 1;
}

word getFreeSwapPage()
{
    for (int page = 0; page < MAX_DISK_PAGE; page++)
    {
        if (DBT[page] == 0)
        {
            DBT[page] = 1;
            return (word)page;
        }
    }
    assert(1 && "We ran out of memory!!!");
    return MAX_DISK_PAGE;
}

word getFreeFrame()
{
    // Look for a frame that is not in system memory and that is free
    // If you find one set it to occupied in the FBT
    for (int frame = 0; frame < MAX_FRAME; frame++)
    {
        word mem_addr = ((word)frame) << 10;
        if (mem_addr <= SYS_MEM_END)
            continue;
        else if (FBT[frame] == 0)
        {
            FBT[frame] = 1;
            return (word)frame;
        }
    }

    // No free frames we need to swap out
    for (;;)
    {
        if (DEFINED(memory[swapClockPtr]))
        {
            // If a frame has been used recently by this process
            if (REFERENCED(memory[swapClockPtr]))
            {
                // Clear the recently used flag
                memory[swapClockPtr] = CLR_REFERENCED(memory[swapClockPtr]);
            }
            else
            {
                // The first frame whose recently used flag is clear
                word table_entry = memory[swapClockPtr];
                word nswapepage = getFreeSwapPage();
                word nframe = VALUE(table_entry);
                swap_out(nframe, nswapepage);
                table_entry = FLAGS(SET_PAGED(CLR_DIRTY(CLR_DEFINED(table_entry)))) + nswapepage;
                memory[swapClockPtr] = table_entry;
                return nframe;
            }
        }

        if (swapClockPtr++ == PAGE_TABLE_END)
            swapClockPtr = PAGE_TABLE;
    }
    return 0;
}

/*Table Entry Values*/
//           ___________________________________Frame defined
//          / __________________________________Dirty frame
//         / / _________________________________Referenced frame
//        / / / ________________________________Paged in memory
//       / / / /             ___________________Frame # or SwapPage # (0-63) (2^6 - 1)
//      / / / /             /                 __________
//     / / / /             /
//    / / / /             /
//   / / / /             /
//  / / / /             /
// F D R P - - - - - - f f f f f f
word mmu(word virtual_addr, word writeFlag)
{
    word physical_addr,          // The physical address to return
        npage, nframe, offset,   // The page number and offset in the virtual address
        table_addr, table_entry; // The table address and table entry

    npage = PAGE_NUM(virtual_addr);
    offset = OFFSET(virtual_addr);
    table_addr = npage + page_tble_ptr;
    table_entry = memory[table_addr];

    if (DEFINED(table_entry))
    {
        if (writeFlag)
            table_entry = SET_DIRTY(table_entry);
    }
    else
    {
        word nframe = getFreeFrame();
        if (PAGED(table_entry))
        {
            swap_in(VALUE(table_entry), nframe);
            table_entry = CLR_PAGED(table_entry);
        }
        else
        {
            memset(&memory[(nframe << 10)], 0, FRAME_SIZE);
        }
        table_entry = FLAGS(SET_DIRTY(SET_DEFINED(table_entry))) + nframe;
    }

    memory[table_addr] = table_entry;
    physical_addr = (VALUE(table_entry) << 10) + offset;

    /*printf("\nva=%d::flg=%d::pa=%d::page#=%d::offset=%d::table_addr=%d::F(%d)D(%d)R(%d)P(%d)::value=%d::frame#=%d\n",
           virtual_addr, writeFlag, physical_addr, npage, offset, table_addr, DEFINED(table_entry) > 0,
           DIRTY(table_entry) > 0, REFERENCED(table_entry) > 0, PAGED(table_entry) > 0, VALUE(table_entry), nframe);*/

    return physical_addr;
}