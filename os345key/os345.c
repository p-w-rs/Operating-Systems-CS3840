// os345.c - OS Kernel	06/21/2020
// ***********************************************************************
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// **                                                                   **
// ** The code given here is the basis for the BYU CS345 projects.      **
// ** It comes "as is" and "unwarranted."  As such, when you use part   **
// ** or all of the code, it becomes "yours" and you are responsible to **
// ** understand any algorithm or method presented.  Likewise, any      **
// ** errors or problems become your responsibility to fix.             **
// **                                                                   **
// ** NOTES:                                                            **
// ** -Comments beginning with "// ??" may require some implementation. **
// ** -Tab stops are set at every 3 spaces.                             **
// ** -The function API's in "OS345.h" should not be altered.           **
// **                                                                   **
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// ***********************************************************************
#include <assert.h>
#include <ctype.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "os345.h"
#include "os345config.h"
#include "os345fat.h"
#include "os345lc3.h"
#include "os345signals.h"

#pragma clang diagnostic ignored "-Wparentheses"

// **********************************************************************
//	local prototypes
//
void pollInterrupts(void);
static int scheduler(void);
static int dispatcher(void);

// static void keyboard_isr(void);
// static void timer_isr(void);

int sysKillTask(int taskId);
static int initOS(void);

// **********************************************************************
// **********************************************************************
// global semaphores

Semaphore *semaphoreList; // linked list of active semaphores

Semaphore *keyboard;      // keyboard semaphore
Semaphore *charReady;     // character has been entered
Semaphore *inBufferReady; // input buffer ready semaphore

Semaphore *tics1sec;    // 1 second semaphore
Semaphore *tics10sec;   // 10 second semaphore
Semaphore *tics10thsec; // 1/10 second semaphore

// **********************************************************************
// **********************************************************************
// global system variables

TCB tcb[MAX_TASKS];             // task control block
Semaphore *taskSems[MAX_TASKS]; // task semaphore
jmp_buf k_context;              // context of kernel stack
jmp_buf reset_context;          // context of kernel stack
volatile void *temp;            // temp pointer used in dispatcher

int scheduler_mode; // scheduler mode
int min_pct;
int superMode;                 // system mode
int curTask;                   // current task #
long swapCount;                // number of re-schedule cycles
char inChar;                   // last entered character
int charFlag;                  // 0 => buffered input
int inBufIndx;                 // input pointer into input buffer
char inBuffer[INBUF_SIZE + 1]; // character input buffer
// Message messages[NUM_MESSAGES];		// process message buffers

int pollClock;     // current clock()
int lastPollClock; // last pollClock
bool diskMounted;  // disk has been mounted

time_t oldTime1;  // old 1sec time
time_t oldTime10; // old 1sec time
clock_t myClkTime;
clock_t myOldClkTime;

PQ readyQ;
PQ zeroQ;

int emptyQ(PQ *q)
{
    if (q->head)
        return 0;
    return 1;
}

int enq(PQ *q, int tid)
{
    Node *task = malloc(sizeof(Node));
    task->tid = tid;
    task->priority = tcb[tid].priority;
    task->next = NULL;

    Node *prev = NULL;
    Node *cur = q->head;
    while (cur && task->priority <= cur->priority)
    {
        prev = cur;
        cur = cur->next;
    }

    if (prev)
        prev->next = task;
    else
        q->head = task;
    task->next = cur;

    return 0;
}

int deq(PQ *q)
{
    int tid = -1;
    if (q->head)
    {
        tid = q->head->tid;
        Node *tmp = q->head;
        q->head = q->head->next;
        free(tmp);
    }
    return tid;
}

int del_task(PQ *q, int tid)
{
    Node *prev = NULL;
    Node *cur = q->head;
    while (cur)
    {
        if (cur->tid == tid)
            break;
        prev = cur;
        cur = cur->next;
    }
    if (!cur)
        return 0;

    if (prev)
        prev->next = cur->next;
    else
        q->head = q->head->next;

    free(cur);
    return 0;
}

void printq(PQ *q)
{
    Node *cur = q->head;
    while (cur)
    {
        int i = cur->tid;
        printf("\n%4d/%-4d%20s%4d%4d  ", i, tcb[i].parent, tcb[i].name, tcb[i].priority, tcb[i].ctime);
        if (tcb[i].signal & mySIGSTOP)
            printf("Paused");
        else if (tcb[i].state == S_NEW)
            printf("New");
        else if (tcb[i].state == S_READY)
            printf("Ready");
        else if (tcb[i].state == S_RUNNING)
            printf("Running");
        else if (tcb[i].state == S_BLOCKED)
            printf("Blocked    %s", tcb[i].event->name);
        else if (tcb[i].state == S_EXIT)
            printf("Exiting");
        cur = cur->next;
    }
    printf("\n");
}

