#ifndef PILAESTATICAASM_H_INCLUDED
#define PILAESTATICAASM_H_INCLUDED

// --- Definición de la estructura de la Pila Estática ---
typedef struct {
    char** elementos; // Array de punteros a cadenas (strings)
    int tope;         // Índice del último elemento (-1 si vacía)
    int capacidad;    // Capacidad máxima de la pila
} PilaEstatica;

// --- Prototipos de funciones ---
void inicializarPila(PilaEstatica *pila, int capacidad);
void destruirPila(PilaEstatica *pila);
int estaVacia(PilaEstatica *pila);
int estaLlena(PilaEstatica *pila);
void push(PilaEstatica *pila, const char *elemento);
char* pop(PilaEstatica *pila);
char* peek(PilaEstatica *pila); // Para ver el elemento superior sin desapilar

#endif // PILAESTATICAASM_H_INCLUDED
