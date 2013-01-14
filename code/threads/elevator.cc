/*
 * elevator.cc
 *
 *  Created on: 2013-1-14
 *      Author: liushuo
 */

#include "elevator.h"

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
	return elevator[0];
}

Elevator *Building::AwaitDown(int fromFloor)
{
	return elevator[0];
}

Elevator::Elevator(char *debugName, int numFloors, int myID)
{
	name = debugName;
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
}

Elevator::~Elevator()
{
	for (int i = 0; i < m_numFloors; i++) {
		delete m_barrierIn[i];
		delete m_barrierOut[i];
	}
	DeallocBoundedArray((char *)m_barrierIn, m_numFloors);
	DeallocBoundedArray((char *)m_barrierOut, m_numFloors);
}

bool Elevator::Enter()
{
	occupancy++;
	return true;
}

void Elevator::RequestFloor(int floor)
{
	m_barrierOut[floor]->Wait();
}

void Elevator::Exit()
{
	m_barrierOut[currentfloor]->Complete();
	occupancy--;
}

void Elevator::VisitFloor(int floor)
{
	currentfloor = floor;
}

void Elevator::OpenDoors()
{
	m_barrierOut[currentfloor]->Signal();
	m_barrierIn[currentfloor]->Signal();
}

void Elevator::CloseDoors()
{
	m_barrierOut[currentfloor]->Complete();
	m_barrierIn[currentfloor]->Complete();
}
