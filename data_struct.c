#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers/data_struct.h"
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


