#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "headers/first_pass.h"


/* The Following array specifies the error code numbers and the corresponding error message.
 * Missing lines are intentionally left blank for future error codes
 * */
Error errors[] = {
        {ERROR_CODE_0,"No Error"},
        {ERROR_CODE_1,"Failed to dynamically allocate memory"},/* USED IN DATASTRUCT,*/
        {ERROR_CODE_2,"Error open file , procced to next input file."},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {ERROR_CODE_9,"Macro declare is invaild"},/* used in Preproc when endmacr has extra text after.*/
        {ERROR_CODE_10," Macro name invaild."},/* used in pre proccesor*/
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {ERROR_CODE_17,"Instruction name is invaild."}, /* for first pass */
        {ERROR_CODE_18,"Instruction is missing."},
        {ERROR_CODE_19,"Instruction operands amount is invaild."},
        {ERROR_CODE_20,"Instruction name is not allowed."},
        {ERROR_CODE_21,"Error found on first stage according to instruction cannot procced the second stage, moveing to the next file."}, /* used on main*/
        {ERROR_CODE_22,".data Parameters are invaild."},/* for first pass */
        {ERROR_CODE_23,".string Values in string are invaild."},/* for first pass */
        {ERROR_CODE_24,"Source operand type are not valid for the sorting method"},
        {ERROR_CODE_25,"Destination operand type are not valid for the sorting method"},
        {ERROR_CODE_26,"Instruction given operands exceeds the limit."},
        {ERROR_CODE_27,"Instruction given operands are less than required."},
        {ERROR_CODE_28,"Given operands type are not valid for instruction sort method required."},
        {ERROR_CODE_29,"Instruction should not recive any operands !."},


        {ERROR_CODE_30,"Label name is a reserved name."}, /* for first pass */
        {ERROR_CODE_31," Failed to allocate memory for new symbol"},/* for first pass */
        {ERROR_CODE_32,"File must contain valid name !\n"}, /* Used in main errors*/
        {ERROR_CODE_33,"Unkown Instruction."},/* for first pass */
        {ERROR_CODE_34,".string parameter is out of range."},/* for first pass */
        {ERROR_CODE_35,".data parameter is out of range."},/* for first pass */
        {ERROR_CODE_36,":Label name is invaild."},/* for first pass */
        {ERROR_CODE_37,"Label name is already saved."}, /* for first pass*/
        {ERROR_CODE_38,"Given operands exceeds the limit"}, /* for first pass*/
        {ERROR_CODE_39,"File contain erros , cannot procced"},/* for first pass */
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {-1,            ""},
        {ERROR_CODE_39,""},/* for first pass */
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
    file.file_name, file.line_num,file.colo, errors[error_code].error_msg);
}

