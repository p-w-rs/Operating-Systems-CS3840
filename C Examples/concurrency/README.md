# Concurrency examples

CS3841 examples using concurrency techniques

Examples with file names con\*.c show the progression of problems that can arise by trying to perform mutual exclusion ourselves without OS support.

Other examples show in sem\*.c and mutex\*.c show how the OS can help out by providing concurrency mechanisms.

* race.c - Shows the race condition that can happen then threads try to modify multiple global values at the same time

* con1.c - Concurrency using "taking turns"  
Provides mutual exclusion but does not work for more than 2 processes  
Also does not work if a process does not want to access the critical section

* con2.c - Concurrency using flag array - version 1  
Allows threads to specify desire for critical secgion  
Due to race conditions with the flag array, it cannot guarentee mutual exclusion

* con3.c - Concurrency using flag array - version 2  
Move setting of the flag before the wait  
Allows threads to specify desire for critical secgion  
Due to race conditions with the flag array, it is prone to deadlock if both threads set the flag at the same time

* con4.c - Concurrency using flag array - version 3  
Sets the critical section flag before the wait  
Allows threads to specify desire for critical secgion  
Adds deadlock detection and relase of the flag if both threads have their flag set  
Prone to livelock since threads might be constantly releasing their flag

* con\_dekkers.c - Concurrency algorithm proposed by Theodorus Jozef Dekker  
Combines the deadlock detection of cons4 with the taking turns algorithm in cons1  
Ensures mutual exclusion along with deadlock/livelock prevention, however needs to know how many participants there are  
Also does not work on modern CPUs that perform out-of-order execution

* sem1.c - Uses semaphores to control access to the critical section

* sem2.c - Shows that semaphores are NOT locks, but a signalling mechanism.  Multiple posts to a semaphore incremenet the count regardless of the current value

* sem\_list1.c - Shows what can happen as a result of uncontrolled access to a singly linked list

* sem\_list2.c - Uses a semaphore as a way to control access to a critical section for a singly linked list

* mutex1.c - Uses a mutex lock to control access to a critical section that increments two global variables

* mutex2.c - Shows the undefined behavior that can happen when a thread attempts to unlock a mutex that's already unlocked

* mutex3.c - Shows the deadlock behavior that can happen when a thread attempts to lock a mutex that it has already locked

* prod\_cons1.c - Shows the producer and consumer problem  
When access to a limited shared space is not controlled there are chances where a producer thread might attempt to write when the space is full and a consumer might attempt to read when the space is empty

* prod\_cons2.c - Shows the producer and consumer problem  
Access is controlled using semaphores  
One semaphore counts the number of filled spots  
One semaphore counts the number of empty spots  
Producers wait for at least one free spot before writing  
Consusmers wait for at least one filled spot before reading
