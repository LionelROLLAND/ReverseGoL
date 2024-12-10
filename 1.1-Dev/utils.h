#ifndef __UTILS_H
#define __UTILS_H

#include <stdio.h>

#include "ilist.h"
#include "plist.h"


#define REND_ENTIER 2520 // = PPCM(1,2,3,4,5,6,7,8,9)
#define BORDER 2
#define INFINITY 22681 // = 9*2520 + 1

#include "ilist.h"
#include "plist.h"


struct norm_arg { // Arguments wrappes pour que ce soit moins moche
	plist** tab;
	int width;
	int height;
	plist** prio_list;
	//FILE* fd;
};



char* int_str(unsigned int n);

int maxi(int n, int p);

int get_arg(int argc, char* argv[], char** in_file, char** out_file, char** pred_file, int* rec);

void index_to_coord(int p, int* x, int* y, struct norm_arg* all);

void coord_to_index(int x, int y, int* p, struct norm_arg* all);

ilist* remove_defined(ilist* l, struct norm_arg* all);

int* ptab_to_itab(struct norm_arg* all);

int* cf_to_itab(struct norm_arg* all);

int* read_grid(char* fname, int* width, int* height);

int write_grid(int* tab, char* fname, int width, int height);

//plist* rev_ilist_to_plist(ilist* l, struct norm_arg* all);

//ilist* plist_to_rev_ilist(plist* l, struct norm_arg* all);

plist* ilist_to_rev_plist(ilist* l, struct norm_arg* all);

void free_tab_prio_all(struct norm_arg* all);

#endif