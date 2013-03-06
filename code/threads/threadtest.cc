// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "dllist.h"
#include "elevator.h"

// testnum is set in main.cc
int testnum = 2;

// T and N are used in lab1
int T = 1;
int N = 1;

DLList *list;
Building *building;
int numFloors = 10;
int numElevators = 1;
int numRiders = 1;


extern void InsertNItemsToDLList(DLList *list, int N, int);
extern void RemoveNItemsFromDLList(DLList *list, int N, int);

void rider(int id, int srcFloor, int dstFloor);
//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
    	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

void
DLListTestThread(int which)
{
	// TODO
	printf("*** thread %d starts\n", which);
	InsertNItemsToDLList(list, N, which);
	currentThread->Yield();
	RemoveNItemsFromDLList(list, N, which);
	currentThread->Yield();
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

void
ThreadTest2()
{
	DEBUG('t', "Entering ThreadTest2\n");
	DEBUG('t', "T: %d\n", T);
	DEBUG('t', "N: %d\n", N);
	list = new DLList("test list");
	for (int i = 1; i < T; i++) {
		Thread *t = new Thread("forked thread");
		t->Fork(DLListTestThread, i);
	}
	DLListTestThread(0);
}

void RiderThread(int which)
{
	//building = new Building("building", 10, 1);
	sysAlarm->Pause(1);
	int from, to;
	while (1) {
		from = Random() % numFloors;
		to = Random() % numFloors;
		printf("%d %d\n", from, to);
		// from = 1;
		// to = 7;
		if (to == from) sysAlarm->Pause(1);
		else rider(which, from, to);
		// from = 7;
		// to = 2;
		// rider(which, from, to);
	}
	//building->GetElevator(0)->Run();
}

void TestElevator()
{
	Thread *t = new Thread("rider thread");
	building = new Building("building", 10, 1);
	for (int i = 0; i < numRiders; i++)
		t->Fork(RiderThread, i);
	building->GetElevator(0)->Run();
}

void AlarmThread(int which)
{
	printf("Begin to sleep 1 second\n");
	sysAlarm->Pause(1);
	printf("Thread wake up\n");
}

void TestAlarm()
{
	Thread *t = new Thread("Test Alarm");
	t->Fork(AlarmThread, 0);
}


//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
	switch (testnum) {
	case 1:
		ThreadTest1();
		break;
	case 2:
		ThreadTest2();
		break;
	case 3:
		TestElevator();
		break;
	case 4:
		TestAlarm();
		break;
	default:
		printf("No test specified.\n");
		break;
	}
}

void rider(int id, int srcFloor, int dstFloor) {
	Elevator *e;

	if (srcFloor == dstFloor)
	   return;

	DEBUG('a',"Rider %d travelling from %d to %d\n",id,srcFloor,dstFloor);
	do {
	   if (srcFloor < dstFloor) {
		  DEBUG('a', "Rider %d CallUp(%d)\n", id, srcFloor);
		  building->CallUp(srcFloor);
		  DEBUG('a', "Rider %d AwaitUp(%d)\n", id, srcFloor);
		  e = building->AwaitUp(srcFloor);
	   } else {
		  DEBUG('a', "Rider %d CallDown(%d)\n", id, srcFloor);
		  building->CallDown(srcFloor);
		  DEBUG('a', "Rider %d AwaitDown(%d)\n", id, srcFloor);
		  e = building->AwaitDown(srcFloor);
	   }
	   DEBUG('a', "Rider %d Enter()\n", id);
	} while (!e->Enter()); // elevator might be full!

	DEBUG('a', "Rider %d RequestFloor(%d)\n", id, dstFloor);
	e->RequestFloor(dstFloor); // doesn't return until arrival
	DEBUG('a', "Rider %d Exit()\n", id);
	e->Exit();
	DEBUG('a', "Rider %d finished\n", id);
}
