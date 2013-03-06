/*
 * elevator.cc
 *
 *  Created on: 2013-1-14
 *      Author: liushuo
 */
#include "elevator.h"
#include "system.h"

Building::Building(char *debugname, int numFloors, int numElevators)
{
	name = debugname;
	elevator = (Elevator **)AllocBoundedArray(sizeof(Elevator *) * numElevators);
	m_numElevators = numElevators;
	for (int i = 0; i < numElevators; i++) {
		elevator[i] = new Elevator("elevator", numFloors, i);
	}
}

Building::~Building()
{
	for (int i = 0; i < m_numElevators; i++) {
		delete elevator[i];
	}
	DeallocBoundedArray((char *)elevator, m_numElevators);
}

void Building::CallUp(int fromFloor)
{}

void Building::CallDown(int fromFloor)
{}

Elevator *Building::AwaitUp(int fromFloor)
{
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	DEBUG('t', "awaitup1\n");
	elevator[0]->m_serConLock->Acquire();
	DEBUG('t', "awaitup2\n");
	elevator[0]->m_serList[fromFloor] = 1;
	elevator[0]->m_serCon->Signal(elevator[0]->m_serConLock);
	elevator[0]->m_serConLock->Release();
	elevator[0]->m_barrierIn[fromFloor]->Wait();
	(void) interrupt->SetLevel(oldLevel);
	return elevator[0];
}

Elevator *Building::AwaitDown(int fromFloor)
{
	IntStatus oldLevel = interrupt->SetLevel(IntOff);
	elevator[0]->m_serConLock->Acquire();
	elevator[0]->m_serList[fromFloor] = 1;
	elevator[0]->m_serCon->Signal(elevator[0]->m_serConLock);
	elevator[0]->m_serConLock->Release();// here is a time window
	elevator[0]->m_barrierIn[fromFloor]->Wait();
	(void) interrupt->SetLevel(oldLevel);
	return elevator[0];
}

Elevator::Elevator(char *debugName, int numFloors, int myID):
	name(debugName),
	currentfloor(0),
	m_status(STOP)
{
	DEBUG('t', "construction of elevator\n");
	m_numFloors = numFloors;
	m_id = myID;
	m_barrierIn = (EventBarrier **)AllocBoundedArray(sizeof(EventBarrier *) * numFloors);
	for (int i = 0; i < numFloors; i++) {
		m_barrierIn[i] = new EventBarrier();
	}
	m_barrierOut = (EventBarrier **)AllocBoundedArray(sizeof(EventBarrier *) * numFloors);
	for (int i = 0; i < numFloors; i++) {
		m_barrierOut[i] = new EventBarrier();
	}
	m_serCon = new Condition("service condition");
	m_serConLock = new Lock("service condition lock");
	m_serList = new int[m_numFloors];
	for (int i = 0; i < numFloors; ++i) m_serList[i] = 0;
}

Elevator::~Elevator()
{
	for (int i = 0; i < m_numFloors; i++) {
		delete m_barrierIn[i];
		delete m_barrierOut[i];
	}
	DeallocBoundedArray((char *)m_barrierIn, m_numFloors);
	DeallocBoundedArray((char *)m_barrierOut, m_numFloors);
	delete m_serCon;
	delete m_serConLock;
	delete[] m_serList;
}

bool Elevator::Enter()
{
	occupancy++;
	m_barrierIn[currentfloor]->Complete();
	return true;
}

void Elevator::RequestFloor(int floor)
{
	// DEBUG('t', "requeset floor %d\n", floor);
	m_serConLock->Acquire();
	// DEBUG('t', "requeset floor %d - 2\n", floor);
	m_serList[floor] = 1;
	m_serCon->Signal(m_serConLock);
	m_serConLock->Release();
	m_barrierOut[floor]->Wait();
}

void Elevator::Exit()
{
	m_barrierOut[currentfloor]->Complete();
	occupancy--;
}

void Elevator::VisitFloor(int floor)
{
	int sleepTime = floor > currentfloor ? (floor - currentfloor) : (currentfloor - floor);
	DEBUG('e', "Elevator::VisitFloor: sleepTime = %d\n", sleepTime);
	sysAlarm->Pause(sleepTime);
	currentfloor = floor;
	printf("%s visit floor %d.\n", name, floor);
	
	if (m_serList[floor] == 1) {
		m_serConLock->Acquire();
		m_serList[floor] = 0;
		m_serConLock->Release();
		OpenDoors();
		CloseDoors();
	}
	
}

void Elevator::OpenDoors()
{
	printf("%s open door.\n", name);
	printf("%d riders will go out\n", m_barrierOut[currentfloor]->Waiters());
	if (m_barrierOut[currentfloor]->Waiters() > 0)
		m_barrierOut[currentfloor]->Signal();
	printf("%d riders will go in\n", m_barrierIn[currentfloor]->Waiters());
	if (m_barrierIn[currentfloor]->Waiters() > 0)
		m_barrierIn[currentfloor]->Signal();
}

void Elevator::CloseDoors()
{
	printf("%s close door.\n", name);
	//m_barrierOut[currentfloor]->Complete();
	//m_barrierIn[currentfloor]->Complete();
}

void Elevator::Run()
{
	currentfloor = 0;
	bool hasService;
	int i;
	int nextFloor = -1;
	while(1) {
		hasService = false;
		m_serConLock->Acquire();
		// for (int i = 0; i < m_numFloors; i++) {
		// 	DEBUG('a', "%d, ", m_serList[i]);
		// }
		// DEBUG('a', "\n");
		switch (m_status) {
		case UP:
			for (i = currentfloor; i < m_numFloors; i++) {
				if (m_serList[i] == 1) {
					hasService = true;
					nextFloor = i;
				}
			}
			if (hasService == false) {
				for (i = currentfloor; i >= 0; i--) {
					if (m_serList[i] == 1) {
						hasService = true;
						nextFloor = i;
					}
				}
			}
			break;
		case DOWN:
			for (i = currentfloor; i >= 0; i--) {
				if (m_serList[i] == 1) {
					hasService = true;
					nextFloor = i;
				}
			}
			if (hasService == false) {
				for (i = currentfloor; i < m_numFloors; i++) {
					if (m_serList[i] == 1) {
						hasService = true;
						nextFloor = i;
					}
				}
			}
			break;
		case STOP:
			for (i = 0; i < m_numFloors; i++) {
				if (m_serList[i] == 1) {
					hasService = true;
					nextFloor = i;
					break;
				}
			}
			break;
		}
		if (hasService == false) {
			nextFloor = -1;
			m_status = STOP;
			printf("There is no request now, so elevator %s stops on the %dth floor.\n", name, currentfloor);
			m_serCon->Wait(m_serConLock);
			DEBUG('t', "elevator thread wake up\n");
		}
		m_serConLock->Release();
		DEBUG('t', "elevator thread wake up2\n");
		DEBUG('t', "nextFloor: %d\n", nextFloor);
		if (nextFloor != -1) {
			if (nextFloor > currentfloor) {
				m_status = UP;
				VisitFloor(currentfloor + 1);
			}
			else if (nextFloor < currentfloor) {
				m_status = DOWN;
				VisitFloor(currentfloor - 1);
			}
			else VisitFloor(currentfloor);
		}
	}
}
