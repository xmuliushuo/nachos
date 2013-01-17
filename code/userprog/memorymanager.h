#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include "table.h"

class MemoryManager {
public:
	MemoryManager(int numPhysPages);
	~MemoryManager() { delete m_physMemTable; }
	int AllocNewPage();
	void FreePage(int);
private:
	int m_numPhysPages;
	Table *m_physMemTable;
};

#endif