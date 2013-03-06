/*
 * table.cc
 *
 *  Created on: 2013-1-13
 *      Author: liushuo
 */


#include "table.h"

Table::Table(int size, char *debugName):m_size(size),
	m_lock(new Lock("table lock")),
	m_entry((void **)AllocBoundedArray(sizeof(void *) * size)),
	name(debugName)
{
	if (m_entry == NULL)
		printf("alloc error!");
    for(int i = 0; i < m_size; i++)
    	m_entry[i] = NULL;
}

int Table::Alloc(void *object)
{
	m_lock->Acquire();
	for (int i = 0; i < m_size; i++) {
		if (m_entry[i] == NULL) {
			m_entry[i] = object;
			m_lock->Release();
			return i;
		}
	}
	m_lock->Release();
	return -1;
}

void* Table::Get(int index)
{
	ASSERT((index > 0) && (index < m_size));
	void *thing;
	m_lock->Acquire();
	thing = m_entry[index];
	m_lock->Release();
	return thing;
}

void Table::Release(int index)
{
	m_lock->Acquire();
	m_entry[index] = NULL;
	m_lock->Release();
}
