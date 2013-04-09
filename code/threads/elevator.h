/*
 * elevator.h
 *
 *  Created on: 2013-1-14
 *      Author: liushuo
 */

#ifndef ELEVATOR_H_
#define ELEVATOR_H_

#include "eventbarrier.h"
/*

Here are the method signatures for the Elevator and Building classes.
You should feel free to add to these classes, but do not change the
existing interfaces.

*/

class Elevator {
	friend class Building;
public:
	Elevator(char *debugName, int numFloors, int myID);
	~Elevator();
	char *getName() { return name; }

	// elevator control interface: called by Elevator thread
	void OpenDoors();                //   signal exiters and enterers to action
	void CloseDoors();               //   after exiters are out and enterers are in
	void VisitFloor(int floor);      //   go to a particular floor

	// elevator rider interface (part 1): called by rider threads.
	bool Enter();                    //   get in
	void Exit();                     //   get out (iff destinationFloor)
	void RequestFloor(int floor);    //   tell the elevator our destinationFloor

	void Run();
	// insert your methods here, if needed

private:
	char *name;

	int currentfloor;           // floor where currently stopped
	int occupancy;              // how many riders currently onboard
	int m_id;
	int m_numFloors;
	EventBarrier **m_barrierIn;
	EventBarrier **m_barrierOut;
	enum {UP, DOWN, STOP} m_status;

	Condition *m_serCon; // the condition that the service list is empty
	Lock *m_serConLock;  // the lock to be used with m_serCon also 
	                  // protects the m_serList.
	int *m_serList;      // the service list, the ith item is 1 
	                  // means that there is a request on the ith floor
	// insert your data structures here, if needed
};

class Building {
	friend class Elevator;
public:
	Building(char *debugname, int numFloors, int numElevators);
	~Building();
	char *getName() { return name; }


	// elevator rider interface (part 2): called by rider threads
	void CallUp(int fromFloor);      //   signal an elevator we want to go up
	void CallDown(int fromFloor);    //   ... down
	Elevator *AwaitUp(int fromFloor); // wait for elevator arrival & going up
	Elevator *AwaitDown(int fromFloor); // ... down
	Elevator *GetElevator(int index) { return elevator[index]; }
private:
	char *name;
	Elevator **elevator;         // the elevators in the building (array)
	int m_numElevators;
	Condition *m_noElevator;
	Lock *m_noElevatorLock;
	// insert your data structures here, if needed
};

#endif /* ELEVATOR_H_ */
