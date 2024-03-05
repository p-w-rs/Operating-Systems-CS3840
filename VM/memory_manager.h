#ifndef __mm_h__
#define __mm_h__
/*
max memory is 131072 bytes (around 131 MB)
    - We can index up to 65536 addresses with 16 bits
    - Each adrress is 2 bytes (a word)
    - 65536*2 = 131072 bytes
    - We may have less ram than this which will require us to swap to disk
*/
typedef unsigned short int word;
#define WORD_SIZE 2        // A word size is 2 bytes
#define PAGE_SIZE 1024     // We have 10 bits for our offset, 2^10 = 1024 (approx 16 MB)
#define FRAME_SIZE 1024    // We have 10 bits for our offset, 2^10 = 1024 (approx 16 MB)
#define MAX_PAGES 64       // We have 6 bits for our page #, 2^6 = 64
#define MAX_FRAMES 64      // We have 6 bits for our table entry value 2^6 = 64
#define MAX_MEM_ADDR 65535 // 0 -> 2^16-1 since a word is 2 bytes OR (64 frames)x(1024 words per frame)-1

/*
os system memory takes up 32768 bytes (approx 32 MB)
    - address 16383 is the 16384 word of memory
    - 16383*2 = 32768
    - 16384/(1024 frame size) = 16 frames for system memory

our computer has 98304 bytes of ram and 131072 bytes of disk
    - 48 frames total, 16 for system and 32 for processes
    - we have space in our page system memory for 20 processes
        - our page tabel starts at address 0 and ends at address 1279 (1280 addresses)
        - each process wants to have up to 64 pages and needs one table address per page
        - (1280 addresses)/(64 addresses per process) = 20 processes
*/
#define SYS_MEM 0         // The os memeory starts at physical address 0
#define SYS_MEM_END 16383 // The os memory ends with physical address 16383, the 17th frame begins at addr 16384
#define PAGE_TABLE 0
#define PAGE_TABLE_END 1279
#define MAX_FRAME 48
#define MAX_DISK_PAGE 64

/* Usefull bit operators */
#define PAGE_NUM(w) (w >> 10)  // get the first 6 bits of a virtual address
#define OFFSET(w) (w & 0x03FF) // keep the last 10 bits of a virtual address 0x03FF => 0000001111111111
#define VALUE(w) (w & 0x003F)  // keep the last 6 bits of a page table entry 0x03FF => 0000000000111111
#define FLAGS(w) (w & 0xFFC0)  // keep the first 10 bits of a page table entry 0xFFC0 => 1111111111000000

#define DEFINED(w) ((1 << 15) & w)         // get the first bit of a page table entry
#define DIRTY(w) ((1 << 14) & w)           // get the second bit of a page table entry
#define REFERENCED(w) ((1 << 13) & w)      // get the third bit of a page table entry
#define PAGED(w) ((1 << 12) & w)           // get the fourth bit of a page table entry
#define SET_DEFINED(w) ((1 << 15) | w)     // set the first bit of a page table entry
#define SET_DIRTY(w) ((1 << 14) | w)       // set the second bit of a page table entry
#define SET_REFERENCED(w) ((1 << 13) | w)  // set the third bit of a page table entry
#define SET_PAGED(w) ((1 << 12) | w)       // set the fourth bit of a page table entry
#define CLR_DEFINED(w) (~(1 << 15) & w)    // clear the first bit of a page table entry
#define CLR_DIRTY(w) (~(1 << 14) & w)      // clear the second bit of a page table entry
#define CLR_REFERENCED(w) (~(1 << 13) & w) // clear the third bit of a page table entry
#define CLR_PAGED(w) (~(1 << 12) & w)      // clear the fourth bit of a page table entry

void init_memory();
void swap_in(word swappage, word frame);
void swap_out(word frame, word swappage);
word getFreeSwapPage();
word getFreeFrame();
word mmu(word virtual_addr, word writeFlag);

#endif