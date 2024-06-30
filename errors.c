#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "errors.h"
#include "data_struct.h"

/* The Following array specifies the error code numbers and the corresponding error message.
 * Missing lines are intentionally left blank for future error codes
 * */
Error errors[] = {
        {ERROR_CODE_0,  "No Error"},
        {ERROR_CODE_1,  "Failed to dynamically allocate memory"},/* USED IN DATASTRUCT,*/
        {ERROR_CODE_2,"Setting the pointer to the start of file is FAILED"},
        {ERROR_CODE_3,"Macro declare is invaild"}/* used in Preproc when endmacr has extra text after.*/

};

void print_internal_error(int error_code) {
    /* Print the error code number and the error message */
    printf("~~ERROR: ID:%d~~ | %s\n", error_code, errors[error_code].error_msg);
}

void print_external_error(int error_code, location file) {
    /* Print the error code number, file name, assembly line number and the error message */
    printf("~~ERROR: ID:%d~~ in %s at line:%d | there is error: %s\n", error_code, \
    file.file_name, file.line, errors[error_code].error_msg);
}