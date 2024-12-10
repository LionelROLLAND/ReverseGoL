#ifndef __SPECIFIC_H
#define __SPECIFIC_H

#include "ilist.h"
#include "plist.h"
#include "utils.h"


void rajoute_points(int x, int y, ilist** rec_part, ilist** rec_all, struct norm_arg* all);

int left_cells(int x, int y, struct norm_arg* all);

int living_cells(int x, int y, struct norm_arg* all);

int get_c1(int n, int k, int successeur, int* next_state);

int get_cc(int n, int k, int successeur, int* neighb_state, int p_state);

int get_cf(int x, int y, int* next_state, struct norm_arg* all);

int compute_priority(ilist* rec_part, ilist* rec_all, struct norm_arg* all);

void set_point(int x, int y, int st, struct norm_arg* all);

int trie(ilist** rec_part, ilist** rec_all, struct norm_arg* all);

void reinit_point(int x, int y, struct norm_arg* all);

int reinit_add_comp_sort(plist* to_reinit, plist* STOP, plist* just_changed, struct norm_arg* all);

#endif