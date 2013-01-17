#include "processmanager.h"
#include "system.h"

Process::Process(Thread *t)
{
	thread = t;
    m_exitConLock = new Lock("thread exit condition lock");
    m_exitCondition = new Condition("thread exit condition");
    status = -1;
}

ProcessManager::ProcessManager(int maxProcessNum):
	m_maxProcessNum(maxProcessNum),
	m_processTable(new Table(maxProcessNum))
{}

int ProcessManager::GetFreeId(Process *thread)
{
	return m_processTable->Alloc((void *)thread);
}

Process * ProcessManager::Get(int id)
{
	return (Process *)m_processTable->Get(id);
}

void ProcessManager::Release(int id)
{
	m_processTable->Release(id);
}