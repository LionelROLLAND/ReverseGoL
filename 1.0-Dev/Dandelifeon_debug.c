#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define REND_ENTIER 2520 // = PPCM(1,2,3,4,5,6,7,8,9)
#define BORDER 2
#define INFINITY 22681 // = 9*2520 + 1

char* logfile = "log.txt";

typedef struct intlist {
	int val;
	struct intlist* prev;
	struct intlist* next;
} ilist;


typedef struct point_info {
	int x;
	int y;
	int state; //etat du point
	int successeur; //etat du successeur
	int next_state_c1; // Etat (+4) a donner a la case dans le cas ou c1 = infini
	int next_state_cc; // Etat 8+4) a donner aux voisines dans le cas ou cc = infini
	int n; //nb de cases libres (= pas determinees) autour = cr = cases remplies si on complete le point
	int k; //nb de cases vivantes autour
	int c1; //   1/(nb minimum de cases voisines a remplir pour qu'on ait plus le chooix de l'état du centre)   
	int cc; //   nb minimum de cases voisines a remplir pour qu'on ait plus le choix de l'état des autres voisines 
	int cf; //   contrainte finale
} point;


typedef struct point_list {
	point* p;
	struct point_list* prev;
	struct point_list* next;
} plist;


struct norm_arg { // Arguments wrappes pour que ce soit moins moche
	plist** tab;
	int width;
	int height;
	plist** prio_list;
	FILE* fd;
};

/* etat :

0 -> morte
1 -> vivante
2 -> indeterminee
3 -> sert a rien
4 -> doit etre fixee a 0
5 -> doit etre fixee a 1

*/


void debug(char* s) {
	printf("%s", s);
	char c;
	scanf("%c", &c);
}


void print_point(point* p) {
	printf("x: %d, y: %d, n: %d, k: %d, c1: %d, cc: %d, cf: %d, state: %d, succ: %d\n", 
		p->x, p->y, p->n, p->k, p->c1, p->cc, p->cf, p->state, p->successeur);
}


void print_plist(plist* l) {
	while (l) {
		print_point(l->p);
		l = l->next;
	}
	printf("\n\n");
}


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


void free_ilist(ilist* l) {
	ilist* temp;
	while (l) {
		temp = l;
		l = l->next;
		free(temp);
	}
}


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

void print_ilist(ilist* l) {
	while (l) {
		printf("%d\n\n",l->val);
		l = l->next;
	}
}


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


void print_tab(int* tab, int width, int height) {
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			printf("%d       ", tab[j * width + i]);
		}
		printf("\n\n\n");
	}
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


plist* rev_ilist_to_plist(ilist* l, struct norm_arg* all) {
	plist* result = NULL;
	plist* temp;
	while (l) {
		temp = (all->tab)[l->val];
		if (*all->prio_list == temp) {
			*all->prio_list = (*all->prio_list)->next;
		}
		temp = take_plist(temp);
		result = add_a_prev_plist(result, temp);
		l = l->prev;
	}
	return result;
}


ilist* plist_to_rev_ilist(plist* l, struct norm_arg* all) {
	ilist* result = NULL;
	int temp_index;
	while (l) {
		coord_to_index(l->p->x, l->p->y, &temp_index, all);
		result = add_a_prev_ilist(result, new_ilist(temp_index));
		l = l->next;
	}
	return result;
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


void free_plist(plist* l) {
	plist* temp;
	while (l) {
		temp = l;
		l = l->next;
		free(temp);
	}
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


void rajoute_points(int x, int y, ilist** rec_part, ilist** rec_all, struct norm_arg* all) { //Rajoute les points dont il faut recalculer la contrainte dans les listes qu'il faut quand l'etat de x y change
	int temp_index;
	for (int i = -2; i < 3; i++) {
		for (int j = -2; j < 3; j++) {
			coord_to_index(x + i, y + j, &temp_index, all);
			if ((all->tab)[temp_index]->p->state != 3) {
				*rec_part = add_a_prev_ilist(*rec_part, new_ilist(temp_index));
				if ((i <= 1) && (i >= -1) && (j <= 1) && (j >= -1)) {
					*rec_all = add_a_prev_ilist(*rec_all, new_ilist(temp_index));
				}
			}
		}
	}
}


int left_cells(int x, int y, struct norm_arg* all) {  // cellules voisines indeterminees autour de x y
	int n = 0;
	int temp_index;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (i || j) {
				coord_to_index(x + i, y + j, &temp_index, all);
				n = n + ((all->tab)[temp_index]->p->state == 2);
			}
		}
	}
	return n;
}


