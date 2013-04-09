/*
 * alarm.cc
 *
 *  Created on: 2013-1-14
 *      Author: liushuo
 */


#include "alarm.h"
#include "system.h"

const int TimerTicksPerSec = 100000;

void static loopwhile(int which)
{
    while(sysAlarm->GetPauseNum()!=0)
    {
        currentThread->Yield();
    }
    currentThread->Finish();
}


Alarm::Alarm():m_queue(new DLList("alarm list"))
{}

Alarm::~Alarm()
{
	delete m_queue;
}

void Alarm::Pause(int howLong)
{
	if (howLong <= 0)
		return;
    pauseNum++;
    if( pauseNum == 1 ){
        Thread *t = new Thread("while thread");
        t->Fork(loopwhile, 0);
    }

	int wakeTime = stats->totalTicks + howLong * TimerTicks * TimerTicksPerSec;
    DEBUG('A', "thread wakeTime: %d\n", wakeTime);
	IntStatus oldLevel =interrupt->SetLevel(IntOff);
    m_queue->SortedInsert(currentThread, wakeTime);
    //pauseNum++;
    DEBUG('A', "thread now go to sleep\n");
    currentThread->Sleep();
    (void)interrupt->SetLevel(oldLevel);
}

void Alarm::WakeUp()
{
	Thread *thread;
    int wakeTime = -1;
    thread = (Thread *)m_queue->Remove(&wakeTime);
    //DEBUG('A', "wakeTime: %d now : %d\n", wakeTime, stats->totalTicks);
    while (thread != NULL) {
        if (stats->totalTicks >= wakeTime) {
            scheduler->ReadyToRun(thread);
            pauseNum--;
            thread = (Thread *)m_queue->Remove(&wakeTime);
        }
        else {
            m_queue->SortedInsert(thread, wakeTime);
            break;
        }
    }
}