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

// testnum is set in main.cc
int testnum = 2;

// T and N are used in lab1
int T = 1;
int N = 1;

DLList *list;

extern void InsertNItemsToDLList(DLList *list, int N);
extern void RemoveNItemsFromDLList(DLList *list, int N);

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
	InsertNItemsToDLList(list, N);
	currentThread->Yield();
	RemoveNItemsFromDLList(list, N);
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
	list = new DLList();
	for (int i = 1; i < T; i++) {
		Thread *t = new Thread("forked thread");
		t->Fork(DLListTestThread, i);
	}
	DLListTestThread(0);
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
    default:
		printf("No test specified.\n");
		break;
    }
}

