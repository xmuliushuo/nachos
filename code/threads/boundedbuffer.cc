/*
 * boundedm_buffer.cc
 *
 *  Created on: 2013-1-14
 *      Author: liushuo
 */


#include "boundedbuffer.h"
#include "system.h"

BoundedBuffer::BoundedBuffer(int maxsize){
	m_lock = new Lock("buffer lock");
	m_bufferEmpty = new Condition("bufferEmpty");
	m_bufferFull = new Condition("bufferFull");
    m_buffer = (void *)AllocBoundedArray(sizeof(void *) * maxsize);
	m_bufferSize = maxsize;
	m_count = 0;
	m_nextin = m_nextout = 0;
}

BoundedBuffer::~BoundedBuffer()
{
      DeallocBoundedArray((char *)m_buffer, m_bufferSize);
      delete m_lock;
      delete m_bufferEmpty;
      delete m_bufferFull;
}

void BoundedBuffer::Read(void* data, int size){
	int i;
	m_lock->Acquire();
	while(m_count == 0)
		m_bufferFull->Wait(m_lock);
	for(i = 0; i < size; i++){
		while(m_count == 0)
			m_bufferFull->Wait(m_lock);
		*((char *)data + i) = *((char *)m_buffer + m_nextout);
		if(i == size - 1)
			printf("%d\n", size);
		m_nextout = (m_nextout + 1) % m_bufferSize;
		m_count--;
		m_bufferEmpty->Signal(m_lock);
	}
	m_lock->Release();
}

void BoundedBuffer::Write(void *data, int size){
	int j;
	m_lock->Acquire();
	while(m_count == m_bufferSize)
		m_bufferEmpty->Wait(m_lock);
	for(j = 0; j < size; j++){
		while(m_count == m_bufferSize)
			m_bufferEmpty->Wait(m_lock);
		*((char *)m_buffer + m_nextin) = *((char *)data + j);
		//printf("%c\n",*((char *)data +j));
		if(j == size-1)
			printf("%d\n", size);
		m_nextin = (m_nextin+1) % m_bufferSize;
		m_count++;
		m_bufferFull->Signal(m_lock);
	}
	m_lock->Release();
}


