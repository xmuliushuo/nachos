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
    int id = threadTable->Alloc(thread);
    if (id < 0) {
        printf("Has reached the max process number.\n");
        return -1;
    }
    thread->Fork(ThreadRun, 0);
    return id;
}

int _Join(int id)
{
    Thread *thread = (Thread *)threadTable->Get(id);
    if (thread == NULL) {
        printf("the process %d doesn't exist.\n", id);
        return -1;
    }
    int status = thread->GetExitStatus();
    threadTable->Release(id);
    delete thread;
    return status;
}

void _Exit(int status)
{
    currentThread->SetExitStatus(status);
    for (int i = 0; i < currentThread->space->GetNumPages(); i++) {
        mm->FreePage(currentThread->space->GetPageTable()[i].physicalPage);
    }
    delete currentThread->space;
    currentThread->Finish();
}