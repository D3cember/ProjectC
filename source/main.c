#include "../headers/data_struct.h"
#include "../headers/preproc.h"
#include "../headers/errors.h"
#include "../headers/first_pass.h"
#include "../headers/globaldefine.h"
#include "../headers/util.h"
#include "../headers/second_pass.h"


int main(int argc , char *argv[]) {
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
                if (error_detect > 0){
                {
                    print_internal_error(ERROR_CODE_3);
                    continue;
                }
                printf("----- End of Pre Processor stage ------\n\n");
                printf("----- Starting First pass Stage ------\n");
                first_pass(fileNameAm);
                printf("\n\n");
                if (error_detect != 0)
                {
                    print_internal_error(ERROR_CODE_21);
                    continue;
                }
                printf("----- End of First Pass Stage  ------\n\n");
                printf("----- Starting Second Pass stage ------\n");
                second_pass(fileName,fileNameAm);
                printf("----- End of Second Pass stage ------\n");
            }
        }
        return 0;
    }
}
