#include <stdlib.h>

#include "plist.h"


plist* new_plist(point* to_add) {
	plist* new = malloc(sizeof(plist));
	new->p = to_add;
	new->prev = NULL;
	new->next = NULL;
	return new;
}


void attach_plist(plist* l, plist* to_add) {
	if (to_add) to_add->prev = l;
	if (l) l->next = to_add;
}


plist* add_a_prev_plist(plist* l, plist* to_add) {
	if (l) {
		attach_plist(l->prev, to_add);
	} else {
		attach_plist(NULL, to_add);
	}
	attach_plist(to_add, l);
	return to_add;
}


plist* add_a_next_plist(plist* l, plist* to_add) {
	if (l) {
		attach_plist(to_add, l->next);
	} else {
		attach_plist(to_add, NULL);
	}
	attach_plist(l, to_add);
	return to_add;
}


plist* take_plist(plist* l) {
	attach_plist(l->prev, l->next);
	l->prev = NULL;
	l->next = NULL;
	return l;
}



void free_plist(plist* l) {
	plist* temp;
	while (l) {
		temp = l;
		l = l->next;
		free(temp);
	}
}

plist* sort_dec_p(plist* l, plist** last) { //Trie l dans l'ordre decroissant des priorites, et renvoie le dernier element via last (important pour la fonction)
	if (!l) {
		*last = NULL;
		return NULL;
	}
	plist* temp = l->next;
	plist* save;
	plist* gauche = NULL;
	plist* droite = NULL;
	while (temp) {
		save = temp;
		temp = temp->next;
		if (save->p->cf >= l->p->cf) {
			gauche = add_a_prev_plist(gauche, save);
		} else {
			droite = add_a_prev_plist(droite, save);
		}
	}

	gauche = sort_dec_p(gauche, &temp);
	droite = sort_dec_p(droite, last);
	attach_plist(temp, l);
	attach_plist(l, droite);
	if (!droite) *last = l;
	if (!gauche) gauche = l;
	return gauche;
}


plist* sort_snd_into_fst_dec(plist* base, plist* insert) { //base et insert etant toutes les deux triees dans l'ordre decroissant, va inserer insert dans base en conservant l'ordre
	//printf("In sort :\n");
	plist* result = base;
	plist* temp;
	plist* last = base;
	while (insert && base) {
		last = base;
		if (insert->p->cf < base->p->cf) {
			base = base->next;
		} else {
			temp = insert->next;
			insert = take_plist(insert);
			add_a_prev_plist(base, insert);
			if (!(insert->prev)) {
				result = insert;
			}
			insert = temp;
		}
		//printf("Prio :\n");
		//print_plist(result);
		//printf("insert :\n");
		//print_plist(insert);
		//debug("");

	}
	while (base) {
		last = base;
		base = base->next;
	}
	attach_plist(last, insert);
	if (insert && !(insert->prev)) {
		result = insert;
	}
	return result;
}