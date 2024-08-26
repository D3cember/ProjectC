#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include "globaldefine.h"
#include "errors.h"

typedef struct NodeOfMacroContentList {
    char line[MAX_LINE_LENGTH];
    struct NodeOfMacroContentList* next;
} NodeOfMacroContentList;

typedef struct {
    NodeOfMacroContentList* head;
    NodeOfMacroContentList* tail;
} LinkedListOfMacro_Content;

typedef struct NodeOnList {
    char* name;
    struct NodeOnList* next;
    LinkedListOfMacro_Content* Macro_content;
} NodeOnList;

typedef struct LinkedListOfMacro{ 
    NodeOnList* head;
    NodeOnList* tail;
} LinkedListOfMacro;


typedef struct Symbol {
    char *label;
    int address;
    int is_external;
    int is_entry;
    struct Symbol *next;
} Symbol;

extern Symbol *symbol_table;

typedef struct CodeNode {
    int address;
    char binary_code[MAX_BINARY_LENGTH]; /* שמירת המחרוזת הבינארית */
    struct CodeNode *next;
} CodeNode;



void addNode(LinkedListOfMacro* macroTable , char *line); /*createing node*/ 
void add_macro_content(NodeOnList *macro, char *line);
void free_macro_content_list(LinkedListOfMacro_Content *macroTabble);
void free_linked_list(LinkedListOfMacro *list);
void *handle_malloc(size_t size);
int add_symbol(const char *label, int address, int is_external, int is_entry);
void print_symbol_table(const char *filename);
void free_symbol_table(void);
void add_code_node(int address, const char *binary_code, CodeNode **code_list);
void print_binary_code_list(FILE *file);





#endif 

