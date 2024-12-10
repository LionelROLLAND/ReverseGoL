#include <stdio.h>
#include <stdlib.h>

#include "ilist.h"



ilist* new_ilist(int n) {
	ilist* new = malloc(sizeof(ilist));
	new->val = n;
	new->prev = NULL;
	new->next = NULL;
	return new;
}


void attach_ilist(ilist* l, ilist* to_add) {
	if (to_add) to_add->prev = l;
	if (l) l->next = to_add;
}


ilist* add_a_prev_ilist(ilist* l, ilist* to_add) {
	if (l) {
		attach_ilist(l->prev, to_add);
	} else {
		attach_ilist(NULL, to_add);
	}
	attach_ilist(to_add, l);
	return to_add;
}


void free_ilist(ilist* l) {
	ilist* temp;
	while (l) {
		temp = l;
		l = l->next;
		free(temp);
	}
}


ilist* sort_unique(ilist* l, ilist** last) { //Trie en enlevant les doublons de maniere grave opti
	if (!l) {
		*last = NULL;
		return NULL;
	}
	ilist* temp = l->next;
	ilist* save;
	ilist* gauche = NULL;
	ilist* droite = NULL;
	while (temp) {
		temp->prev = NULL;
		save = temp;
		temp = temp->next;
		if (save->val < l->val) {
			gauche = add_a_prev_ilist(gauche, save);
		} else if (save->val > l->val) {
			droite = add_a_prev_ilist(droite, save);
		} else {
			free(save);
		}
	}

	gauche = sort_unique(gauche, &temp);
	droite = sort_unique(droite, last);
	attach_ilist(temp, l);
	attach_ilist(l, droite);
	if (!droite) *last = l;
	if (!gauche) gauche = l;
	return gauche;
}
