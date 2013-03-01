#include "system.h"
#include "addrspace.h"
#include "syscall.h"

void ThreadRun(int arg)
{
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	machine->Run();
}


SpaceId _Exec(char *filename)
{
	OpenFile *executable = fileSystem->Open(filename);
    AddrSpace *space;

    if (executable == NULL) {
    	printf("Unable to open file %s\n", filename);
    	return -1;
    }
    space = new AddrSpace();
    if (!space->Setup(executable)) {
        printf("setup AddrSpace failed!\n");
        delete space;
        delete executable;
        return -1;
    } 
    delete executable;
    Thread *thread = new Thread(filename);
    thread->space = space;
    Process *p = new Process(thread);
    thread->process = p;
    int id = pm->GetFreeId(p);
    thread->id = id;
    p->id = id;
    if (id < 0) {
        printf("Has reached the max process number.\n");
        return -1;
    }
    thread->Fork(ThreadRun, 0);
    return id;
}

int _Join(int id)
{
    Process *p = pm->Get(id);
    if (p == NULL) {
        printf("the process %d doesn't exist.\n", id);
        return -1;
    }
    //DEBUG('a', "thread->isZombie = %d\n", thread->isZombie);
    if (p->status == -1) {
        printf("the process %d hasn't exit.\n", p->id);
        p->m_exitConLock->Acquire();
        p->m_exitCondition->Wait(p->m_exitConLock);
        p->m_exitConLock->Release();
    }
    int status = p->status;
    printf("now going to release%d\n", id);
    pm->Release(id);
    printf("join thread %d\n", id);
    // TODO this must be deleted!!
    delete p;
    DEBUG('a', "syscall.cc 65\n");
    return status;
}

void _Exit(int status)
{
    DEBUG('a', "thread %d exit\n", currentThread->id);
    //currentThread->SetExitStatus(status);
    
    for (int i = 0; i < currentThread->space->GetNumPages(); i++) {
        mm->FreePage(currentThread->space->GetPageTable()[i].physicalPage);
    }
    delete currentThread->space;
    Process *p = currentThread->process;
    DEBUG('a', "thread %d delete space\n", currentThread->id);
    if (p != NULL) {
       // p->status = status;
        p->m_exitConLock->Acquire();
	p->status = status;
        p->m_exitCondition->Broadcast(p->m_exitConLock);
        p->m_exitConLock->Release();
    }
    //DEBUG('a', "thread %d broadcast\n", currentThread->id);
    currentThread->Finish();
}
