#include <stdio.h>
#include <stdlib.h>

#include "debug.h"
#include "utils.h"
#include "ilist.h"
#include "plist.h"



char* logfile = "log.txt";

void debug(char* s) {
	printf("%s", s);
	char c;
	scanf("%c", &c);
}


void print_point(point* p) {
	printf("x: %d, y: %d, n: %d, k: %d, c1: %d, cc: %d, cf: %d, state: %d, succ: %d\n", 
		p->x, p->y, p->n, p->k, p->c1, p->cc, p->cf, p->state, p->successeur);
}


void print_ilist(ilist* l) {
	while (l) {
		printf("%d\n\n",l->val);
		l = l->next;
	}
}


void print_plist(plist* l) {
	while (l) {
		print_point(l->p);
		l = l->next;
	}
	printf("\n\n");
}


void print_tab(int* tab, int width, int height) {
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			printf("%d       ", tab[j * width + i]);
		}
		printf("\n\n\n");
	}
}


int* succ_to_itab(struct norm_arg* all) { // " successeurs
	int* result = malloc((all->width)*(all->height)*sizeof(int));
	int temp_i_ptab;
	int temp_i_tab;
	for (int x = 0; x < all->width; x++) {
		for (int y = 0; y < all->height; y++) {
			temp_i_tab = x*(all->width) + y;
			coord_to_index(x, y, &temp_i_ptab, all);
			result[temp_i_tab] = (all->tab)[temp_i_ptab]->p->successeur;
		}
	}
	return result;
}


int* ptab_to_itab_bis(struct norm_arg* all) { // Pareil, avec les bords en plus
	int* result = malloc((2*BORDER + all->width)*(2*BORDER + all->height)*sizeof(int));
	int temp_i_ptab;
	int temp_i_tab;
	for (int x = -BORDER; x < BORDER + all->width; x++) {
		for (int y = -BORDER; y < BORDER + all->height; y++) {
			temp_i_tab = (x + BORDER)*(2*BORDER + all->width) + y + BORDER;
			coord_to_index(x, y, &temp_i_ptab, all);
			result[temp_i_tab] = (all->tab)[temp_i_ptab]->p->state;
		}
	}
	return result;
}
