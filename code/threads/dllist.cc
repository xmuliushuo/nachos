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

//----------------------------------------------------------------------
// List::Remove
//      Remove the first "item" from the front of the list.
//
// Returns:
//	Pointer to removed item, NULL if nothing on the list.
//----------------------------------------------------------------------

void *
DLList::Remove(int *keyPtr)
{
    //return SortedRemove(keyPtr);  // Same as SortedRemove, but ignore the key
}

//----------------------------------------------------------------------
// List::Mapcar
//	Apply a function to each item on the list, by walking through
//	the list, one element at a time.
//
//	Unlike LISP, this mapcar does not return anything!
//
//	"func" is the procedure to apply to each element of the list.
//----------------------------------------------------------------------

void
List::Mapcar(VoidFunctionPtr func)
{
    for (ListElement *ptr = first; ptr != NULL; ptr = ptr->next) {
       DEBUG('l', "In mapcar, about to invoke %x(%x)\n", func, ptr->item);
       (*func)((int)ptr->item);
    }
}

//----------------------------------------------------------------------
// List::IsEmpty
//      Returns TRUE if the list is empty (has no items).
//----------------------------------------------------------------------

bool
List::IsEmpty()
{
    if (first == NULL)
        return TRUE;
    else
	return FALSE;
}

//----------------------------------------------------------------------
// List::SortedInsert
//      Insert an "item" into a list, so that the list elements are
//	sorted in increasing order by "sortKey".
//
//	Allocate a ListElement to keep track of the item.
//      If the list is empty, then this will be the only element.
//	Otherwise, walk through the list, one element at a time,
//	to find where the new item should be placed.
//
//	"item" is the thing to put on the list, it can be a pointer to
//		anything.
//	"sortKey" is the priority of the item.
//----------------------------------------------------------------------

void
List::SortedInsert(void *item, int sortKey)
{
    ListElement *element = new ListElement(item, sortKey);
    ListElement *ptr;		// keep track

    if (IsEmpty()) {	// if list is empty, put
        first = element;
        last = element;
    } else if (sortKey < first->key) {
		// item goes on front of list
	element->next = first;
	first = element;
    } else {		// look for first elt in list bigger than item
        for (ptr = first; ptr->next != NULL; ptr = ptr->next) {
            if (sortKey < ptr->next->key) {
		element->next = ptr->next;
	        ptr->next = element;
		return;
	    }
	}
	last->next = element;		// item goes at end of list
	last = element;
    }
}

//----------------------------------------------------------------------
// List::SortedRemove
//      Remove the first "item" from the front of a sorted list.
//
// Returns:
//	Pointer to removed item, NULL if nothing on the list.
//	Sets *keyPtr to the priority value of the removed item
//	(this is needed by interrupt.cc, for instance).
//
//	"keyPtr" is a pointer to the location in which to store the
//		priority of the removed item.
//----------------------------------------------------------------------

void *
List::SortedRemove(int *keyPtr)
{
    ListElement *element = first;
    void *thing;

    if (IsEmpty())
	return NULL;

    thing = first->item;
    if (first == last) {	// list had one item, now has none
        first = NULL;
	last = NULL;
    } else {
        first = element->next;
    }
    if (keyPtr != NULL)
        *keyPtr = element->key;
    delete element;
    return thing;
}
