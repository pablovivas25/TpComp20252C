#include <stdio.h>   // Para printf, fprintf, perror
#include <stdlib.h>  // Para malloc, free, exit, EXIT_FAILURE
#include <string.h>  // Para strdup, strlen
#include "PilaEstaticaASM.h"

//#define DEBUG_MODE
// --- Implementación de funciones ---

/**
 * @brief Inicializa una pila estática con la capacidad especificada.
 * @param pila Puntero a la estructura de la pila.
 * @param capacidad La capacidad máxima de la pila.
 */
void inicializarPila(PilaEstatica *pila, int capacidad) {
    if (capacidad <= 0) {
        fprintf(stderr, "Error: La capacidad de la pila debe ser mayor que 0.\n");
        exit(EXIT_FAILURE); // Sale del programa si la capacidad es inválida
    }
    pila->elementos = (char**)malloc(sizeof(char*) * capacidad);
    if (pila->elementos == NULL) {
        perror("Error al asignar memoria para los elementos de la pila");
        exit(EXIT_FAILURE); // Sale del programa si falla la asignación de memoria
    }
    pila->tope = -1; // La pila está vacía al principio
    pila->capacidad = capacidad;
    
    #ifdef DEBUG_MODE
        printf("DEBUG: Pila inicializada con capacidad: %d\n", capacidad);
    #endif // DEBUG_MODE
}

/**
 * @brief Libera la memoria asignada a la pila y sus elementos.
 * Debe ser llamada al finalizar el uso de la pila.
 * @param pila Puntero a la estructura de la pila.
 */
void destruirPila(PilaEstatica *pila) {
    if (pila == NULL) return;

    // Liberar la memoria de cada string apilado
    while (!estaVacia(pila)) {
        free(pop(pila)); // pop ya devuelve un puntero a la cadena liberada
    }

    // Liberar la memoria del array de punteros
    free(pila->elementos);
    pila->elementos = NULL; // Evitar punteros colgantes
    pila->capacidad = 0;
    pila->tope = -1;
    
    #ifdef DEBUG_MODE
        printf("DEBUG: Pila destruida y memoria liberada.\n");
    #endif // DEBUG_MODE
}

/**
 * @brief Verifica si la pila está vacía.
 * @param pila Puntero a la estructura de la pila.
 * @return 1 si la pila está vacía, 0 en caso contrario.
 */
int estaVacia(PilaEstatica *pila) {
    return (pila->tope == -1);
}

/**
 * @brief Verifica si la pila está llena.
 * @param pila Puntero a la estructura de la pila.
 * @return 1 si la pila está llena, 0 en caso contrario.
 */
int estaLlena(PilaEstatica *pila) {
    return (pila->tope == pila->capacidad - 1);
}

/**
 * @brief Apila un elemento (string) en la pila.
 * Asigna memoria para el string y lo copia.
 * @param pila Puntero a la estructura de la pila.
 * @param elemento La cadena de caracteres a apilar.
 */
void push(PilaEstatica *pila, const char *elemento) {
    if (estaLlena(pila)) {
        fprintf(stderr, "Error: La pila está llena. No se puede apilar '%s'.\n", elemento);
        return;
    }
    // strdup asigna memoria para el string y lo copia
    pila->elementos[++pila->tope] = strdup(elemento);
    if (pila->elementos[pila->tope] == NULL) {
        perror("Error al asignar memoria para el elemento");
        pila->tope--; // Deshacer el incremento de tope
        exit(EXIT_FAILURE);
    }
    
    #ifdef DEBUG_MODE
        printf("DEBUG: Apilado: '%s'\n", elemento);
    #endif // DEBUG_MODE
}

/**
 * @brief Desapila y retorna el elemento superior de la pila.
 * El llamador es responsable de liberar la memoria del string retornado.
 * @param pila Puntero a la estructura de la pila.
 * @return Un puntero a la cadena desapilada, o NULL si la pila está vacía.
 */
char* pop(PilaEstatica *pila) {
    if (estaVacia(pila)) {
        fprintf(stderr, "Error: La pila está vacía. No se puede desapilar.\n");
        return NULL; // Retorna NULL si la pila está vacía
    }
    char* elemento_desapilado = pila->elementos[pila->tope];
    pila->elementos[pila->tope] = NULL; // Opcional: limpiar la referencia
    pila->tope--;
    #ifdef DEBUG_MODE
        printf("DEBUG: Desapilado: '%s'\n", elemento_desapilado);
    #endif // DEBUG_MODE
    return elemento_desapilado; // Retorna el puntero al string
}

/**
 * @brief Retorna el elemento superior de la pila sin desapilarlo.
 * @param pila Puntero a la estructura de la pila.
 * @return Un puntero a la cadena superior, o NULL si la pila está vacía.
 */
char* peek(PilaEstatica *pila) {
    if (estaVacia(pila)) {
        fprintf(stderr, "Error: La pila está vacía. No hay elemento superior.\n");
        return NULL;
    }
    return pila->elementos[pila->tope];
}
