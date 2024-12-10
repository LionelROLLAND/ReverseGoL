#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "utils.h"
#include "ilist.h"
#include "plist.h"



char* int_str(unsigned int n)  {
	int p;
	if (n == 0) {
		p = 2;
	} else {
		int i = 1;
		long int t = 1;
		while (t <= n) {
			t = 10*t;
			i++;
		}
		p = i;
	}
	char* s = malloc(p*sizeof(char));
	s[p-1] = 0;
	unsigned int temp = n;
	int i = p-2;
	while (i >= 0) {
		s[i] = temp%10 + 48;
		i--;
		temp = temp/10;
	}
	return s;
}


int maxi(int n, int p) {
	if (n < p) return p;
	return n;
}

int get_arg(int argc, char* argv[], char** in_file, char** out_file, char** pred_file, int* rec) { //Traite les arguments de la CLI
	int len;
	int success = getopt(argc, argv, "i:o:p:RC");
	while (success != -1) {
		switch ((char) success) {
			case 'i':
			len = strlen(optarg);
			*in_file = calloc(len + 1, sizeof(char));
			strcpy(*in_file, optarg);
			break;

			case 'o':
			len = strlen(optarg);
			*out_file = calloc(len + 1, sizeof(char));
			strcpy(*out_file, optarg);
			break;

			case 'p':
			len = strlen(optarg);
			*pred_file = calloc(len + 1, sizeof(char));
			strcpy(*pred_file, optarg);
			break;

			case 'R':
			*rec = 1;
			break;
			
			case 'C':
			*rec = -1;
			break;

			default:
			return 1;
		}
		success = getopt(argc, argv, "i:o:p:RC");
	}
	return 0;

}



void index_to_coord(int p, int* x, int* y, struct norm_arg* all) {  // index = place dans le tableau
	*x = p % (2 * BORDER + all->width) - BORDER;
	*y = p / (2 * BORDER + all->width) - BORDER;
}


void coord_to_index(int x, int y, int* p, struct norm_arg* all) {
	*p = ((y + BORDER) * (2 * BORDER + all->width)) + x + BORDER;
}


ilist* remove_defined(ilist* l, struct norm_arg* all) {  //Enleve les index des points qu'on a deja fixes
	ilist* result = l;
	ilist* temp;
	while (l) {
		temp = l;
		l = l->next;
		if ((all->tab)[temp->val]->p->state <= 1) {
			if (result == temp) result = result->next;
			attach_ilist(temp->prev, temp->next);
			free(temp);
		}
	}
	return result;
}


int* ptab_to_itab(struct norm_arg* all) {  // Donne le tableau des etats a partir de celui des points
	int* result = malloc((all->width)*(all->height)*sizeof(int));
	int temp_i_ptab;
	int temp_i_tab;
	for (int x = 0; x < all->width; x++) {
		for (int y = 0; y < all->height; y++) {
			temp_i_tab = x*(all->width) + y;
			coord_to_index(x, y, &temp_i_ptab, all);
			result[temp_i_tab] = (all->tab)[temp_i_ptab]->p->state;
		}
	}
	return result;
}


int* cf_to_itab(struct norm_arg* all) { // Pareil avec cf
	int* result = malloc((all->width)*(all->height)*sizeof(int));
	int temp_i_ptab;
	int temp_i_tab;
	for (int x = 0; x < all->width; x++) {
		for (int y = 0; y < all->height; y++) {
			temp_i_tab = x*(all->width) + y;
			coord_to_index(x, y, &temp_i_ptab, all);
			result[temp_i_tab] = (all->tab)[temp_i_ptab]->p->cf;
		}
	}
	return result;
}


int* read_grid(char* fname, int* width, int* height) {  //fichier --> tableau d'entier
	FILE* fd = fopen(fname, "r");
	if (fd == NULL) {
		printf("error\n\n");
		return NULL;
	}

	unsigned char c;
	*height = 0;
	*width = 0;
	ilist* temp = NULL;
	int s = fscanf(fd,"%c",&c);

	while (s != EOF && !feof(fd)) {
		(*height)++;
		while (s != EOF && !feof(fd) && c != '\n') {
			int accu = 0;
			while (s != EOF && !feof(fd) && c >= '0' && c <= '9') {
				if (*height == 1) {
					(*width)++;
				}
				accu = 10*accu + (int) c - 48;
				s = fscanf(fd,"%c",&c);
			}

			temp = add_a_prev_ilist(temp, new_ilist(accu));

			while (s != EOF && !feof(fd) && c == ';') {
				s = fscanf(fd, "%c", &c);
			}
		}
		s = fscanf(fd, "%c", &c);
	}

	fclose(fd);
	int length = (*height) * (*width);
	int* tab = malloc(length * sizeof(int));
	ilist* save;

	for (int i = length - 1; i >= 0; i--) {
		save = temp;
		tab[i] = save->val;
		temp = save->next;
		free(save);
	}
	

	return tab;
}


int write_grid(int* tab, char* fname, int width, int height) { //tableau d'entier --> fichier
	FILE* fd = fopen(fname, "w");
	if (!fd) {
		printf("Error\n\n");
		return -1;
	}
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int temp = tab[j*width + i];
			if (temp < 0) {
				char* t_s = int_str((unsigned int) (-temp));
				fprintf(fd, "%c", '-');
				fprintf(fd, "%s", t_s);
				free(t_s);
			} else {
				char* t_s = int_str((unsigned int) temp);
				fprintf(fd, "%s", t_s);
				free(t_s);
			}
			fprintf(fd, "%c", ';');
		}
		fprintf(fd, "\n");
	}
	fclose(fd);
	return 0;
}


plist* ilist_to_rev_plist(ilist* l, struct norm_arg* all) {  //convertit une ilist en une plist en inversant l'ordre
	plist* result = NULL;
	plist* temp;
	while (l) {
		temp = (all->tab)[l->val];
		if (*all->prio_list == temp) *all->prio_list = (*all->prio_list)->next;
		temp = take_plist(temp);
		result = add_a_prev_plist(result, temp);
		l = l->next;
	}
	return result;
}


void free_tab_prio_all(struct norm_arg* all) {
	int max = (2*BORDER + all->width)*(2*BORDER + all->height);
	for (int i = 0; i < max; i++) {
		free((all->tab)[i]->p);
	}
	free(all->tab);
	free(all->prio_list);
	free(all);
}