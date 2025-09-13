#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

#define SUCCESS 1
#define DUPLICATE 2
#define NO_MEMORY 0

typedef struct sNode
{
    char name[50];
    char dataType[50];
    char value[50];
    int  length;
    struct sNode* next;
} tNode;

typedef tNode* tList;

void createList(tList *p);
int insertOrder(tList *p, char *name, char *dataType, char *value, int length);

#endif // LISTA_H_INCLUDED