int living_cells(int x, int y, struct norm_arg* all) {  //" " vivantes " " " "
	int k = 0;
	int temp_index;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (i || j) {
				coord_to_index(x + i, y + j, &temp_index, all);
				k = k + ((all->tab)[temp_index]->p->state == 1);
			}
		}
	}
	return k;
}


//Les 3 fonctions obscures qui decoulent de mon idee :


int get_c1(int n, int k, int successeur, int* next_state) { //calcule c1 pour des n et k donnes, retourne aussi l'etat force le cas echeant
	if (successeur == 0) {
		if (k == 3 && n == 0) {
			return -1; //erreur
		} else if (k > 3 || n + k < 2) {
			return 0;
		} else if (n + k == 2) {
			*next_state = 4;
			if (n) return REND_ENTIER/n;
			return INFINITY; //contrainte infinie
		} else {
			*next_state = 4;
			if (n-1) return REND_ENTIER/(n-1);
			return INFINITY;
		}
	} else if (successeur == 1) {
		if (k > 3 || n + k < 2) {
			return -1;
		} else if (k <= 2) {
			*next_state = 5;
			if (n + k - 2) return REND_ENTIER/(n + k - 2);
			return INFINITY;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}


int get_cc(int n, int k, int successeur, int* neighb_state, int p_state) { //Pareil avec cc
	//printf("n : %d, k : %d, succ : %d, state : %d \n", n, k, successeur, p_state);
	if (p_state == 2 || p_state >= 4) {
		int n_state0;
		int n_state1;
		int result0 = get_cc(n, k, successeur, &n_state0, 0);
		int result1 = get_cc(n, k, successeur, &n_state1, 1);
		if (result0 == 0 && result1 == 0 && n_state0 != n_state1) {
			return -1;
		} else if (result0 < result1) {
			*neighb_state = n_state1;
			return result1;
		} else {
			*neighb_state = n_state0;
			return result0;
		}
	}

	if (successeur == 0) {
		if (p_state == 1) {
			if (n + k < 2 || k > 3) {
				return INFINITY;
			} else if (k >= 2) {
				*neighb_state = 5;
				return n + k - 4;
			} else {
				*neighb_state = 4;
				return maxi(n - 2, 1 - k);
			}
		} else {
			if (k > 3 || n + k < 3) {
				return INFINITY;
			} else if (k <= 2) {
				*neighb_state = 4;
				return maxi(n - 1, 2 - k);
			} else {
				*neighb_state = 5;
				return n - 1;
			}
		}
	} else if (successeur == 1) {
		if (p_state == 1) {
			if ((k == 2 && n <= 1) || (k == 3 && n == 0)) {
				return INFINITY;
			} else if (n + k - 2 <= 3 - k && k <= 1) {
				*neighb_state = 5;
				return n + k - 2;
			} else {
				*neighb_state = 4;
				return 3 - k;
			}
		} else {
			if (k == 3 && n == 0) {
				return INFINITY;
			} else if (n + k - 3 <= 3 - k && k <= 2) {
				*neighb_state = 5;
				return n + k - 3;
			} else {
				*neighb_state = 4;
				return 3 - k;
			}
		} 
	} else {
		return INFINITY;
	}
}


int get_cf(int x, int y, int* next_state, struct norm_arg* all) { //Pareil avec cf
	//printf("pb point x : %d   y : %d\n", x, y);
	int temp_index;
	coord_to_index(x, y, &temp_index, all);
	point* temp_point = (all->tab)[temp_index]->p;
	int c = temp_point->c1;
	*next_state = temp_point->next_state_c1;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			coord_to_index(x + i, y + j, &temp_index, all);
			temp_point = (all->tab)[temp_index]->p;
			if ((i || j) && temp_point->state != 3) {
				switch (temp_point->cc) {
					case 0:
					if (c == INFINITY && temp_point->next_state_cc != *next_state) return -1;
					*next_state = temp_point->next_state_cc;
					c = INFINITY;
					break;

					case INFINITY:
					break;

					default:
					if (c != INFINITY) c = c + REND_ENTIER/temp_point->cc;
				}
			}
		}
	}
	return c;
}


