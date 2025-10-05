#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PILA_LLENA  0
#define PILA_VACIA  0
#define TODO_OK     1
#define MAX_ELEMENTS 1000

#define SENTIDO_IZQ 1
#define SENTIDO_DER 0

typedef struct {
    char *vector_elements[MAX_ELEMENTS];
    int vector_index;
    int index_stack[MAX_ELEMENTS];
    int stack_top;
} T_RPN;

T_RPN *rpn = NULL;

void init_polaca();
void mostrar_polaca();
int insertar_en_polaca(char *);
int actualizar_polaca(int, int);
int actualizar_elemento_en_polaca(int, char *);
int invertir_condicion(int);
int avanzar_polaca();
int posicion_polaca_actual();
int reordenar_polaca(int, int, int);
char* buscar_en_polaca(char *);
int apilar_indice(int);
int desapilar_indice(int *);
int ver_tope_de_pila(int *);
int pila_vacia();
int pila_llena();

/* ************************************************** */
/* *********************** RPN ********************** */
/* ************************************************** */

void init_polaca() {
    printf("Starting Reverse Polish Notation...\n");

    rpn = (T_RPN *) malloc (sizeof (T_RPN));
    rpn->stack_top=0;
    rpn->vector_index=0;
}

int insertar_en_polaca(char *value) {
    rpn->vector_elements[rpn->vector_index] = (char *) malloc (strlen (value) + 1);
    strcpy(rpn->vector_elements[rpn->vector_index], value);
    rpn->vector_index++;
    return rpn->vector_index;
}
void eliminar_ultimo_de_polaca() {
    if (rpn->vector_index > 0) {
        rpn->vector_index--;
        free(rpn->vector_elements[rpn->vector_index]);
        rpn->vector_elements[rpn->vector_index] = NULL;
    }
}


int actualizar_polaca(int index, int offset) {
    char jumpPos[4];
    sprintf(jumpPos,"%d",rpn->vector_index + offset);
    rpn->vector_elements[index] = (char *) malloc (strlen (jumpPos) + 1);
    strcpy(rpn->vector_elements[index], jumpPos);
    return TODO_OK;
}

int actualizar_elemento_en_polaca(int index, char *value) {
    rpn->vector_elements[index] = (char *) malloc (strlen (value) + 1);
    strcpy(rpn->vector_elements[index], value);
    return TODO_OK;
}

char* obtener_opuesto(char *operador) {
    if (strcmp(operador, "BLT")==0) {
        return ("BGE");
    } else if (strcmp(operador, "BLE")==0) {
        return ("BGT");
    } else if (strcmp(operador, "BGT")==0) {
        return ("BLE");
    } else if (strcmp(operador, "BGE")==0) {
        return ("BLT");
    } else if (strcmp(operador, "BEQ")==0) {
        return ("BNE");
    } else if (strcmp(operador, "BNE")==0) {
        return ("BEQ");
    } 
}

int invertir_condicion(int index) {
    strcpy(rpn->vector_elements[index], obtener_opuesto(rpn->vector_elements[index]));
    return 0;
}

int avanzar_polaca() {
    rpn->vector_elements[rpn->vector_index]='\0';
    rpn->vector_index++;
    return rpn->vector_index;
}

int posicion_polaca_actual() {
    return rpn->vector_index;
}

