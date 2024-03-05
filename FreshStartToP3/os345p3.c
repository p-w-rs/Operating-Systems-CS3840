// os345p3.c - Jurassic Park 07/27/2020
// ***********************************************************************
// **   DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER ** DISCLAMER   **
// **                                                                   **
// ** The code given here is the basis for the CS345 projects.          **
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
#include "os345park.h"

// ***********************************************************************
// project 3 variables

// Jurassic Park
extern JPARK myPark;
extern Semaphore *parkMutex;            // protect park access
extern Semaphore *fillSeat[NUM_CARS];   // (signal) seat ready to fill
extern Semaphore *seatFilled[NUM_CARS]; // (wait) passenger seated
extern Semaphore *rideOver[NUM_CARS];   // (signal) ride over

Semaphore *roomInPark;
Semaphore *tickets;
Semaphore *needWorker;
Semaphore *needTicket;
Semaphore *ticketReady;
Semaphore *buyTicket;
Semaphore *needPassenger;
Semaphore *seated;
Semaphore *ticketBoothMutex;
Semaphore *driverMutex;
Semaphore *needDriver;

// ***********************************************************************
// project 3 functions and tasks
void CL3_project3(int, char **);
void CL3_dc(int, char **);

void visitorTask(int, char **);
void workerTask(int, char **);
void carTask(int, char **);

// ***********************************************************************
// ***********************************************************************
// project3 command
int P3_main(int argc, char *argv[])
{
    char buf[32];
    char *newArgv[2];
    parkMutex = NULL;

    roomInPark = createSemaphore("roomInPark", 1, MAX_IN_PARK);
    tickets = createSemaphore("tickets", 1, MAX_TICKETS);
    needWorker = createSemaphore("needWorker", 0, 0);
    needTicket = createSemaphore("needTicket", 0, 0);
    ticketReady = createSemaphore("ticketReady", 0, 0);
    buyTicket = createSemaphore("buyTicket", 0, 0);
    needPassenger = createSemaphore("needPassenger", 0, 0);
    seated = createSemaphore("seated", 0, 0);
    ticketBoothMutex = createSemaphore("ticketBoothMutex", 0, 1);
    driverMutex = createSemaphore("driverMutex", 0, 1);
    needDriver = createSemaphore("needTicket", 0, 0);

    // start park
    sprintf(buf, "jurassicPark");
    newArgv[0] = buf;
    createTask(buf,          // task name
               jurassicTask, // task
               MED_PRIORITY, // task priority
               1,            // task count
               newArgv);     // task argument

    // wait for park to get initialized...
    while (!parkMutex)
        SWAP;
    printf("\nStart Jurassic Park...");

    //?? create car, driver, and visitor tasks here
    for (int i = 0; i < NUM_VISITORS; i++)
    {
        sprintf(buf, "visitor %d", i);
        newArgv[0] = buf;
        createTask(buf,          // task name
                   visitorTask,  // task
                   MED_PRIORITY, // task priority
                   1,            // task count
                   newArgv);     // task argument
    }

    for (int i = 0; i < NUM_DRIVERS; i++)
    {
        char name[32];
        char id[32];
        sprintf(name, "worker %d", i);
        sprintf(id, "%d", i);
        newArgv[0] = name;
        newArgv[1] = id;
        createTask(buf,          // task name
                   workerTask,   // task
                   MED_PRIORITY, // task priority
                   2,            // task count
                   newArgv);     // task argument
    }

    for (int i = 0; i < NUM_CARS; i++)
    {
        char name[32];
        char id[32];
        sprintf(name, "car %d", i);
        sprintf(id, "%d", i);
        newArgv[0] = name;
        newArgv[1] = id;
        createTask(buf,          // task name
                   carTask,      // task
                   MED_PRIORITY, // task priority
                   2,            // task count
                   newArgv);     // task argument
    }

    return 0;
} // end project3

void rwait()
{
    int t = (rand() % 2) + 1;
    time_t start_time, cur_time;
    time(&start_time);
    time(&cur_time);
    while (cur_time - start_time < t)
    {
        SWAP;
        time(&cur_time);
    }
}