int compute_priority(ilist* rec_part, ilist* rec_all, struct norm_arg* all) { //Recalcule les contraintes des points de rec_part, a condition que rec_all corresponde
	int state_prevision;
	point* t_point;
	ilist* copy_part = rec_part;
	while (copy_part) { //Est probablement devenue accessoire
		t_point = (all->tab)[copy_part->val]->p;
		if (t_point->state >= 4) t_point->state = 2;
		copy_part = copy_part->next;
	}


	while (rec_all) {
		t_point = (all->tab)[rec_all->val]->p;

		if (t_point->state != 3) {
			t_point->cc = get_cc(t_point->n, t_point->k, t_point->successeur, &state_prevision, t_point->state);
			//print_point(t_point);
			//printf("cc : %d\n", t_point->cc);
			if (t_point->cc < 0) {
				return -1;
			} else if (t_point->cc == 0) {
				t_point->next_state_cc = state_prevision;
			}
		}




		if (t_point->state == 2 || t_point->state >= 4) {
			t_point->c1 = get_c1(t_point->n, t_point->k, t_point->successeur, &state_prevision);
			//print_point(t_point);
			//printf("c1 : %d\n\n", t_point->c1);

			switch (t_point->c1) {
				case INFINITY:
				t_point->next_state_c1 = state_prevision;
				break;

				case -1:
				return -1;
				break;
			}
		}
		rec_all = rec_all->next;
	}

	

	while (rec_part) {
		t_point = (all->tab)[rec_part->val]->p;
		if (t_point->state == 2 || t_point->state >= 4) {
			t_point->cf = get_cf(t_point->x, t_point->y, &state_prevision, all);
			if (t_point->cf < 0) return -1;
			if (t_point->cf == INFINITY) t_point->state = state_prevision;
		}
		//print_point(t_point);
        //int* info = ptab_to_itab(all);
        //print_tab(info, all->width, all->height);
        //free(info);
		rec_part = rec_part->next;
	}

	return 0;
}  //Cette fonction foire = l'etat des points de la grille ne permet plus d'atteindre la grille des successeurs qu'on s'est fixee


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


void set_point(int x, int y, int st, struct norm_arg* all) { //Fixe x y dans l'etat st et applique les modifs de k et n qui en decoulent sur les voisines
	char* str_x = int_str(x);
	char* str_y = int_str(y);
	char* str_st = int_str(st);
	fprintf(all->fd, "%s;%s;%s\n", str_x, str_y, str_st);
	free(str_x);
	free(str_y);
	free(str_st);
	int temp_index;
	point* temp_point;
	coord_to_index(x, y, &temp_index, all);

	int change = ((all->tab)[temp_index]->p->state >= 2);
	(all->tab)[temp_index]->p->state = st;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (i || j) {
				if (change) {
					coord_to_index(x + i, y + j, &temp_index, all);
					//printf("index : %d", temp_index);
					temp_point = (all->tab)[temp_index]->p;
					(temp_point->n)--;
					temp_point->k = temp_point->k + st;
					//printf("n: %d, k: %d, succ: %d, cc: %d, cf: %d\n", temp_point->n, temp_point->k, temp_point->successeur, temp_point->cc, temp_point->cf);
					//printf("state = %d\n", (all->tab)[temp_index]->p->state);
				} else {
					debug("Abnormal behaviour\n");
				}
			}
		}
	}
}


