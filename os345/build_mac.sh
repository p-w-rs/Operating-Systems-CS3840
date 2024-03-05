clang -std=c18 -g -arch x86_64 -Ofast -Wall -Wextra\
    -o shell\
    os345.c os345fat.c os345lc3.c os345mmu.c os345park.c os345interrupts.c os345semaphores.c os345signals.c os345tasks.c\
    os345p1.c os345p2.c os345p3.c os345p4.c os345p5.c os345p6.c