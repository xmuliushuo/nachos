/*
 * dllist-driver.cc
 *
 *  Created on: 2012-11-30
 *      Author: liushuo
 */

#include "dllist.h"
#include "utility.h"


void InsertNItemsToDLList(DLList *list, int N, int which)
{
	int key;

	for (int i = 0; i < N; ++i) {
		key = Random() % 100;
		list->SortedInsert(NULL, key);
		printf("Thread %d: item was inserted with key = %d\n", which, key);
	}
}

void RemoveNItemsFromDLList(DLList *list, int N, int which)
{
	int key;
	for (int i = 0; i < N; ++i) {
		list->Remove(&key);
		printf("Thread %d: item was removed with key = %d\n", which, key);
	}
}

//int main()
//{
//	DLList list;
//	InsertNItemsToDLList(&list, 10);
//	RemoveNItemsFromDLList(&list, 10);
//	return 0;
//}
