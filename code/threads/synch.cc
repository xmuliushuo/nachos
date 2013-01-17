// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
    	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
#ifdef SLEEP
// Implement your locks and condition variables using the sleep/wakeup primitives (the
// Thread::Sleep and Scheduler::ReadyToRun primitives). It will be necessary to disable
// interrupts temporarily at strategic points, to eliminate the possibility of an ill-timed interrupt or
// involuntary context switch. In particular, Thread::Sleep requires you to disable interrupts before
// you call it. However, you may lose points for holding interrupts disabled when it is not necessary
// to do so. Disabling interrupts is a blunt instrument and should be avoided unless necessary.
Lock::Lock(const char* debugName):name(debugName), thread(NULL), value(FREE), queue(new List())
{ }

Lock::~Lock()
{
	delete queue;
}

void Lock::Acquire()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    if (value == BUSY) {
    	queue->Append((void *)currentThread);
    	currentThread->Sleep();
    }
    value = BUSY;
    thread = currentThread;
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

bool Lock::isHeldByCurrentThread()
{ 
	return (thread == currentThread); 
}

void Lock::Release()
{
	Thread *nextThread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    nextThread = (Thread *)queue->Remove();
    if (nextThread != NULL)	   // make thread ready, consuming the V immediately
    	scheduler->ReadyToRun(nextThread);
    value = FREE;
    thread = NULL;
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

Condition::Condition(const char* debugName):name(debugName), queue(new List())
{ }

Condition::~Condition()
{
	delete queue;
}

void Condition::Wait(Lock* conditionLock)
{
	ASSERT(conditionLock->isHeldByCurrentThread());
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	conditionLock->Release();
	queue->Append((void *)currentThread);
	currentThread->Sleep();
	(void) interrupt->SetLevel(oldLevel);
	conditionLock->Acquire();
}

void Condition::Signal(Lock* conditionLock)
{
	Thread *nextThread;
	ASSERT(conditionLock->isHeldByCurrentThread());
	if (!queue->IsEmpty()) {
		nextThread = (Thread *)queue->Remove();
		scheduler->ReadyToRun(nextThread);
	}
}

void Condition::Broadcast(Lock* conditionLock)
{
	Thread *nextThread;
	ASSERT(conditionLock->isHeldByCurrentThread());
	while (!queue->IsEmpty()) {
		nextThread = (Thread *)queue->Remove();
		scheduler->ReadyToRun(nextThread);
	}
	DEBUG('a', "broadcast finished\n");
}


#else
// Implement your locks and condition variables using semaphores as the only synchronization
// primitive. This time it is not necessary (or permitted) to disable interrupts in your code: the
// semaphore primitives disable interrupts as necessary to implement the semaphore abstraction,
// which you now have at your disposal as a sufficient "toehold" for synchronization.
// Warning : this part of the assignment seems easy but it is actually the most subtle and difficult.
// In particular, your solution for condition variables should guarantee that a Signal cannot affect a
// subsequent Wait.
Lock::Lock(const char* debugName):name(debugName),
		thread(NULL),
		sem(new Semaphore(debugName, 1))
{ }

Lock::~Lock()
{
	delete sem;
}

void Lock::Acquire()
{
	sem->P();
    thread = currentThread;
}

bool Lock::isHeldByCurrentThread()
{ 
	return (thread == currentThread); 
}

void Lock::Release()
{
	thread = NULL;
	sem->V();
}

Condition::Condition(const char* debugName):name(debugName),
		sem(new Semaphore(debugName, 0)),
{ }

Condition::~Condition()
{
	delete sem;
}

void Condition::Wait(Lock* conditionLock)
{
	ASSERT(conditionLock->isHeldByCurrentThread());
	count++;
	conditionLock->Release();
	sem->P();
	conditionLock->Acquire();
}

void Condition::Signal(Lock* conditionLock)
{
	ASSERT(conditionLock->isHeldByCurrentThread());
	if (count > 0) {
		count--;
		sem->V();
	}
}

void Condition::Broadcast(Lock* conditionLock)
{
	ASSERT(conditionLock->isHeldByCurrentThread());
	while (count > 0) {
		count--;
		sem->V();
	}
}
#endif