void carTask(int argc, char *argv[])
{
    int id = atoi(argv[1]);
    Semaphore *visitorSems[3];
    Semaphore *workerSem;
    do
    {
        for (int i = 0; i < 3; i++)
        {
            SWAP SEM_WAIT(fillSeat[id]);

            SWAP SEM_SIGNAL(needPassenger);
            // signal need a visitor
            // get visitor local semaphore into car local semaphore
            // use mail box such that visitorSems[i] -> visitors mySem
            // signal that the visitor is seated
            SWAP SEM_SIGNAL(seated);

            if (i == 2)
            {
                SWAP SEM_WAIT(driverMutex);
                {
                    SWAP SEM_SIGNAL(needDriver);
                    SWAP SEM_SIGNAL(needWorker);
                    // get the worker local semphore into car local semphore
                    // use mail box such that workerSem -> workers mySem
                    // pass carID to driver via another mailbox
                }
                SWAP SEM_SIGNAL(driverMutex);
            }

            SWAP SEM_SIGNAL(seatFilled[id]);
        }

        SWAP SEM_WAIT(rideOver[id]);
        for (int i = 0; i < 3; i++)
        {
            // signal each visitorSems[i]
            // this tells the visitor the ride is done
        }
        // signal workerSem
        // this tells the worker that he is done driving

    } while (1);
}

void workerTask(int argc, char *argv[])
{
    int id = atoi(argv[1]);
    do
    {
        SWAP SEM_WAIT(needWorker);
        if (semTryLock(needTicket))
        {
            SWAP SEM_WAIT(parkMutex);
            {
                SWAP myPark.drivers[id] = -1;
            }
            SWAP SEM_SIGNAL(parkMutex);

            rwait();
            SWAP SEM_SIGNAL(ticketReady);
            SWAP SEM_WAIT(buyTicket);

            SWAP SEM_WAIT(parkMutex);
            {
                SWAP myPark.drivers[id] = 0;
            }
            SWAP SEM_SIGNAL(parkMutex);
        }
        else if (semTryLock(needDriver))
        {
        }

    } while (1);
}

void visitorTask(int argc, char *argv[])
{
    Semaphore *mySem = createSemaphore(argv[0], 0, 0);
    rwait(); // should be replaces by delta clock signaling
    SWAP SEM_WAIT(parkMutex);
    {
        SWAP myPark.numOutsidePark++;
    }
    SWAP SEM_SIGNAL(parkMutex);

    rwait();
    SWAP SEM_WAIT(roomInPark);
    {
        SWAP SEM_WAIT(parkMutex);
        {
            SWAP myPark.numOutsidePark--;
            SWAP myPark.numInPark++;
            SWAP myPark.numInTicketLine++;
        }
        SWAP SEM_SIGNAL(parkMutex);

        rwait();
        SWAP SEM_WAIT(tickets);
        {
            SWAP SEM_WAIT(ticketBoothMutex);
            {
                SWAP SEM_SIGNAL(needTicket);
                SWAP SEM_SIGNAL(needWorker);
                SWAP SEM_WAIT(ticketReady);
                SWAP SEM_SIGNAL(buyTicket);
            }
            SWAP SEM_SIGNAL(ticketBoothMutex);
            SWAP SEM_WAIT(parkMutex);
            {
                SWAP myPark.numTicketsAvailable--;
                SWAP myPark.numInTicketLine--;
                SWAP myPark.numInMuseumLine++;
            }
            SWAP SEM_SIGNAL(parkMutex);
            rwait();

            SWAP SEM_WAIT(parkMutex);
            {
                SWAP myPark.numInMuseumLine--;
                SWAP myPark.numInCarLine++;
            }
            SWAP SEM_SIGNAL(parkMutex);

            SWAP SEM_WAIT(needPassenger);
            // Wait for a car to signal it needs a passenger
            // pass our local semaphore via global mailbox to cartask
            // wait till car says we are seated
            SWAP SEM_WAIT(seated);

            SWAP SEM_WAIT(parkMutex);
            {
                SWAP myPark.numInCarLine--;
                SWAP myPark.numInCars++;
                SWAP myPark.numTicketsAvailable++;
            }
            SWAP SEM_SIGNAL(parkMutex);
        }
        SWAP SEM_SIGNAL(tickets);

        SWAP SEM_WAIT(mySem);
        // wait for car ride to be over
        // go to the gift shop

        SWAP SEM_WAIT(parkMutex);
        {
            SWAP myPark.numInPark--;
            SWAP myPark.numInMuseumLine--;
            SWAP myPark.numExitedPark++;
        }
        SWAP SEM_SIGNAL(parkMutex);
    }
    SWAP SEM_SIGNAL(roomInPark);
}

// ***********************************************************************
// ***********************************************************************
// delta clock command
int P3_dc(int argc, char *argv[])
{
    printf("\nDelta Clock");
    // ?? Implement a routine to display the current delta clock contents
    printf("\nTo Be Implemented!");
    return 0;
} // end CL3_dc