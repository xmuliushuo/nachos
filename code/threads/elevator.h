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

     // insert your methods here, if needed

   private:
     char *name;

     int currentfloor;           // floor where currently stopped
     int occupancy;              // how many riders currently onboard
     int m_id;
     int m_numFloors;
     EventBarrier **m_barrierIn;
     EventBarrier **m_barrierOut;
     // insert your data structures here, if needed
};

class Building {
   public:
     Building(char *debugname, int numFloors, int numElevators);
     ~Building();
     char *getName() { return name; }


     // elevator rider interface (part 2): called by rider threads
     void CallUp(int fromFloor);      //   signal an elevator we want to go up
     void CallDown(int fromFloor);    //   ... down
     Elevator *AwaitUp(int fromFloor); // wait for elevator arrival & going up
     Elevator *AwaitDown(int fromFloor); // ... down

   private:
     char *name;
     Elevator **elevator;         // the elevators in the building (array)
     int m_numElevators;

     // insert your data structures here, if needed
};

#endif /* ELEVATOR_H_ */