int reordenar_polaca(int posPivot, int cantArgREORDER, int sentido) {
    if (sentido==SENTIDO_IZQ && posPivot==0) {
        return 0; // NADA QUE ORDENAR
    }
    if (sentido==SENTIDO_DER && posPivot==cantArgREORDER) {
        return 0; // NADA QUE ORDENAR
    }

    int posIni, posFin, posExpIni, posExpFin, tmpIndex;
    desapilar_indice(&posFin);
    posFin=posFin-2;
    posExpFin=posFin;
    while(cantArgREORDER>=posPivot) {
        desapilar_indice(&tmpIndex);
        
        if (cantArgREORDER==posPivot) {
            posExpIni=tmpIndex;
        } else {
            posExpFin=tmpIndex-1;
        }
        cantArgREORDER--;
    }
    while (cantArgREORDER>0) {
        desapilar_indice(&tmpIndex);
        cantArgREORDER--;
    }
    desapilar_indice(&posIni);

    int totalElementos=(posFin-posIni)+2;
    char **elementosArg=(char**) malloc (totalElementos*sizeof (char*));
    int i,j=0;
    if (sentido==SENTIDO_IZQ) {
        for(i=posExpIni;i<=posExpFin;i++) {
            elementosArg[j]=rpn->vector_elements[i];
            j++;
        }
        //printf("     ante-ultimo-elemento \"%s\"\n",elementosArg[j-1]);
        if (strcmp(elementosArg[j-1], ",")!=0) {
            elementosArg[j] = ",";
            j++;
        }
        for(i=posIni;i<=posFin;i++) {
            if (i<posExpIni||i>posExpFin) {
                elementosArg[j]=rpn->vector_elements[i];
                j++;
            }
        }
    } else if (sentido==SENTIDO_DER) {
        for(i=posIni;i<=posFin;i++) {
            if (i<posExpIni||i>posExpFin) {
                elementosArg[j]=rpn->vector_elements[i];
                j++;
            }
        }
        //printf("     ante-ultimo-elemento \"%s\"\n",elementosArg[j-1]);
        if (strcmp(elementosArg[j-1], ",")!=0) {
            elementosArg[j] = ",";
            j++;
        }
        for(i=posExpIni;i<=posExpFin;i++) {
            elementosArg[j]=rpn->vector_elements[i];
            j++;
        }
    }
    j=0;
    for (i=posIni;i<=posFin;i++) {
        rpn->vector_elements[i]=elementosArg[j];
        j++;
    }
    return 0;
}

char* buscar_en_polaca(char *var) { // implementar busqueda de la variable en el vector
    int lastPosition=rpn->vector_index-1;
    while(lastPosition>=0) {
        if (rpn->vector_elements[lastPosition]!='\0'&&strcmp(rpn->vector_elements[lastPosition], var)==0) {
            return (rpn->vector_elements[lastPosition-1]);
        }
        lastPosition--;
    }
    return NULL;
}

int apilar_indice(int index) {
    if (rpn->stack_top==MAX_ELEMENTS) {
        return PILA_LLENA; 
    }
    rpn->index_stack[rpn->stack_top] = index;
    rpn->stack_top++;
    return TODO_OK;
}

int desapilar_indice(int *value) {
    if (rpn->stack_top==0) {
        return PILA_VACIA;
    }
    rpn->stack_top--;
    *value = rpn->index_stack[rpn->stack_top];
    return TODO_OK;
}

int ver_tope_de_pila(int *value) {
    if (rpn->stack_top==0) {
        return PILA_VACIA;
    }
    *value = rpn->index_stack[rpn->stack_top-1];
    return TODO_OK;
}

int pila_vacia() {
    return rpn->stack_top==0; 
}

int pila_llena() {
    return rpn->stack_top==MAX_ELEMENTS;
}

void mostrar_polaca() {
    int i;
    FILE* p_file=fopen("intermediate-code.txt","w+");
    if(!p_file) {
        printf("Error al crear el archivo intermedio.\n");
        return;
    }
    // fprintf(p_file , "[%3d] -> %s" , i, rpn->vector_elements[i]);
    for (i=0; i<rpn->vector_index; i++) {
        fprintf(p_file, "[%03d] -> %s\n", i, rpn->vector_elements[i]);
        printf("[%03d] -> %s\n", i, rpn->vector_elements[i]);
        // printf("\n");
    }
    printf("\n");
    fprintf(p_file, "\n");
    fclose(p_file);
}

