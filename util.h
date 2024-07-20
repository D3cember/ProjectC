#ifndef PROJECTC_UTIL_H
#define PROJECTC_UTIL_H


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "data_struct.h"

extern const char *reserved_keywords[20];
char *copy_contain(FILE *as, fpos_t *fpos, int macro_length);
char* trim_whitespace(char* str);

#endif