// **********************************************************************
// **********************************************************************
// OS startup
//
// 1. Init OS
// 2. Define reset longjmp vector
// 3. Define global system semaphores
// 4. Create CLI task
// 5. Enter scheduling/idle loop
//
int main(int argc, char *argv[])
{
    // save context for restart (a system reset would return here...)
    int resetCode = setjmp(reset_context);
    superMode = TRUE; // supervisor mode

    switch (resetCode)
    {
    case POWER_DOWN_QUIT: // quit
        powerDown(0);
        printf("\nGoodbye!!");
        return 0;

    case POWER_DOWN_RESTART: // restart
        powerDown(resetCode);
        printf("\nRestarting system...\n");

    case POWER_UP: // startup
        break;

    default:
        printf("\nShutting down due to error %d", resetCode);
        powerDown(resetCode);
        return resetCode;
    }

    // output header message
    printf("%s", STARTUP_MSG);

    // initalize OS
    if (resetCode = initOS())
        return resetCode;

    // create global/system semaphores here
    //?? vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

    charReady = createSemaphore("charReady", BINARY, 0);
    inBufferReady = createSemaphore("inBufferReady", BINARY, 0);
    keyboard = createSemaphore("keyboard", BINARY, 1);
    tics1sec = createSemaphore("tics1sec", BINARY, 0);
    tics10sec = createSemaphore("tics10sec", BINARY, 0);
    tics10thsec = createSemaphore("tics10thsec", BINARY, 0);

    //?? ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

    // schedule CLI task
    createTask("myShell",    // task name
               P1_main,      // task
               MED_PRIORITY, // task priority
               argc,         // task arg count
               argv);        // task argument pointers

    // HERE WE GO................

    // Scheduling loop
    // 1. Check for asynchronous events (character inputs, timers, etc.)
    // 2. Choose a ready task to schedule
    // 3. Dispatch task
    // 4. Loop (forever!)
    // printf("STARTING\n");
    // printq(&readyQ);

    while (1) // scheduling loop
    {
        // check for character / timer interrupts
        pollInterrupts();

        // schedule highest priority ready task
        if ((curTask = scheduler()) < 0)
            continue;

        // dispatch curTask, quit OS if negative return
        if (dispatcher() < 0)
            break;
    } // end of scheduling loop

    // exit os
    longjmp(reset_context, POWER_DOWN_QUIT);
    return 0;
} // end main

void compute_pct(int tid, int group_pct)
{

    if (tcb[tid].n_children)
    {
        int pct = group_pct / (tcb[tid].n_children + 1);
        int rem = group_pct % (tcb[tid].n_children + 1);
        tcb[tid].ctime = pct + rem;
        for (int i = 0; i < tcb[tid].n_children; i++)
        {
            compute_pct(tcb[tid].children[i], pct);
        }
    }
    else
    {
        tcb[tid].ctime = group_pct;
    }
}

void reschedule()
{
    int tid = deq(&zeroQ);
    while (tid != -1)
    {
        if (tcb[tid].name)
            enq(&readyQ, tid);
        tid = deq(&zeroQ);
    }
}

// **********************************************************************
// **********************************************************************
// scheduler
//
static int scheduler()
{
    int nextTask = -1;
    if (scheduler_mode == 0)
    {

        if (curTask != -1 && tcb[curTask].name && tcb[curTask].state != S_BLOCKED)
            enq(&readyQ, curTask);
        nextTask = deq(&readyQ);
        if (nextTask == -1)
            return -1;

        if (tcb[nextTask].signal & mySIGSTOP)
        {
            enq(&readyQ, nextTask);
            return -1;
        }
    }
    else
    {
        if (curTask != -1 && tcb[curTask].name && tcb[curTask].ctime > 0 && tcb[curTask].state != S_BLOCKED)
            enq(&readyQ, curTask);

        if (emptyQ(&readyQ) && !emptyQ(&zeroQ))
        {
            compute_pct(0, 1000);
            reschedule();
        }
        nextTask = deq(&readyQ);
        if (nextTask == -1)
            return -1;

        if (tcb[nextTask].signal & mySIGSTOP)
        {
            enq(&readyQ, nextTask);
            return -1;
        }

        if (--tcb[nextTask].ctime <= 0)
        {
            enq(&zeroQ, nextTask);
        }
    }

    return nextTask;
} // end scheduler

