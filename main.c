#include "headers/data_struct.h"
#include "headers/preproc.h"
#include "headers/errors.h"
#include "headers/first_pass.h"
#include "headers/globaldefine.h"
#include "headers/util.h"


int main(int argc , char *argv[]) {
    const char *symbol_table_filename = "symbol_table.txt"; /* FOR TEST AND DEBUG ONLY*/
    int i;
    char fileName[30];
    char fileNameAss[30];
    char fileNameAm[30];
    int error_detect = 0;
    if (argc <= 1)
    {
        print_internal_error(ERROR_CODE_12);
    }else
    {
        for (i = 1; i < argc; i++)
        {
            /* Copy the base file name without suffix from the command line arguments */
            strncpy(fileName, argv[i], sizeof(fileName) - 1);
            fileName[sizeof(fileName) - 1] = '\0';  /* Ensure null termination */            
            strcpy(fileNameAss, fileName);
            strcat(fileNameAss, AS_FILE);

            strcpy(fileNameAm, fileName);
            strcat(fileNameAm, AM_FILE);
            printf("%s\n",fileNameAss);
            printf("%s\n",fileNameAm);
            
            printf("----- Starting Pre Processor stage ------\n");
            process_file(fileNameAss,fileNameAm);
            printf("\n\n");
            printf("----- Starting First Pass ------\n");
            first_pass(fileNameAm);
            if (error_detect != 0)
            {
                print_internal_error(ERROR_CODE_21);
                continue;
            }
            /* ------ FOR TESTING --------   */
            print_symbol_table(symbol_table_filename);
            free_symbol_table();


        }
    }
}
