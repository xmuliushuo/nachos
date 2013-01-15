#include "memorymanager.h"

MemoryManager::MemoryManager(int numPhysPages):
	m_numPhysPages(numPhysPages),
	m_physMemTable(new Table(numPhysPages))
{ }

int MemoryManager::AllocNewPage()
{
	return m_physMemTable->Alloc((void *)1);
}