#ifndef ASSEMBLERUTILS_H_INCLUDED
#define ASSEMBLERUTILS_H_INCLUDED
#include "lista.h"

#define MAX_LENGTH_RORD 100

extern int cont_fct_reord;

// --- Nueva enumeración para Tipos de Variable ---
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_UNKNOWN
} VariableType;

int generar_assembler(tList *ptrTS, char **polaca, int rpn_size);
VariableType getVariableType(const char* dataType);
char* getTypeFromTS(tList ts, const char* name);

#endif // ASSEMBLERUTILS_H_INCLUDED
