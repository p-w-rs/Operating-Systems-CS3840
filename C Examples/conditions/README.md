# Producer/Consumer with condition variable examples

CS3841 examples for producer/consumer that motivate the need for condition variables

* prod\_con1.c - Producer and consumer with unlimited sized buffer  
Works with a single semaphore to count full slots when only a single producer and consumer is used

* prod\_con2.c - Producer and consumer with unlimited sized buffer  
Shows a problem that can arise when multiple consumers are consuming elements from the shared space at the same time  
With no controlled access between consumers, the number of elements in the shared space become incorrect

* prod\_con3.c - Producer and consumer with unlimited sized buffer  
Fixes the problem when multiple producers and consumers modify a shared space by using a mutex lock to prevent threads from modifying the space at the same time

* prod\_con4.c - Producer and consumer with unlimited sized buffer  
Shows a problem that can arise when a consumer wants to consume multiple items from the shared space.  Since a mutex lock is used to lock out other threads, it is possible for a consumer to remove elements from an empty shared space

* prod\_con5.c - Producer and consumer with unlimited sized buffer  
An attempt to fix the problem of a consumer removing from an empty shared space when more than one item is to be consumed by waiting in a while loop until enough elements are in the space  
Unfortunately causes deadlock since the consumer is holding the the mutex lock while waiting for more items

* prod\_con6.c - Producer and consumer with unlimited sized buffer  
An attempt to fix the problem of a consumer removing from an empty shared space when more than one item is to be consumed by waiting in a while loop until enough elements are in the space.  
Attemps to fix the deadlock provlem in prod\_cons5.c by releasing the lock  
Unfortunately the consumer releasing the lock in the while loop causes undefined behavior

* prod\_con7.c - Producer and consumer with unlimited sized buffer  
An attempt to fix the problem of a consumer removing from an empty shared space when more than one item is to be consumed by releasing the lock on the shared space and then waiting on a semaphore until there are more items.  
Unfortunately, there is know way for the consumer to get back into the "head of the line" after more items are produced.  Another consumer can skip ahead

* prod\_con\_cond1.c - Producer and consumer with unlimited sized buffer  
Uses a condition variable to ensure proper order of the consumers when they wait for more items to be consumed  
Unfortunately, using a single condition variable without additional mutual exculsion does not maintain the consumers order in line

* prod\_con\_cond2.c - Producer and consumer with unlimited sized buffer  
Uses a condition variable to ensure proper order of the consumers when they wait for more items to be consumed  
Also uses a mutex to control the ordering of consumers  
This example provides correct behavior when there are multiple producers and multiple consumers and each consumer orders multiple items.
