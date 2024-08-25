#ifndef SECOND_PASS_H
#define SECOND_PASS_H
#include "data_struct.h"
#include "globaldefine.h"
#include "errors.h"
#include "first_pass.h"

void add_to_ent_file(const char *filename, const char *label, int address);
void second_pass(char *filename,char *fileNameAm);
void free_code_list(CodeNode *head);


#endif /*SECOND_PASS_H*/

