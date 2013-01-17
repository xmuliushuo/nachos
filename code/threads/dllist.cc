/*
 * dllist.cc
 *
 *  Created on: 2012-11-24
 *      Author: liushuo
 */

#include "dllist.h"
#include "utility.h"
#include "system.h"
//#include <stdio.h>
//#define TRUE 1
//#define FALSE 0

DLLElement::DLLElement(void *itemPtr, int sortKey):
			next(NULL),
			prev(NULL),
			key(sortKey),
			item(itemPtr)
{

}

DLList::DLList():
		first(NULL),
		last(NULL),
		lock(new Lock("dllist lock"))
{
}

DLList::~DLList()
{
    while (Remove(NULL) != NULL)
	;	 // delete all the list elements
	delete lock;
}

void
DLList::Append(void *item)
{
	lock->Acquire();
    if (IsEmpty()) {		// list is empty
    	DLLElement *element = new DLLElement(item, 0);
		first = element;
		last = element;
    } else {			// else put it after last
    	DLLElement *element = new DLLElement(item, last->key + 1);
    	element->prev = last;
		last->next = element;
		last = element;
    }
    lock->Release();
}

void
DLList::Prepend(void *item)
{
	lock->Acquire();
    if (IsEmpty()) {		// list is empty
    	DLLElement *element = new DLLElement(item, 0);
		first = element;
		last = element;
    } else {			// else put it before first
    	DLLElement *element = new DLLElement(item, first->key - 1);
		element->next = first;
		first->prev = element;
		first = element;
    }
    lock->Release();
}

// remove from head of list
// set *keyPtr to key of the removed item
// return item(or NULL if list is empty)
void *
DLList::Remove(int *keyPtr)
{
	lock->Acquire();
	if (IsEmpty()) {
		lock->Release();
		return NULL;
	}
	DLLElement *element = first;
	void *thing;
	thing = element->item;
	if (first == last) {
		first = NULL;
		last = NULL;
	}
	else {
		first = first->next;
		currentThread->Yield();
		first->prev = NULL;
	}
	if (keyPtr != NULL) {
		*keyPtr = element->key;
	}
	delete element;
	lock->Release();
	return thing;
}

// return true if list has elements
bool
DLList::IsEmpty()
{
    if (first == NULL)
        return TRUE;
    else
    	return FALSE;
}


void
DLList::SortedInsert(void *item, int sortKey)
{
	lock->Acquire();
    DLLElement *element = new DLLElement(item, sortKey);
    if (IsEmpty()) {
    	first = element;
    	last = element;
    }
    else if (sortKey < first->key) {
    	// item goes on front of list
    	element->next = first;
    	first->prev = element;
    	first = element;
    }
    else {
    	for (DLLElement *ptr = first; ptr->next != NULL; ptr = ptr->next) {
    		if (sortKey < ptr->next->key) {
    			element->next = ptr->next;
    			currentThread->Yield();
    			element->prev = ptr;
    			ptr->next->prev = element;
    			ptr->next = element;
    			lock->Release();
    			return;
    		}
    	}
    	last->next = element;
    	element->prev = last;
    	last = element;
    }
    lock->Release();
}

// remove first item with key==sortKey
// return NULL if no such item exists
void *
DLList::SortedRemove(int sortKey)
{
	lock->Acquire();
	if (IsEmpty()) {
		lock->Release();
		return NULL;
	}
	DLLElement *element;
	for (DLLElement *ptr = first; ptr != NULL; ptr = ptr->next) {
		if (sortKey == ptr->key) {
			element = ptr;
			if (ptr == first) {
				if (first == last) {
					first = last = NULL;
				}
				else {
					first = ptr->next;
					first->prev = NULL;
				}
			}
			else if (ptr == last){
				last = ptr->prev;
				last->next = NULL;
			}
			else {
				ptr->prev->next = ptr->next;
				ptr->next->prev = ptr->prev;
			}
			break;
		}
	}
	if (element != NULL) {
		delete element;
		void *thing = element->item;
		lock->Release();
		return thing;
	}
	else {
		lock->Release();
		return NULL;
	}
}
