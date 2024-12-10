#ifndef __ILIST_H
#define __ILIST_H



typedef struct intlist {
	int val;
	struct intlist* prev;
	struct intlist* next;
} ilist;


ilist* new_ilist(int n);

void attach_ilist(ilist* l, ilist* to_add);

ilist* add_a_prev_ilist(ilist* l, ilist* to_add);

void free_ilist(ilist* l);

ilist* sort_unique(ilist* l, ilist** last);


#endif