int trie(ilist** rec_part, ilist** rec_all, struct norm_arg* all) { //Recalcule les contraintes de rec_part et la retrie dans la liste des priorites
	ilist* last_part;
	//print_ilist(*rec_part);
	//print_ilist(*rec_all);
	*rec_all = sort_unique(*rec_all, &last_part);
	*rec_part = sort_unique(*rec_part, &last_part);
	//debug("sorting passed \n");
	int success = compute_priority(*rec_part, *rec_all, all);
	//debug("Priority passed\n");
	if (success < 0) return -1;
	*rec_part = remove_defined(*rec_part, all);
	plist* to_insert = ilist_to_rev_plist(*rec_part, all); //pb ici : Ca prend des mecs de *changed ca fait n'imp --> ah non
	//debug("To insert passed\n");
	plist* nothing;
	to_insert = sort_dec_p(to_insert, &nothing);
	//debug("sort_dec_p passed\n");
	//printf("Prio :\n");
	//print_plist(*all->prio_list);
	//printf("to_insert :\n");
	//print_plist(to_insert);
	//debug("");

	*all->prio_list = sort_snd_into_fst_dec(*all->prio_list, to_insert);
	//printf("After Sort :\nPrio :\n");
	//print_plist(*all->prio_list);
	//debug("sort_snd passed\n");
	return 0;
}


void reinit_point(int x, int y, struct norm_arg* all) { //Reinitialise x y (= le fixe a 2) et appliques les modifs de k et n sur les voisines
	char* str_x = int_str(x);
	char* str_y = int_str(y);
	fprintf(all->fd, "%s;%s;2\n", str_x, str_y);
	free(str_x);
	free(str_y);
	int temp_index;
	coord_to_index(x, y, &temp_index, all);
	point* main_point = (all->tab)[temp_index]->p;
	point* temp_point;
	for (int i = -1; i < 2; i++) {
		for (int j = -1; j < 2; j++) {
			if (i || j) {
				coord_to_index(x + i, y + j, &temp_index, all);
				temp_point = (all->tab)[temp_index]->p;
				if (main_point->state == 1) {
					(temp_point->n)++;
					temp_point->k = temp_point->k - 1;
				} else if (main_point->state == 0) {
					(temp_point->n)++;
				}
			}
		}
	}
	main_point->state = 2;
}


/* Prochaine fonction :

On veut reinitialiser les points de to_reinit (jusau'a STOP) et on vient de changer l'etat de ceux de just_changed
Il va s'ensuivre un certain nombre de points dont on veut recalculer les contraintes
Du coup :

1. On reinitialise to_reinit
2. On ajoute les bons points aux bonnes listes
3. On recalcule les priorites
4. On trie tout ca
5. D'ou le nom de la fonction
*/


int reinit_add_comp_sort(plist* to_reinit, plist* STOP, plist* just_changed, struct norm_arg* all) {
	ilist* rec_all = NULL;
	ilist* rec_part = NULL;
	//debug("cp 1-1\n");
	while (to_reinit != STOP) {
		//printf("temp :\n");
		//print_plist(to_reinit);
		reinit_point(to_reinit->p->x, to_reinit->p->y, all);
		//debug("cp 1-1-1\n");
		rajoute_points(to_reinit->p->x, to_reinit->p->y, &rec_part, &rec_all, all);
		//debug("cp 1-1-2\n");
		to_reinit = to_reinit->next;
	}
	//debug("cp 1-2\n");
	while (just_changed) {
		//debug("Before rajoute_points in reinit 2\n");
		rajoute_points(just_changed->p->x, just_changed->p->y, &rec_part, &rec_all, all);
		//debug("After rajoute_points in reinit 2\n");
		just_changed = just_changed->next;
	}
	//debug("cp 1-3\n");

	int success = trie(&rec_part, &rec_all, all);

	//debug("cp 1-4\n");
	free_ilist(rec_all);
	free_ilist(rec_part);
	if (success < 0) return -1;

	return 0;
}


