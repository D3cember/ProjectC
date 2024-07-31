#include <stdio.h>
#include <string.h>
#include "headers/data_struct.h"
#include "headers/util.h"
#include "headers/globaldefine.h"
#include "headers/errors.h"

void first_pass(const char *filename) {
    FILE *file;
    char line[256];
    int IC = IC_START;
    int DC = DC_START;
    char *label;
    char *instruction;
    char *operand;
    char *colon_pos;
    char *p;

    file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    while (fgets(line, sizeof(line), file)) {
        /* הסרת תווים לא רצויים */
        p = strtok(line, "\n");
        if (!p) {
            continue;
        }

        /* התעלמות משורות ריקות או הערות */
        if (line[0] == ';' || line[0] == '\0') {
            continue;
        }

        /* ניתוח השורה */
        label = NULL;
        instruction = NULL;
        operand = NULL;

        /* זיהוי תווית */
        colon_pos = strchr(line, ':');
        if (colon_pos) {
            *colon_pos = '\0'; /* חיתוך התו ':' */
            label = line;
            instruction = colon_pos + 1;
            while (*instruction == ' ') instruction++; /* דילוג על רווחים */
        } else {
            instruction = line;
        }

        /* עדכון טבלת הסמלים אם יש תווית */
        if (label) {
            if (strcmp(instruction, ".data") == 0 || strcmp(instruction, ".string") == 0) {
                add_symbol(label, DC); /* סמל עבור נתונים */
            } else {
                add_symbol(label, IC + IC_BASE); /* סמל עבור הוראות */
            }
        }

        /* פיצול פקודה ואופרנדים */
        operand = strtok(instruction, " ");
        if (operand) {
            instruction = operand;
            operand = strtok(NULL, "");
        }

        /* עדכון IC ו-DC בהתאם לפקודה או לנתונים */
        if (strcmp(instruction, ".data") == 0) {
            DC += count_data_items(operand); /* פונקציה לספירת פריטי הנתונים */
        } else if (strcmp(instruction, ".string") == 0) {
            DC += strlen(operand) - 2 + 1; /* אורך המחרוזת + '\0' */
        } else {
            IC += 1; /* עבור פקודות, נניח שכל פקודה אורכה מילה אחת */
        }
    }

    fclose(file);

    /* הוספת 100 לערכים בטבלת הסמלים שאופיינו כנתונים */
    adjust_data_symbols(IC_BASE);
}

void adjust_data_symbols(int base) {
    Symbol *current = symbol_table;
    while (current) {
        if (current->address < IC_BASE) {
            current->address += base;
        }
        current = current->next;
    }
}
