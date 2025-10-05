#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include "lista.h"
#include <math.h>

#define SUCCESS 1
#define DUPLICATE 2
#define NO_MEMORY 0

int insertVariable(tList *p, char *name, char *dataType); 
int insertString(tList *p, char *name);
char *deleteCharacter(char *lex);
int insertNumber(tList *p, char *lex);
void crearTS(tList *p);
void recorrerTS(tList *p);
const char* getTipoDatoVariable(tList* ts, const char* nombreVar);
char* resolverTipoOperacion(const char* tipo1, const char* tipo2, const char* op);
char* get_type_in_ts(tList *p, const char* nombreVar);

#endif // FUNCIONES_H_INCLUDED