int dandelifinal(plist** changed, struct norm_arg* all) { // Fonction recursive qui gere le backtracking
	plist** prio = all->prio_list;
	plist* temp_l = *prio;
	if (!temp_l) return 0;
	int success;
	//int* info = ptab_to_itab(all);
    //print_tab(info, all->width, all->height);
    //free(info);
	//print_plist(*all->prio_list);
	//printf("pb INFINITY ?\n");
	while (temp_l && temp_l->p->cf == INFINITY) { //Boucle qui fixe les points pour lesquels on n'a pas le choix de l'etat a ce stade
		ilist* rec_all = NULL;
		ilist* rec_part = NULL;
		while (temp_l && temp_l->p->cf == INFINITY) {
			//printf("INFINITY\nChanged :\n");
			//print_plist(*changed);
			//printf("Prio :\n");
			//print_plist(*all->prio_list);
			//debug("");
			*prio = (*prio)->next;
			temp_l = take_plist(temp_l); //On enleve le point de la liste de priorite
			//print_point(temp_l->p);
			set_point(temp_l->p->x, temp_l->p->y, temp_l->p->state - 4, all); //On fixe le point
			*changed = add_a_prev_plist(*changed, temp_l); //On rajoute le point a la liste des points changes
			//debug("Before rajoute_points in INFINITY\n");
			rajoute_points(temp_l->p->x, temp_l->p->y, &rec_part, &rec_all, all);
			//debug("After rajoute_points in INFINITY\n");
			//debug("end rajoute_points\n");
			temp_l = *prio;
		}
		//debug("Before tri\n");
		success = trie(&rec_part, &rec_all, all); //Retourne -1 = pb = On doit remonter dans le backtracking
		//int* info = ptab_to_itab(all);
    	//print_tab(info, all->width, all->height);
    	//free(info);
		//print_plist(*all->prio_list);
		//debug("Tri OK \n");
		free_ilist(rec_all);
		free_ilist(rec_part);
		if (success < 0) return -1;
		temp_l = *prio;
	}
	//printf("Nope.\n");
	plist* the_point_l = *prio;
	//print_point(the_point_l->p);
	if (!the_point_l) return 0; //La liste de priorite est vide = On a gagne


	*prio = (*prio)->next;
	the_point_l = take_plist(the_point_l); //Hop, on essaie de fixer le prochain point a 0 :
	//print_point(temp_l->p);
	set_point(the_point_l->p->x, the_point_l->p->y, 0, all);
	//printf("set_point ok\n");
	ilist* rec_part = NULL;
	ilist* rec_all = NULL;
	//debug("Just before rajoute_points\n");
	rajoute_points(the_point_l->p->x, the_point_l->p->y, &rec_part, &rec_all, all); //Ca fait des points a recalculer
	//debug("rajoute_points passed\n");


	//int* info = ptab_to_itab(all);
    //print_tab(info, all->width, all->height);
    //free(info);
    //printf("\n");
	//printf("1st tri\nChanged :\n");
	//print_plist(*changed);
	//printf("Prio :\n");
	//print_plist(*all->prio_list);
	//debug("");
	//printf("Just after rajoute_points\n");
	success = trie(&rec_part, &rec_all, all);

	//printf("And then :\n");
	//printf("Changed :\n");
	//print_plist(*changed);
	//printf("Prio :\n");
	//print_plist(*all->prio_list);
	//debug("");
	free_ilist(rec_all);
	free_ilist(rec_part);
	plist* save_changed = *changed; //Sert a distinguer les points qu'on a modifies nous (apres save_changed) de ceux qui seront modifies par les appels plus profonds (avant save_changed) -> On n'a pas encore mis the_point_l dedans
	if (!success) success = dandelifinal(changed, all); //Si ca retourne 0 on a gagne

	//debug("1st dandelifinal passed\n");
	if (!success) return 0;
	
	//printf("cp 1\n");
	reinit_point(the_point_l->p->x, the_point_l->p->y, all); //Sinon ca marche pas et il faut maintenant tenter de fixer le point a 1 :
	//printf("cp 2\n");
	set_point(the_point_l->p->x, the_point_l->p->y, 1, all);
	//printf("cp 3\n");

	success = reinit_add_comp_sort(*changed, save_changed, the_point_l, all); //L'appel precedent nous a refile ses poubelles, on va devoir tout reinitialiser et recalculer
	//printf("cp 4\n");

	if (!success) {
		*changed = save_changed;
		success = dandelifinal(changed, all);
	}
	if (!success) return 0;
	

	*changed = add_a_prev_plist(*changed, the_point_l);

	//debug("2nd dandelifinal passed\n");
	return -1; //A ce point-la, rien n'a marche et il faut remonter dans le backtracking
}


