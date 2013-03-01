#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "table.h"

class Process {
public:
	Process(Thread *t);
	~Process();
    Thread *thread;
    int status;
    Lock *m_exitConLock;
    Condition *m_exitCondition;
    int id;
};


class ProcessManager {
public:
	ProcessManager(int maxProcessNum);
	~ProcessManager() { delete m_processTable; }
	int GetFreeId(Process *);
	Process *Get(int id);
	void Release(int id);
private:
	int m_maxProcessNum;
	Table *m_processTable;
};

#endif