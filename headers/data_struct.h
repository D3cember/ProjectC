#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include "globaldefine.h"


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

typedef struct {
    NodeOnList* head;
    NodeOnList* tail;
} LinkedListOfMacro;


struct location{
    char file;
    int line;
    int column;
};

void addNode(LinkedListOfMacro* macroTable , char *line); /*createing node*/ 
void add_macro_content(NodeOnList *macro, char *line);
void free_macro_content_list(LinkedListOfMacro_Content *macroTabble);
void free_linked_list(LinkedListOfMacro *list);
void *handle_malloc(size_t size);

#endif /*DATA_STRUCT_H*/ 


