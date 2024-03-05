# pthread examples

CS3841 examples using the pthread library to create POSIX threads

* pthreads1.c - Shows how to create and wait for single thread (beyond the main thread)

* pthreads2.c - Shows how to create and wait for multple threads (beyond the main thread)  
Threads prints out its thread ID and process ID  
Shows that threads share the same process ID but all have their own thread ID

* pthreads3.c - Shows how to pass parameter values to thread routines

* pthreads4.c - Shows how to pass parameter values to thread routines  
The threads dereference the pointer passed to the thread routine to demonstrate that memory is shared between all threads

* pthreads5.c - Shows a problem that can happen with parameters passed to thread routines due to the fact that the parameter must be a pointer  
The referenced location must be valid for the entire life of the thread  
NOTE: Context switches between threads can happen when unexpected

* pthreads6.c - Shows the problems that can happen due to the fact that thread routine parameters are pointers to memory locations  
Threads can easily affect other threads memory  
NOTE: Context switches between threads can happen when unexpected

* pthreads\_race.c - Shows a "race condition" when multiple threads try to race to modify a global value  
Due to unpredictable behavior in context switching the value of global\_value does not result in what is expected

* pthreads\_race\_atomic.c - Uses an atomic add instruction to prevent the race condition that can happen when multiple threads modify a global value at the same time
