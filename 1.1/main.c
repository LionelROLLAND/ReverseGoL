#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "specific.h"
#include "ilist.h"
#include "plist.h"
//#include "debug.h"

int dandelifinal(plist** changed, struct norm_arg* all) { // Fonction recursive qui gere le backtracking
	plist** prio = all->prio_list;
	plist* temp_l = *prio;
	if (!temp_l) return 0;
	int success;
	while (temp_l && temp_l->p->cf == INFINITY) { //Boucle qui fixe les points pour lesquels on n'a pas le choix de l'etat a ce stade
		ilist* rec_all = NULL;
		ilist* rec_part = NULL;
		while (temp_l && temp_l->p->cf == INFINITY) {
			*prio = (*prio)->next;
			temp_l = take_plist(temp_l); //On enleve le point de la liste de priorite
			set_point(temp_l->p->x, temp_l->p->y, temp_l->p->state - 4, all); //On fixe le point
			*changed = add_a_prev_plist(*changed, temp_l); //On rajoute le point a la liste des points changes
			rajoute_points(temp_l->p->x, temp_l->p->y, &rec_part, &rec_all, all);
			temp_l = *prio;
		}
		success = trie(&rec_part, &rec_all, all); //Retourne -1 = pb = On doit remonter dans le backtracking
		free_ilist(rec_all);
		free_ilist(rec_part);
		if (success < 0) return -1;
		temp_l = *prio;
	}
	plist* the_point_l = *prio;
	if (!the_point_l) return 0; //La liste de priorite est vide = On a gagne


	*prio = (*prio)->next;
	the_point_l = take_plist(the_point_l); //Hop, on essaie de fixer le prochain point a 0 :
	set_point(the_point_l->p->x, the_point_l->p->y, 0, all);
	ilist* rec_part = NULL;
	ilist* rec_all = NULL;
	rajoute_points(the_point_l->p->x, the_point_l->p->y, &rec_part, &rec_all, all); //Ca fait des points a recalculer
	success = trie(&rec_part, &rec_all, all);

	free_ilist(rec_all);
	free_ilist(rec_part);
	plist* save_changed = *changed; //Sert a distinguer les points qu'on a modifies nous (apres save_changed) de ceux qui seront modifies par les appels plus profonds (avant save_changed) -> On n'a pas encore mis the_point_l dedans
	if (!success) success = dandelifinal(changed, all); //Si ca retourne 0 on a gagne

	if (!success) return 0;
	
	reinit_point(the_point_l->p->x, the_point_l->p->y, all); //Sinon ca marche pas et il faut maintenant tenter de fixer le point a 1 :
	set_point(the_point_l->p->x, the_point_l->p->y, 1, all);

	success = reinit_add_comp_sort(*changed, save_changed, the_point_l, all); //L'appel precedent nous a refile ses poubelles, on va devoir tout reinitialiser et recalculer

	if (!success) {
		*changed = save_changed;
		success = dandelifinal(changed, all);
	}
	if (!success) return 0;
	

	*changed = add_a_prev_plist(*changed, the_point_l);

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



	free_ilist(count);
	if (success < 0) return NULL;
	return all;
}


int main(int argc, char* argv[]) {
	char* in_file;
	char* out_file;
	char* pred_file;
	int rec;
	
	get_arg(argc, argv, &in_file, &out_file, &pred_file, &rec);

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
		return -1;
	}
	int success = 0;
	if (rec < 0) {
		int* cf_tab = cf_to_itab(all);
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
	if (success < 0) return -1;
	return 0;
}