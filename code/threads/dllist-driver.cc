/*
 * dllist-driver.cc
 *
 *  Created on: 2012-11-30
 *      Author: liushuo
 */

#include "dllist.h"
//#include "utility.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void InsertNItemsToDLList(DLList *list, int N)
{
	int key;
	srand(time(NULL));
	for (int i = 0; i < N; ++i) {
		key = rand() % 100;
		list->SortedInsert(NULL, key);
	}
}

void RemoveNItemsFromDLList(DLList *list, int N)
{
	int key;
	for (int i = 0; i < N; ++i) {
		list->Remove(&key);
		printf("%d ", key);
	}
	printf("\n");
}

int main()
{
	DLList list;
	InsertNItemsToDLList(&list, 10);
	RemoveNItemsFromDLList(&list, 10);
	return 0;
}
