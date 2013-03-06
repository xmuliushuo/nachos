/*
 * eventbarrier.cc
 *
 *  Created on: 2013-1-14
 *      Author: liushuo
 */

#include "eventbarrier.h"

EventBarrier::EventBarrier():
	m_status(UNSIGNALED),
	m_barrier(new Condition("EventBarrier::m_barrier")),
	m_lock(new Lock("EventBarrier::m_lock")),
	m_complete(new Condition("EventBarrier::m_complete"))
{}

EventBarrier::~EventBarrier()
{
	delete m_barrier;
	delete m_complete;
	delete m_lock;
}

void EventBarrier::Signal()
{
	DEBUG('t', "%s signal\n", currentThread->getName());
	m_lock->Acquire();
	m_status = SIGNALED;
	m_barrier->Broadcast(m_lock);
	m_complete->Wait(m_lock);
	m_status = UNSIGNALED;
	m_lock->Release();
	DEBUG('t', "%s signal return\n", currentThread->getName());
}

void EventBarrier::Wait()
{
	DEBUG('t', "%s wait\n", currentThread->getName());
	m_lock->Acquire();
	m_waitNum++;
	if (m_status == SIGNALED) {
		m_lock->Release();
		return;
	}
	m_barrier->Wait(m_lock);
	m_lock->Release();
	DEBUG('t', "%s wait return\n", currentThread->getName());
}

void EventBarrier::Complete()
{
	DEBUG('t', "%s Complete\n", currentThread->getName());
	m_lock->Acquire();
	if (m_waitNum == 1) {
		m_complete->Broadcast(m_lock);
		m_waitNum--;
	}
	else {
		m_waitNum--;
		m_complete->Wait(m_lock);
	}
	m_lock->Release();
	DEBUG('t', "%s Complete return\n", currentThread->getName());
}


