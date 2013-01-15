/*
 * alarm.h
 *
 *  Created on: 2013-1-14
 *      Author: liushuo
 */

#ifndef ALARM_H_
#define ALARM_H_

#include "dllist.h"

class Alarm {
public:
	Alarm();
	~Alarm();
	void Pause(int howLong);
	void WakeUp();
	int GetPauseNum() { return pauseNum; }
private:
	DLList *m_queue;
	int pauseNum;
};


#endif /* ALARM_H_ */
