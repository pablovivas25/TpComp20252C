#ifndef FUNCIONES_H_INCLUDED
#define FUNCIONES_H_INCLUDED

#include "lista.h"

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

float get_calc_area(float x1,float y1,float x2,float y2,float x3,float y3);
char *nuevoTemp();
void splitTempPair(const char *pair, char *a, char *b);
#endif // FUNCIONES_H_INCLUDED