#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/util.h"
#include "headers/errors.h"


void addNode(LinkedListOfMacro *macroTable , char *line) { /*createing node*/ 
    if(macroTable->head==NULL)
    {
        macroTable->head=(NodeOnList*) handle_malloc(sizeof(NodeOnList));
        macroTable->tail=macroTable->head;
        macroTable->head->name= Macro_name(line);
        macroTable->head->Macro_content = (LinkedListOfMacro_Content*) handle_malloc(sizeof(LinkedListOfMacro_Content));
        macroTable->head->Macro_content->head=NULL;
        macroTable->head->Macro_content->tail=NULL;
    }
    else
    {
        NodeOnList* newTail=(NodeOnList*) handle_malloc(sizeof(NodeOnList));
        newTail->name= Macro_name(line);
        newTail->Macro_content = (LinkedListOfMacro_Content*) handle_malloc(sizeof(LinkedListOfMacro_Content));
        newTail->Macro_content->head=NULL;
        newTail->Macro_content->tail=NULL;
        macroTable->tail->next=newTail;
        macroTable->tail=newTail;
    }
}


void add_macro_content(NodeOnList *macro, char *line)
{
    NodeOfMacroContentList* newNode = (NodeOfMacroContentList*) handle_malloc(sizeof(NodeOfMacroContentList));
    strncpy(newNode->line, line, MAX_LINE_LENGTH - 1);
    newNode->line[MAX_LINE_LENGTH - 1] = '\0'; /*Ensure null termination*/ 
    newNode->next = NULL;
    if (macro->Macro_content->head == NULL){
        macro->Macro_content->head = newNode;
        macro->Macro_content->tail=newNode;
    }
    else
    {
        macro->Macro_content->tail->next = newNode;
        macro->Macro_content->tail = newNode;
    }
}


void free_macro_content_list(LinkedListOfMacro_Content *contentList) {
    NodeOfMacroContentList *current = contentList->head;
    NodeOfMacroContentList *nextNode;

    while(current != NULL){
        nextNode = current->next;
        free(current);
        current = nextNode;
    }
}

void free_linked_list(LinkedListOfMacro *list){
    NodeOnList *temp = list->head;
    NodeOnList *nextNode;
    while(temp != NULL){
        nextNode = temp->next;
        free_macro_content_list(temp->Macro_content);
        free(temp->Macro_content);
        free(temp->name);
        free(temp);
        temp = nextNode;
    }
}

void *handle_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        print_internal_error(ERROR_CODE_1);
        exit(EXIT_FAILURE);
    }
    return ptr;
}
Symbol *symbol_table = NULL;

int add_symbol(const char *label, int address, int is_external) {
    Symbol *new_symbol;
    Symbol *current;
    char *label_copy = NULL;

    current = symbol_table;

    /* בדיקת כפילות תווית */
    while (current != NULL) {
        if (label && current->label && strcmp(current->label, label) == 0) {
            return 0; /* תווית קיימת */
        }
        current = current->next;
    }

    /* הקצאת זיכרון ל-symbol החדש */
    new_symbol = (Symbol *)malloc(sizeof(Symbol));
    if (new_symbol == NULL) {
        print_internal_error(ERROR_CODE_1);
        exit(EXIT_FAILURE);
    }

    /* העתקת מחרוזת התווית והקצאת זיכרון */
    if (label) {
        label_copy = (char *)malloc(strlen(label) + 1);
        if (label_copy == NULL) {
            print_internal_error(ERROR_CODE_1);
            exit(EXIT_FAILURE);
        }
        strcpy(label_copy, label);
    }

    /* שמירת הערכים ב-symbol החדש */
    new_symbol->label = label_copy;
    new_symbol->address = address;
    new_symbol->is_external = is_external;
    new_symbol->next = symbol_table;

    /* עדכון ראש הרשימה המקושרת */
    symbol_table = new_symbol;

    return 1;
}

/*CodeNode *code_list = NULL;  Main list */

/*void add_code_node(int address, unsigned short binary_code) {
    CodeNode *new_node = (CodeNode *)malloc(sizeof(CodeNode));
    CodeNode *current;

    if (!new_node) {
        print_internal_error(ERROR_CODE_3); // טיפול בשגיאת זיכרון
        return;
    }

    new_node->address = address;
    new_node->binary_code = binary_code;
    new_node->next = NULL;

    if (code_list == NULL) {
        code_list = new_node;
    } else {
        current = code_list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}*/






  

/*------------------- TEST ----------------------- */

void print_symbol_table(const char *filename) {
    FILE *file;
    Symbol *current; 

    file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    current = symbol_table;
    while (current) {
        fprintf(file, "%s\t%d\n", current->label, current->address);
        current = current->next;
    }

    fclose(file);
}

void free_symbol_table(void) {
    Symbol *current = symbol_table;
    while (current) {
        Symbol *to_free = current;
        current = current->next;
        free(to_free->label); 
        free(to_free);
    }
    symbol_table = NULL;
}
/*------------------- TEST ----------------------- */