struct norm_arg* init(int* successeurs, int* pre_def, int width, int height) { //Initialise tout ce qu'il faut
	plist** mega_grid = malloc((2*BORDER + width)*(2*BORDER + height)*sizeof(plist*));
	ilist* count = NULL;
	plist* to_sort = NULL;
	struct norm_arg* all = malloc(sizeof(struct norm_arg));
	all->width = width;
	all->height = height;
	all->prio_list = malloc(sizeof(plist*));
	*all->prio_list = NULL;
	all->tab = mega_grid;
	all->fd = fopen(logfile, "w");


	plist* new_pl;
	point* new_p;
	int temp_i_ptab;
	int temp_i_tab;
	for (int px = -BORDER; px < BORDER + width; px++) {
		for (int py = -BORDER; py < BORDER + height; py++) {
			new_p = malloc(sizeof(point));
			new_pl = new_plist(new_p);
			new_p->x = px;
			new_p->y = py;
			coord_to_index(px, py, &temp_i_ptab, all);
			mega_grid[temp_i_ptab] = new_pl;
			if (px >= 0 && px < width && py >= 0 && py < height) {
				temp_i_tab = px*(all->width) + py;
				new_p->state = pre_def[temp_i_tab];
				new_p->successeur = successeurs[temp_i_tab];
				count = add_a_prev_ilist(count, new_ilist(temp_i_ptab));
				if (new_p->state == 2) to_sort = add_a_prev_plist(to_sort, new_pl);
			} else {
				new_p->state = 3;
			}
		}
	} // tableau cree

	while (count) {
		new_p = mega_grid[count->val]->p;
		new_p->n = left_cells(new_p->x, new_p->y, all);
		new_p->k = living_cells(new_p->x, new_p->y, all);
		new_p->cf = 0;
		count = count->next;
	}
	int success = reinit_add_comp_sort(NULL, NULL, to_sort, all);
	//printf("Prio :\n");
	//print_plist(*all->prio_list);



	free_ilist(count);
	if (success < 0) return NULL;
	return all;
}


int main(int argc, char* argv[]) {
	char* in_file;
	char* out_file;
	char* pred_file;
	int rec;
	
	/*
	for (int i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}
	*/
	get_arg(argc, argv, &in_file, &out_file, &pred_file, &rec);
	//printf("%d\n\n", rec);

	int width;
	int height;
	int* grid = read_grid(in_file, &width, &height);
	int* pred = read_grid(pred_file, &width, &height);
	struct norm_arg* all = init(grid, pred, width, height);
	free(grid);
	free(pred);
	if (!all) {
		printf("Pas de solution !\n\n");
		free_tab_prio_all(all);
		write_grid(NULL, out_file, 0, 0);
		fclose(all->fd);
		return -1;
	}
	int success = 0;
	if (rec < 0) {
		int* cf_tab = cf_to_itab(all);
		print_tab(cf_tab, all->width, all->height);
		write_grid(cf_tab, out_file, all->width, all->height);
		free(cf_tab);
	} else {
		plist* changed = NULL;
		success = dandelifinal(&changed, all);
		if (success < 0) {
			printf("Ecological disaster : The game of life is dead.\nSign in as God to restart it.\n");
			write_grid(NULL, out_file, 0, 0);
		} else {
			int* result = ptab_to_itab(all);
			write_grid(result, out_file, all->width, all->height);
			free(result);
		}

	}
	free_tab_prio_all(all);
	fclose(all->fd);
	if (success < 0) return -1;
	return 0;
}