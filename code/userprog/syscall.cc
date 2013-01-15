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
    Thread *thread = new Thread(filename);
    thread->space = space;
    thread->Fork(ThreadRun, 0);
    return 0;
}