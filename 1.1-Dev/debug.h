#ifndef __DEBUG_H
#define __DEBUG_H

#include "utils.h"
#include "ilist.h"
#include "plist.h"

extern char* logfile;

void debug(char* s);

void print_point(point* p);

void print_ilist(ilist* l);

void print_plist(plist* l);

void print_tab(int* tab, int width, int height);

int* succ_to_itab(struct norm_arg* all);

int* ptab_to_itab_bis(struct norm_arg* all);


#endif