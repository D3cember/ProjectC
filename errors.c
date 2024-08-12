#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "headers/errors.h"
#include "headers/globaldefine.h"


/* The Following array specifies the error code numbers and the corresponding error message.
 * Missing lines are intentionally left blank for future error codes
 * */
Error errors[] = {
        {ERROR_CODE_0,"No Error"},
        {ERROR_CODE_1,"Failed to dynamically allocate memory"},/* USED IN DATASTRUCT,*/
        {ERROR_CODE_2,"Setting the pointer to the start of file is FAILED"},
        {ERROR_CODE_3,"Macro declare is invaild"},/* used in Preproc when endmacr has extra text after.*/
        {ERROR_CODE_4," Error open file"},
        {ERROR_CODE_5," Macro name invaild."},
        {ERROR_CODE_6,"Macro declare invaild."},
        {ERROR_CODE_7,"Operation name is invaild."},
        {ERROR_CODE_8,"Label name is a reserved name."},
        {ERROR_CODE_9," - FIRST_PASS - : .data Parameters are invaild."},
        {ERROR_CODE_10," - FIRST_PASS - :.string Values in string are invaild."},
        {ERROR_CODE_11," - FIRST_PASS - : Failed to allocate memory for new symbol"},
        {ERROR_CODE_12,"File must contain valid name !\n"}, /* Used in main errors*/
        {ERROR_CODE_13,"Unkown Instruction."},
        {ERROR_CODE_14," - FIRST_PASS - : .string parameter is out of range."},
        {ERROR_CODE_15," - FIRST_PASS - : .data parameter is out of range."},
        {ERROR_CODE_16," - FIRST_PASS - :Label name is invaild."},
        {ERROR_CODE_17," - FIRST_PASS - :Label name is already saved."},



};

/*
TODO : need to re adjust the error names and modfiy it well.
       and to check about every occurence of error use.


*/

void print_internal_error(int error_code) {
    /* Print the error code number and the error message */
    printf("~~ERROR: ID:%d~~ | %s\n", error_code, errors[error_code].error_msg);
}

void print_external_error(int error_code, location file) {
    /* Print the error code number, file name, assembly line number and the error message */
    printf("~~ERROR: ID:%d~~ in %s at line:%d at colonum : %s | there is error: %s\n", error_code, \
    file.file_name, file.line_num,file.col, errors[error_code].error_msg);
}

