#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/data_struct.h"

int binary_to_octal(const char *binary_code) {
    int decimal = 0;
    int octal = 0;
    int i, multiplier = 1;

    for (i = strlen(binary_code) - 1; i >= 0; i--) {
        if (binary_code[i] == '1') {
            decimal += multiplier;
        }
        multiplier *= 2;
    }

    multiplier = 1;
    while (decimal != 0) {
        octal += (decimal % 8) * multiplier;
        decimal /= 8;
        multiplier *= 10;
    }

    return octal;
}

void update_externals_file(const char *src_file, const char *symbol, int ref_address) {
    char *ext_file;
    FILE *file_handle;
    char *dot;
    int name_length;

    dot = strrchr(src_file, '.');
    if (!dot) {
        printf("Bad filename: %s\n", src_file);
        return;
    }

    name_length = dot - src_file;

    ext_file = malloc(name_length + 5); 
    if (!ext_file) {
        printf("Memory allocation failed\n");
        return;
    }

    strncpy(ext_file, src_file, name_length);
    strcpy(ext_file + name_length, ".ext");

    file_handle = fopen(ext_file, "a");
    if (!file_handle) {
        printf("Can't open %s\n", ext_file);
        free(ext_file);
        return;
    }

    fprintf(file_handle, "%-5s %04d\n", symbol, ref_address);

    fclose(file_handle);
    free(ext_file);
}

void create_entry_file(const char *filename, Symbol *symbol_table) {
    FILE *entry_file;
    char *entry_filename;
    Symbol *sym;

    entry_filename = (char *)malloc(strlen(filename) + 5);
    if (!entry_filename) {
        print_internal_error(ERROR_CODE_1);
        return;
    }
    strcpy(entry_filename, filename);
    strcat(entry_filename, ".ent");

    entry_file = fopen(entry_filename, "w");
    if (!entry_file) {
        print_internal_error(ERROR_CODE_2);  
        free(entry_filename);
        return;
    }

    /* מציאת הצומת האחרון ברשימה (עם הכתובת הגבוהה ביותר) */
    sym = symbol_table;
    while (sym && sym->next != NULL) {
        sym = sym->next;
    }

    /* הדפסת הצמתים בסדר עולה מהסוף להתחלה */
    for (; sym != NULL; sym = sym->prev) {
        if (sym->is_entry) {
            fprintf(entry_file, "%s %03d\n", sym->label, sym->address);
        }
    }

    fclose(entry_file);
    free(entry_filename);
}




void create_ext_file(const char *base_filename, Symbol *symbol_table) {
    char *ext_filename;
    FILE *ext_file;
    Symbol *sym;

    /* יצירת שם הקובץ עם סיומת .ext */
    ext_filename = (char *)malloc(strlen(base_filename) + 5);  /* 4 תווים עבור ".ext" ועוד אחד ל-null terminator */
    if (!ext_filename) {
        print_internal_error(ERROR_CODE_1);  /* Error allocating memory */
        return;
    }
    strcpy(ext_filename, base_filename);
    strcat(ext_filename, ".ext");

    ext_file = fopen(ext_filename, "r");
    if (ext_file) {
        fclose(ext_file);
        ext_file = fopen(ext_filename, "a");
    } else {
        ext_file = fopen(ext_filename, "w");
    }

    if (!ext_file) {
        print_internal_error(ERROR_CODE_2);  /* Error opening file */
        free(ext_filename);
        return;
    }

    sym = symbol_table;
    while (sym != NULL) {
        if (sym->is_external) {
            fprintf(ext_file, "%s %04d\n", sym->label, sym->address);
        }
        sym = sym->next;
    }

    fclose(ext_file);
    free(ext_filename);
}

void create_ob_file(const char *filename, CodeNode *code_list, int IC, int DC) {
    FILE *ob_file;
    char *ob_filename;
    CodeNode *current;

    ob_filename = malloc(strlen(filename) + 4);
    if (!ob_filename) {
        printf("Memory allocation failed\n");
        return;
    }

    sprintf(ob_filename, "%s.ob", filename);

    ob_file = fopen(ob_filename, "w");
    if (!ob_file) {
        printf("Can't open %s\n", ob_filename);
        free(ob_filename);
        return;
    }

    fprintf(ob_file, "%5d %2d\n", IC, DC); 

    for (current = code_list; current != NULL; current = current->next) {
        int octal_value = binary_to_octal(current->binary_code);
        fprintf(ob_file, "%04d %05d\n", current->address, octal_value);
    }

    fclose(ob_file);
    free(ob_filename);
}



