#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"

void createList(tList *p)
{
    *p = NULL;
}

int insertOrder(tList *p, char *name, char *dataType, char *value, int length)
{
    int result = -1;
    tNode* nue = (tNode*)malloc(sizeof(tNode));
    
    if(!nue)
        return NO_MEMORY;

    while(*p && strcmp((*p)->name, name) < 0)
        p = &(*p)->next;
    if(*p && strcmp((*p)->name, name) == 0)
        return DUPLICATE;

    strcpy(nue->name, name);
    strcpy(nue->dataType, dataType);
    strcpy(nue->value, value);
    
    nue->length = length;

    nue->next = *p;

    *p = nue;

    return SUCCESS;
}