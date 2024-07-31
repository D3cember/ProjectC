#ifndef PREPROC_H
#define PREPROC_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "data_struct.h"


int check_endmacr_format(const char *line);
NodeOnList* find_macro(LinkedListOfMacro *macroTable, const char *name);
void process_file(FILE *as, FILE *am);


#endif /*PREPROC_H*/ 
