
#include "util.h"
#include <stdlib.h>
#include <string.h>
#include "stdio.h"
#include "util.h"
#include "errors.h"
#include "preproc.h"
#include "data_struct.h"

char *copy_contain(FILE *as,fpos_t *fpos,int length){
    int i;
    char *string;
    if (fsetpos(as,fpos) != 0){
        print_internal_error(ERROR_CODE_2);
        return NULL;
    }
    string = handle_malloc((length+1)*sizeof(char));
    for (i = 0; i <length ; i++) {
        *(string+i) = getc(as);
    }
    *(string+i) = '\0';
    fgetpos(as,fpos);
    return string;
}
