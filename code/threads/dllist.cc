/*
 * dllist.cc
 *
 *  Created on: 2012-11-24
 *      Author: liushuo
 */

#include "dllist.h"
#include "utility.h"

DLLElement::DLLElement(void *itemPtr, int sortKey):
			next(NULL),
			prev(NULL),
			key(sortKey),
			item(itemPtr)
{

}

DLList::DLList():
		first(NULL),
		last(NULL)
{
}

DLList::~DLList()
{
    while (Remove(NULL) != NULL)
	;	 // delete all the list elements
}

void
DLList::Append(void *item)
{
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
}

void
DLList::Prepend(void *item)
{
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
}

// remove from head of list
// set *keyPtr to key of the removed item
// return item(or NULL if list is empty)
void *
DLList::Remove(int *keyPtr)
{
	// TODO
    //return SortedRemove(keyPtr);  // Same as SortedRemove, but ignore the key
	return NULL;
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
    			element->prev = ptr;
    			ptr->next->prev = element;
    			ptr->next = element;
    			return;
    		}
    	}
    	last->next = element;
    	element->prev = last;
    	last = element;
    }
}

// remove first item with key==sortKey
// return NULL if no such item exists
void *
List::SortedRemove(int *keyPtr)
{
	// TODO
	return NULL;
//    ListElement *element = first;
//    void *thing;
//
//    if (IsEmpty())
//	return NULL;
//
//    thing = first->item;
//    if (first == last) {	// list had one item, now has none
//        first = NULL;
//	last = NULL;
//    } else {
//        first = element->next;
//    }
//    if (keyPtr != NULL)
//        *keyPtr = element->key;
//    delete element;
//    return thing;
}
