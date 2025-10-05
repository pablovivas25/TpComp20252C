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
    TYPE_UNKNOWN,
    TYPE_CTE_INT,    // constante int
    TYPE_CTE_FLOAT,  // constante float
} VariableType;

int generar_assembler(tList *ptrTS, char **polaca, int rpn_size);

#endif // ASSEMBLERUTILS_H_INCLUDED