// **********************************************************************
// **********************************************************************
// dispatch curTask
//
static int dispatcher()
{
    int result;

    // schedule task
    switch (tcb[curTask].state)
    {
    case S_NEW: {
        // new task
        printf("\nNew Task[%d] %s", curTask, tcb[curTask].name);
        tcb[curTask].state = S_RUNNING; // set task to run state

        // save kernel context for task SWAP's
        if (setjmp(k_context))
        {
            superMode = TRUE; // supervisor mode
            break;            // context switch to next task
        }

        // move to new task stack (leave room for return value/address)
        temp = (int *)tcb[curTask].stack + (STACK_SIZE - 8);
        SET_STACK(temp);
        superMode = FALSE; // user mode

        // begin execution of new task, pass argc, argv
        result = (*tcb[curTask].task)(tcb[curTask].argc, tcb[curTask].argv);

        // task has completed
        if (result)
            printf("\nTask[%d] returned error %d", curTask, result);
        else
            printf("\nTask[%d] returned %d", curTask, result);
        tcb[curTask].state = S_EXIT; // set task to exit state

        // return to kernal mode
        longjmp(k_context, 1); // return to kernel
    }

    case S_READY: {
        tcb[curTask].state = S_RUNNING; // set task to run
    }

    case S_RUNNING: {
        if (setjmp(k_context))
        {
            // SWAP executed in task
            superMode = TRUE; // supervisor mode
            break;            // return from task
        }
        if (signals())
            break;
        longjmp(tcb[curTask].context, 3); // restore task context
    }

    case S_BLOCKED: {
        break;
    }

    case S_EXIT: {
        if (curTask == 0)
            return -1; // if CLI, then quit scheduler
        // release resources and kill task
        sysKillTask(curTask); // kill current task
        break;
    }

    default: {
        printf("Unknown Task[%d] State", curTask);
        longjmp(reset_context, POWER_DOWN_ERROR);
    }
    }
    return 0;
} // end dispatcher

// **********************************************************************
// **********************************************************************
// Do a context switch to next task.

// 1. If scheduling task, return (setjmp returns non-zero value)
// 2. Else, save current task context (setjmp returns zero value)
// 3. Set current task state to READY
// 4. Enter kernel mode (longjmp to k_context)

void swapTask()
{
    assert("SWAP Error" && !superMode); // assert user mode

    // increment swap cycle counter
    swapCount++;

    // either save current task context or schedule task (return)
    if (setjmp(tcb[curTask].context))
    {
        superMode = FALSE; // user mode
        return;
    }

    // context switch - move task state to ready
    if (tcb[curTask].state == S_RUNNING)
        tcb[curTask].state = S_READY;

    // move to kernel mode (reschedule)
    longjmp(k_context, 2);
} // end swapTask

// **********************************************************************
// **********************************************************************
// system utility functions
// **********************************************************************
// **********************************************************************

// **********************************************************************
// **********************************************************************
// initialize operating system
static int initOS()
{
    int i;

    // make any system adjustments (for unblocking keyboard inputs)
    INIT_OS

    // reset system variables
    curTask = -1;       // current task #
    swapCount = 0;      // number of scheduler cycles
    scheduler_mode = 0; // default scheduler
    min_pct = 1000;
    inChar = 0;        // last entered character
    charFlag = 0;      // 0 => buffered input
    inBufIndx = 0;     // input pointer into input buffer
    semaphoreList = 0; // linked list of active semaphores
    diskMounted = 0;   // disk has been mounted

    // malloc ready queue
    readyQ.head = NULL;

    // capture current time
    lastPollClock = clock(); // last pollClock
    time(&oldTime1);
    time(&oldTime10);

    // init system tcb's
    for (i = 0; i < MAX_TASKS; i++)
    {
        tcb[i].name = NULL; // tcb
        taskSems[i] = NULL; // task semaphore
    }

    // init tcb
    for (i = 0; i < MAX_TASKS; i++)
    {
        tcb[i].name = NULL;
    }

    // initialize lc-3 memory
    initLC3Memory(LC3_MEM_FRAME, 0xF800 >> 6);

    // ?? initialize all execution queues

    return 0;
} // end initOS

// **********************************************************************
// **********************************************************************
// Causes the system to shut down. Use this for critical errors
void powerDown(int code)
{
    int i;
    printf("\nPowerDown Code %d", code);

    // release all system resources.
    printf("\nRecovering Task Resources...");

    // kill all tasks
    for (i = MAX_TASKS - 1; i >= 0; i--)
        if (tcb[i].name)
            sysKillTask(i);

    // delete all semaphores
    while (semaphoreList)
        deleteSemaphore(&semaphoreList);

    // free ready queue
    int tid = deq(&readyQ);
    while (tid >= 0)
        tid = deq(&readyQ);

    // ?? release any other system resources
    // ?? deltaclock (project 3)

    RESTORE_OS
    return;
} // end powerDown
