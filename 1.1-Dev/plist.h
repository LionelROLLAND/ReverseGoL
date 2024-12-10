#ifndef __PLIST_H
#define __PLIST_H

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


/* etat :

0 -> morte
1 -> vivante
2 -> indeterminee
3 -> sert a rien
4 -> doit etre fixee a 0
5 -> doit etre fixee a 1

*/

typedef struct point_list {
	point* p;
	struct point_list* prev;
	struct point_list* next;
} plist;


plist* new_plist(point* to_add);

void attach_plist(plist* l, plist* to_add);

plist* add_a_prev_plist(plist* l, plist* to_add);

plist* add_a_next_plist(plist* l, plist* to_add);

plist* take_plist(plist* l);

void free_plist(plist* l);

plist* sort_dec_p(plist* l, plist** last);

plist* sort_snd_into_fst_dec(plist* base, plist* insert);

#endif