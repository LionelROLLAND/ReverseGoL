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


/*
ilist* add_a_next_ilist(ilist* l, ilist* to_add) {
	if (l) {
		attach_ilist(to_add, l->next);
	} else {
		attach_ilist(to_add, NULL);
	}
	attach_ilist(l, to_add);
	return to_add;
}


ilist* take_ilist(ilist* l) {
	attach_ilist(l->prev, l->next);
	l->prev = NULL;
	l->next = NULL;
	return l;
}
*/


void free_ilist(ilist* l) {
	ilist* temp;
	while (l) {
		temp = l;
		l = l->next;
		free(temp);
	}
}


/*
ilist* sort(ilist* l, ilist** last) {
	if (!l) {
		*last = NULL;
		return NULL;
	}
	ilist* temp = l->next;
	ilist* save;
	ilist* gauche = NULL;
	ilist* droite = NULL;
	while (temp) {
		save = temp;
		temp = temp->next;
		if (save->val <= l->val) {
			attach_ilist(save, gauche);
			gauche = save;
		} else {
			attach_ilist(save, droite);
			droite = save;
		}
	}

	gauche = sort(gauche, &temp);
	droite = sort(droite, last);
	attach_ilist(temp, l);
	attach_ilist(l, droite);
	if (!droite) *last = l;
	if (!gauche) gauche = l;
	return gauche;
}
*/


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


/*


ilist* copy_ilist(ilist* l) {
	ilist* result = NULL;
	while (l && l->next) {
		l = l->next;
	}
	while (l) {
		result = add_a_prev_ilist(result, new_ilist(l->val));
		l = l->prev;
	}
	return result;
}
*/
