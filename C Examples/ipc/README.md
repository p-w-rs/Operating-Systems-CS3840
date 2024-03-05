# Inter-process communication (IPC) examples

CS3841 examples using IPC mechanisms to communicate between processes

* ipcfs1.c - Using the file system for inter-process communication  
Shows a problem with the fact that the file position pointer is shared between parent and childUsing fork to create two processes

* ipcfs2.c - Using the file system for inter-process communication  
Fixes the file position pointer problem in ipcfs1.c by using lseek to reset the pointer to the beginning  
Shows the frustration with using the file system for IPC by needing to move the file position pointer around

* ipcpipe1.c - Using a pipe to communicate data from a child process to a parent process

* ipcpipe2.c - Pipes are unidirectional and require two pipes to send data in two directions

* ipcmsg1.c - Using a message queue to send data from a parent process to a child process  
NOTE: message queues are persistent

* ipcmsg2.c - ses message queues to send multiple messages between two processes  
Shows what can happen with blocking queues when the queue gets full  
NOTE: message queues are persistent

* signal1.c - Shows how to set up a signal handler for the interrupt (CTRL+C) signal

* signal2.c - Shows how to set up a signal handler for the segmentation fault signal  
Unfortunately does not fix the segmentation fault and essentially loops forever

* ipcsignal.c - Shows how to use the kill system call to send a signal from one process to another

* ipcshm1.c - Uses shared memory to send data from a child process to a parent process  
NOTE: named shared memory segments are persistent

* ipcshm2.c - Uses shared memory to send data from a child process to a parent process  
Uses shm\_unlink to remove the shared memory when done  
NOTE: named shared memory segments are persistent
