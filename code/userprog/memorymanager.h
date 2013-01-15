#ifndef MM_H
#define MM_H

#include "table.h"

class MemoryManager {
public:
	MemoryManager(int numPhysPages);
	~MemoryManager() { delete m_physMemTable; }
	int AllocNewPage();
private:
	int m_numPhysPages;
	Table *m_physMemTable;
};

#endif