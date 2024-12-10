#include "specific.h"
#include "utils.h"
#include "ilist.h"
#include "plist.h"

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


void set_point(int x, int y, int st, struct norm_arg* all) { //Fixe x y dans l'etat st et applique les modifs de k et n qui en decoulent sur les voisines
	/*
	char* str_x = int_str(x);
	char* str_y = int_str(y);
	char* str_st = int_str(st);
	fprintf(all->fd, "%s;%s;%s\n", str_x, str_y, str_st);
	free(str_x);
	free(str_y);
	free(str_st);
	*/
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
					//debug("Abnormal behaviour\n");
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
	/*
	char* str_x = int_str(x);
	char* str_y = int_str(y);
	fprintf(all->fd, "%s;%s;2\n", str_x, str_y);
	free(str_x);
	free(str_y);
	*/
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