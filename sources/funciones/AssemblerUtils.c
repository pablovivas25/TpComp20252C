#include <stdio.h>
#include <string.h>   // For strstr, strlen, strncpy
#include <stdlib.h>   // For malloc, realloc, free
#include <stdlib.h> // Para EXIT_FAILURE
#include <ctype.h> // Para isalnum, isalpha
#include "PilaEstaticaASM.h"
#include "AssemblerUtils.h"
#include "funciones.h"

tList listaTS;

// --- Función de Hashing: DJB2 ---
// Fuente: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash_string_djb2(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    return hash;
}

/**
 * @brief Estandariza un nombre de variable para ser compatible con la sintaxis de etiquetas de ensamblador.
 *
 * Reemplaza caracteres no alfanuméricos (excepto el guion bajo) por guiones bajos.
 * Asegura que el nombre no empiece con un dígito.
 * Convierte el nombre a minúsculas (opcional, pero buena práctica para consistencia).
 *
 * @param nombre_original El nombre de la variable a estandarizar.
 * @param tipo El tipo de variable a estandarizar.
 * @return Un nuevo string con el nombre estandarizado. Debe ser liberado con free() por el llamador.
 * Retorna NULL si hay un error de asignación de memoria.
 */

// Funcion auxiliar para estandarizar el nombre de las variables de usuario
char* estandarizar_nombre_ensamblador(const char* nombre_original, VariableType tipo) {
    // Manejo de nombres nulos o vacíos
    if (nombre_original == NULL || strlen(nombre_original) == 0) {
        char* empty_name_prefix;
        if (tipo == TYPE_INT) {
            empty_name_prefix = "_int_empty";
        } else if (tipo == TYPE_FLOAT) {
            empty_name_prefix = "_float_empty";
        } else if (tipo == TYPE_STRING) {
            empty_name_prefix = "_strh_empty"; // Para string vacío o nulo
        } else {
            empty_name_prefix = "_empty_var";
        }
        char* new_name = strdup(empty_name_prefix);
        if (new_name == NULL) {
            perror("Error de asignacion de memoria para nombre de variable vacia");
        }
        return new_name;
    }

    // Si es un STRING, generamos un nombre basado en el hash
    if (tipo == TYPE_STRING) {
        unsigned long hashed_value = hash_string_djb2(nombre_original);
        // buffer_size: "_strh_" + 16 chars para hash hex (max de unsigned long) + '\0'
        // Un unsigned long es 64 bits, por lo que 16 caracteres hexadecimales.
        size_t buffer_size = strlen("_strh_") + 16 + 1;
        char* hashed_name = (char*)malloc(buffer_size);
        if (hashed_name == NULL) {
            perror("Error de asignacion de memoria para nombre hasheado");
            return NULL;
        }
        // Usamos snprintf para formatear el prefijo y el valor hexadecimal del hash
        snprintf(hashed_name, buffer_size, "_strh_%lx", hashed_value);
        return hashed_name; // Retornamos el nombre hasheado directamente
    }

    // --- Lógica de estandarización original para INT, FLOAT y UNKNOWN ---
    // (Asegurarse que este tamaño sea suficiente para los prefijos y la estandarización)
    // Longitud original + posible prefijo (_int_, _float_) + posible '_' + '\0'
    size_t prefijo_len = 0;
    if (tipo == TYPE_INT) {
        prefijo_len = strlen("_int_s_");
    } else if (tipo == TYPE_FLOAT) {
        prefijo_len = strlen("_float_s_");
    }

    // Un tamaño generoso para la mayoría de los casos de estandarización.
    // +2 para '_' inicial + '\0' o similar
    char* nombre_estandarizado = (char*)malloc(strlen(nombre_original) + prefijo_len + 5);
    if (nombre_estandarizado == NULL) {
        perror("Error de asignacion de memoria para el nombre estandarizado");
        return NULL;
    }

    int j = 0; // Índice para nombre_estandarizado

    // --- Agregar prefijo basado en el tipo (solo INT y FLOAT aquí) ---
    if (tipo == TYPE_INT) {
        if (nombre_original[0]!='-') {
            strcpy(&nombre_estandarizado[j], "_int_p_");
        } else {
            strcpy(&nombre_estandarizado[j], "_int_n_");
        }
        
        j += strlen("_int_s_");
    } else if (tipo == TYPE_FLOAT) {
        if (nombre_original[0]!='-') {
            strcpy(&nombre_estandarizado[j], "_float_p_");
        } else {
            strcpy(&nombre_estandarizado[j], "_float_n_");
        }
        j += strlen("_float_s_");
    }
    

    // --- Lógica de estandarización de caracteres ---
    int i = 0;
    /** Si la cadena original comienza con un dígito o un carácter no alfanumérico/guion_bajo,
      * añadir un '_' inicial (solo si no hay prefijo de tipo ya). isalpha controla si un char es [a-zA-Z]
      * isdigit si un char es [0-9] */
    if (j == 0 && (isdigit(nombre_original[0]) || (!isalpha(nombre_original[0]) && nombre_original[0] != '_'))) {
        nombre_estandarizado[j++] = '_';
    }

    for (i = 0; nombre_original[i] != '\0'; i++) {
        char c = nombre_original[i];
        if (isalnum(c) || c == '_') { // isalnum devuelve 0 si c es [a-zA-Z0-9]
            nombre_estandarizado[j++] = tolower(c);
        } else {
            // Reemplazar caracteres no válidos con un guion bajo,
            // evitando guiones bajos consecutivos si ya hay uno.
            if (j > 0 && nombre_estandarizado[j-1] != '_') {
                nombre_estandarizado[j++] = '_';
            }
        }
    }

    // Eliminar guiones bajos finales
    while (j > 0 && nombre_estandarizado[j-1] == '_') {
        j--;
    }
    nombre_estandarizado[j] = '\0'; // Asegurar terminador nulo

    // Manejar caso de nombre resultante solo con "_"
    if (strcmp(nombre_estandarizado, "_") == 0 && strlen(nombre_original) > 0) {
        free(nombre_estandarizado);
        if (tipo == TYPE_INT) {
            return strdup("_int_renamed_var");
        } else if (tipo == TYPE_FLOAT) {
            return strdup("_float_renamed_var");
        } else { // Para UNKNOWN (STRING ya se maneja arriba)
            return strdup("_renamed_var");
        }
    }
    return nombre_estandarizado;
}
 /*
char* estandarizar_nombre_ensamblador(const char* nombre_original, VariableType tipo) {
    // Manejo de nombres nulos o vacíos
    if (nombre_original == NULL || strlen(nombre_original) == 0) {
        char* empty_name_prefix;
        if (tipo == TYPE_INT) {
            empty_name_prefix = "_int_empty";
        } else if (tipo == TYPE_FLOAT) {
            empty_name_prefix = "_float_empty";
        } else if (tipo == TYPE_STRING) {
            empty_name_prefix = "_strh_empty"; // Para string vacío o nulo
        } else {
            empty_name_prefix = "_empty_var";
        }
        char* new_name = strdup(empty_name_prefix);
        if (new_name == NULL) {
            perror("Error de asignacion de memoria para nombre de variable vacia");
        }
        return new_name;
    }

    // Si es un STRING, generamos un nombre basado en el hash
    if (tipo == TYPE_STRING) {
        unsigned long hashed_value = hash_string_djb2(nombre_original);
        // buffer_size: "_strh_" + 16 chars para hash hex (max de unsigned long) + '\0'
        // Un unsigned long es 64 bits, por lo que 16 caracteres hexadecimales.
        size_t buffer_size = strlen("_strh_") + 16 + 1;
        char* hashed_name = (char*)malloc(buffer_size);
        if (hashed_name == NULL) {
            perror("Error de asignacion de memoria para nombre hasheado");
            return NULL;
        }
        // Usamos snprintf para formatear el prefijo y el valor hexadecimal del hash
        snprintf(hashed_name, buffer_size, "_strh_%lx", hashed_value);
        return hashed_name; // Retornamos el nombre hasheado directamente
    }

    // --- Lógica de estandarización original para INT, FLOAT y UNKNOWN ---
    // (Asegurarse que este tamaño sea suficiente para los prefijos y la estandarización)
    // Longitud original + posible prefijo (_int_, _float_) + posible '_' + '\0'
    size_t prefijo_len = 0;
    if (tipo == TYPE_INT) {
        prefijo_len = strlen("_int_s_");
    } else if (tipo == TYPE_FLOAT) {
        prefijo_len = strlen("_float_s_");
    }

    // Un tamaño generoso para la mayoría de los casos de estandarización.
    // +2 para '_' inicial + '\0' o similar
    char* nombre_estandarizado = (char*)malloc(strlen(nombre_original) + prefijo_len + 5);
    if (nombre_estandarizado == NULL) {
        perror("Error de asignacion de memoria para el nombre estandarizado");
        return NULL;
    }

    int j = 0; // Índice para nombre_estandarizado

    // --- Agregar prefijo basado en el tipo (solo INT y FLOAT aquí) ---
    if (tipo == TYPE_INT) {
        if (nombre_original[0]!='-') {
            strcpy(&nombre_estandarizado[j], "_int_p_");
        } else {
            strcpy(&nombre_estandarizado[j], "_int_n_");
        }
        
        j += strlen("_int_s_");
    } else if (tipo == TYPE_FLOAT) {
        if (nombre_original[0]!='-') {
            strcpy(&nombre_estandarizado[j], "_float_p_");
        } else {
            strcpy(&nombre_estandarizado[j], "_float_n_");
        }
        j += strlen("_float_s_");
    }
    

    // --- Lógica de estandarización de caracteres ---
    int i = 0;

    if (j == 0 && (isdigit(nombre_original[0]) || (!isalpha(nombre_original[0]) && nombre_original[0] != '_'))) {
        nombre_estandarizado[j++] = '_';
    }

    for (i = 0; nombre_original[i] != '\0'; i++) {
        char c = nombre_original[i];
        if (isalnum(c) || c == '_') { // isalnum devuelve 0 si c es [a-zA-Z0-9]
            nombre_estandarizado[j++] = tolower(c);
        } else {
            // Reemplazar caracteres no válidos con un guion bajo,
            // evitando guiones bajos consecutivos si ya hay uno.
            if (j > 0 && nombre_estandarizado[j-1] != '_') {
                nombre_estandarizado[j++] = '_';
            }
        }
    }

    // Eliminar guiones bajos finales
    while (j > 0 && nombre_estandarizado[j-1] == '_') {
        j--;
    }
    nombre_estandarizado[j] = '\0'; // Asegurar terminador nulo

    // Manejar caso de nombre resultante solo con "_"
    if (strcmp(nombre_estandarizado, "_") == 0 && strlen(nombre_original) > 0) {
        free(nombre_estandarizado);
        if (tipo == TYPE_INT) {
            return strdup("_int_renamed_var");
        } else if (tipo == TYPE_FLOAT) {
            return strdup("_float_renamed_var");
        } else { // Para UNKNOWN (STRING ya se maneja arriba)
            return strdup("_renamed_var");
        }
    }
    return nombre_estandarizado;
}*/

// Función para verificar si un string comienza con un prefijo
int startsWith(const char *str, const char *prefix) {
    // 1. Obtener la longitud del prefijo
    size_t prefix_len = strlen(prefix);
    // 2. Obtener la longitud de la cadena principal
    size_t str_len = strlen(str);

    // 3. Si la cadena principal es más corta que el prefijo,
    //    es imposible que comience con ese prefijo.
    if (str_len < prefix_len) {
        return 0; // Falso
    }

    // 4. Comparar los primeros 'prefix_len' caracteres
    //    Si son idénticos, strncmp devuelve 0.
    if (strncmp(str, prefix, prefix_len) == 0) {
        return 1; // Verdadero
    } else {
        return 0; // Falso
    }
}

int es_operador(char* elemento) {
	if (strcmp(elemento, "+")==0) {
		return 1;
	} else if (strcmp(elemento, "-")==0) {
		return 2;
	} else if (strcmp(elemento, "*")==0) {
		return 3;
	} else if (strcmp(elemento, "/")==0) {
		return 4;
	} else if (strcmp(elemento, "=:")==0) {
		return 5;
	} else if (strcmp(elemento, "write")==0) {
        return 6;
	} else if (strcmp(elemento, "CMP")==0) {
        return 7;
	} 
    // else if (strcmp(elemento, "read")==0) {
    //     return 8;
	// }
	return 0;
}

VariableType get_std_type(const char* type_in_str) {
    if (strcmp(type_in_str, "CTE_STRING")==0) {
        return TYPE_STRING;
    } else if (strcmp(type_in_str, "CTE_FLOAT")==0) {
        return TYPE_FLOAT;
    } else if (strcmp(type_in_str, "CTE_INTEGER")==0) {
        return TYPE_INT;
    }
    return TYPE_UNKNOWN;
}

char* get_jump(char* operador) {
	if (strcmp(operador, "BLT")==0) {
        return ("JB");
    } else if (strcmp(operador, "BLE")==0) {
        return ("JBE");
    } else if (strcmp(operador, "BGT")==0) {
        return ("JA");
    } else if (strcmp(operador, "BGE")==0) {
        return ("JAE");
    } else if (strcmp(operador, "BEQ")==0) {
        return ("JE");
    } else if (strcmp(operador, "BNE")==0) {
        return ("JNE");
    }
    return NULL;
}

void agregar_operando(FILE* archivo, tList *ptrTS, const char* operando) {
    if (startsWith(get_type_in_ts(ptrTS,operando), "CTE_")==0) { //FALSO
        fprintf(archivo, "\tFLD @usr_%s\n",operando);
    } else {
        fprintf(archivo, "\tFLD %s\n",estandarizar_nombre_ensamblador(operando, get_std_type(get_type_in_ts(ptrTS,operando))));
    }
}

char** obtener_reorder_list(char** polaca, int size, PilaEstatica* pilaASM) {
    int i,j=0,contArg;
    char *priOp;
    char *segOp;
    char **reorder_list= (char**)malloc(cont_fct_reord * sizeof(char*));
    char buffer[50];

    for (i=0;i<size;i++) {
        if(strcmp(polaca[i],"RORD")==0) {
            reorder_list[j] = (char*)malloc(MAX_LENGTH_RORD * sizeof(char)); // Cada string puede tener MAX_LENGTH_RORD chars
            reorder_list[j][0]='\0';
            //sprintf(reorder_list[j], "\t%-35s_%d\tDB	\"[","@sys_RORD", i); // ¡Peligro de desbordamiento!
            snprintf(buffer, 50, "@sys_RORD_%d", i);
            snprintf(reorder_list[j], MAX_LENGTH_RORD, "\t%-35s\tDB	\"[", buffer);
            i++;
            contArg=0;
            while (strcmp(polaca[i],"f@end")!=0 && i<size) {
                if(es_operador(polaca[i])) {
                    if (contArg==2) {
                        segOp=pop(pilaASM);
                        priOp=pop(pilaASM);
                        strcat(reorder_list[j],priOp);
                        strcat(reorder_list[j],polaca[i]);
                        strcat(reorder_list[j],segOp);
                        contArg=0;
                    } else {
                        priOp=pop(pilaASM);
                        strcat(reorder_list[j],polaca[i]);
                        strcat(reorder_list[j],priOp);
                    }
                    if (strcmp(polaca[i+1],",")==0) {
                        strcat(reorder_list[j],",");
                    }
                } else {
                    if (strcmp(polaca[i],",")!=0) {
                        push(pilaASM, polaca[i]);
                        contArg++;
                    }
                }
                i++;
            }
            strcat(reorder_list[j],"]\",'$'\n");
            //fprintf(archivo, "\t%-35s\tDB	%s", tmpVar, reorder_list[i]);
            j++;
        }
    }
    reorder_list[j]=NULL;
    return reorder_list;
}

// Esta es una nueva función auxiliar que necesitarías crear.
// Suponemos que tu estructura de lista tiene un campo 'value'
// para almacenar el valor de la variable.
char* getValueFromTS(tList *p, const char* name) {
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            return (*p)->value;
        }
        p = &(*p)->next;
    }
    return NULL; // No encontrado
}
// Función para obtener el tipo de dato de la tabla de símbolos
char* getTypeFromTS(tList ts, const char* name) {
    tNode* currentNode = ts;
    while (currentNode != NULL) {
        if (strcmp(currentNode->name, name) == 0) {
            return currentNode->dataType;
        }
        currentNode = currentNode->next;
    }
    return NULL;
}

int generar_assembler(tList *ptrTS, char **polaca, int rpn_size) {

     // Variables auxiliares para TRIAMAX
    float tri[2][3][2]; // 2 triángulos, 3 puntos, 2 coordenadas
    int tri_count = 0;
    int punto_count = 0;

    if (rpn_size>0) {
        printf("La polaca tiene %d ELEMENTOS\n\n",rpn_size);
    } else {
        printf("La polaca NO tiene ELEMENTOS: size %d\n\n",rpn_size);
        return EXIT_FAILURE;
    }

    /** ############################################################ */
    PilaEstatica pilaASM;
    int capacidad_pila = 1000; // Definimos una capacidad fija de 1000 elementos
    int i;
    inicializarPila(&pilaASM, capacidad_pila);

    const char *nombre_archivo = "asm/Final.asm";
    printf("Intentando abrir/crear y limpiar el archivo '%s'...\n", nombre_archivo);

    // Abrir el archivo en modo "w" (write)
    // "w" crea el archivo si no existe, o lo trunca si existe.
    FILE* archivo = fopen(nombre_archivo, "w");

    // Verificar si el archivo se abrió/creó correctamente
    if (archivo == NULL) {
        perror("Error al abrir o crear el archivo"); // Muestra un mensaje de error del sistema
        return EXIT_FAILURE; // Sale del programa con un código de error
    }

    /** BEGIN HEADER DE PROGRAMA */
    fprintf(archivo, "include macros2.asm\n"
					 "include number.asm\n\n"
					 ".MODEL  LARGE\n"
					 ".386\n"
					 ".STACK 200h\n\n"
					 "MAXTEXTSIZE equ 50\n\n"
					 ".DATA\n");

    /** CARGAR LA DEFINICION DE DATOS - SE LEE DESDE LA TS - TABLA DE SIMBOLOS */
    fprintf(archivo, "; definicion de constantes float y enteras\n");
    tList *tmpTS = ptrTS;
    while (*tmpTS) {
        if (strcmp((*tmpTS)->dataType, "CTE_FLOAT")==0) {
#ifdef DEBUG_MODE
        printf("DEBUG: GENERADO %s value(%s), type(%s)\n", (*tmpTS)->name, (*tmpTS)->value, (*tmpTS)->dataType);
#endif // DEBUG_MODE
            
            fprintf(archivo, "\t%-35s	DD	%s\n", estandarizar_nombre_ensamblador((*tmpTS)->value, TYPE_FLOAT),(*tmpTS)->value);
        } else if (strcmp((*tmpTS)->dataType, "CTE_INTEGER")==0) {
#ifdef DEBUG_MODE
        printf("DEBUG: GENERADO %s value(%s), type(%s)\n", (*tmpTS)->name, (*tmpTS)->value, (*tmpTS)->dataType);
#endif // DEBUG_MODE
            fprintf(archivo, "\t%-35s	DD	%s.0\n", estandarizar_nombre_ensamblador((*tmpTS)->value, TYPE_INT),(*tmpTS)->value);
        }
        tmpTS = &(*tmpTS)->next;
    }
      fprintf(archivo, "\t%-35s	DD	%s\n", "_medio","0.5");
      fprintf(archivo, "\t%-35s	DD	%s\n", "_area1","?");
      fprintf(archivo, "\t%-35s	DD	%s\n", "_area2","?");
      fprintf(archivo, "\t%-35s	DD	%s\n", "_areaMax","?");
    fprintf(archivo, "\n"); // FIN DE CONSTANTES ENTERAS Y FLOTANTES

    /** CONSTANTES STRING */
    fprintf(archivo, "; definicion de constantes string\n");
    tmpTS = ptrTS;
    char *tmpVar;
    char varName[80] = "";
    while (*tmpTS) {
        
        if (strcmp((*tmpTS)->dataType, "CTE_STRING")==0) {  
#ifdef DEBUG_MODE
        printf("DEBUG: ESTE ES %s value(%s), type(%s)\n", (*tmpTS)->name, (*tmpTS)->value, (*tmpTS)->dataType);
#endif // DEBUG_MODE

            varName[0]='"';
            varName[1]='\0';
            strcat(varName, (*tmpTS)->value);
            strcat(varName, "\"");
            tmpVar=estandarizar_nombre_ensamblador(varName, TYPE_STRING);
            fprintf(archivo, "\t%-35s\tDB  %s,'$'\n", tmpVar, varName);
            fprintf(archivo, "\ts@%-32s\tEQU ($ - %s)\n", tmpVar, tmpVar);
        } 
        tmpTS = &(*tmpTS)->next;
    }
    fprintf(archivo, "\n"); // FIN DE CONSTANTES STRING

    /** DECLARACIÓN DE STRING DE REORDER */
    char **reorder_list=obtener_reorder_list(polaca, rpn_size, &pilaASM);
    i=0;
    while (reorder_list[i] != NULL) {
        fprintf(archivo, "%s", reorder_list[i]);
        i++;
    }

    fprintf(archivo, "\n");

    // Declarar temporales para cada NCALC encontrado en la polaca
    for (i = 0; i < rpn_size; i++) {
        if (polaca[i] != NULL && strncmp(polaca[i], "NCALC_", 6) == 0) {
            int nargs = 0;
            sscanf(polaca[i], "NCALC_%d", &nargs);
            fprintf(archivo, "\t_ncalc_args_%d\tDD\t%d DUP(?)\n", i, nargs);
            fprintf(archivo, "\t_ncalc_temp_%d\tDD\t?\n", i);
            fprintf(archivo, "\t_ncalc_sum_%d\tDD\t0.0\n", i);
            fprintf(archivo, "\t_ncalc_prod_%d\tDD\t1.0\n", i);
            fprintf(archivo, "\t_ncalc_count_%d\tDD\t0\n", i);
            fprintf(archivo, "\t_ncalc_res_%d\tDD\t0.0\n", i);
        }
    }
    fprintf(archivo, "\n"); // separación

    fprintf(archivo, "; definicion de variables\n");
    tmpTS = ptrTS;
    while (*tmpTS) {
        if (strcmp((*tmpTS)->dataType, "STRING")==0) {           
#ifdef DEBUG_MODE
        printf("DEBUG: ESTa ES %s type(%s)\n", (*tmpTS)->name, (*tmpTS)->dataType);
#endif // DEBUG_MODE
            fprintf(archivo, "\t@usr_%-30s\tDB	MAXTEXTSIZE dup (?),'$'\n", (*tmpTS)->name); /** DECLARACIÓN DE Asignación a STRING*/
        } else if (strcmp((*tmpTS)->dataType, "FLOAT")==0 || strcmp((*tmpTS)->dataType, "INTEGER")==0) {
#ifdef DEBUG_MODE
        printf("DEBUG: ESTa ES %s type(%s)\n", (*tmpTS)->name, (*tmpTS)->dataType);
#endif // DEBUG_MODE
            fprintf(archivo, "\t@usr_%-30s\tDD  ?\n", (*tmpTS)->name);
        }
        tmpTS = &(*tmpTS)->next;
    }

    /** Empezar a trabajar sobre la polaca */
    fprintf(archivo, "\n.CODE\n");
    fprintf(archivo, "START:\n"
            "\tMOV AX,@DATA\n"
            "\tMOV DS,AX\n"
            "\tMOV ES,AX\n"
            "\tFINIT; Inicializa el coprocesador\n");
	int salto=-1;
	int codeOperador;
    tmpTS = ptrTS;
    for (i=0; i<rpn_size; i++) {
        // Escribir algo en el archivo
		if (salto==i) {
			fprintf(archivo, "TAG_%d:\n",i);
			salto=-1;
		}
		codeOperador=es_operador(polaca[i]);
        if (codeOperador) {
			char *priOp;
			char *segOp;
			if (codeOperador == 5) { // operador de asignación '=:'
				priOp=pop(&pilaASM);
				segOp=pop(&pilaASM);
				if (strcmp(get_type_in_ts(tmpTS,priOp), "STRING")==0) {
                    /** Forma de asignar una constante a una variabla
                      * assignToString _cte_str, @usr_b, s@_cte_str: macro en macros2.asm
                      * _cte_str: constante declarada en el espacio de datos
                      * @usr_b: variable de usuario a la que se asigna el string
                      * s@_cte_str: size de la constante string, necesario para realizar la asignacion. */
                    tmpVar=estandarizar_nombre_ensamblador(segOp, get_std_type(get_type_in_ts(tmpTS,segOp)));
                    fprintf(archivo, "\tassignToString %s, @usr_%s, s@%s\n",tmpVar,priOp,tmpVar);
				} else {
				    if (strcmp(segOp, "@tmp")==0) {
                        fprintf(archivo, "\tFSTP @usr_%s\n",priOp);
				    } else {
                        fprintf(archivo, "\tFLD %s\n",estandarizar_nombre_ensamblador(segOp,get_std_type(get_type_in_ts(tmpTS,segOp))));
                        fprintf(archivo, "\tFSTP @usr_%s\n",priOp);
                        // fprintf(archivo, "\tFFREE\n"); // no es necesario
				    }
				}
			} else if (codeOperador>=1&&codeOperador<=4) { // si son los operadores '+', '-', '*' y '/'
			    segOp=pop(&pilaASM);
			    priOp=pop(&pilaASM);
			    if (strcmp(priOp, "@tmp")==0) {
                    //fprintf(archivo, "\tFLD @usr_%s\n",segOp);
                    agregar_operando(archivo, tmpTS, segOp);
			    } else {
            
                    agregar_operando(archivo, tmpTS, priOp);
                    agregar_operando(archivo, tmpTS, segOp);
			    }
			    switch(codeOperador) {
                    case 1: fprintf(archivo, "\tFADD\n"); break;
                    case 2: fprintf(archivo, "\tFSUB\n"); break;
                    case 3: fprintf(archivo, "\tFMUL\n"); break;
                    case 4: fprintf(archivo, "\tFDIV\n"); break;
                    default:
                        printf("operación desconocida!");
                        exit(1);
                }
                push(&pilaASM, "@tmp");
			} else if (strcmp(polaca[i], "write")==0) {
				priOp=pop(&pilaASM);
				char *var_type=get_type_in_ts(tmpTS,priOp);
                //printf("obtuve (%s) para la variable %s\n", var_type, priOp);
				if (strcmp(var_type, "STRING")==0) {
                    fprintf(archivo, "\tdisplayString @usr_%s\n\tnewLine 1\n",priOp);	// standardize var.
				} else if (strcmp(var_type, "FLOAT")==0) {
                    fprintf(archivo, "\tDisplayFloat @usr_%s,2\n\tnewLine 1\n",priOp);	// standardize var.
				} else if (strcmp(var_type, "INTEGER")==0) {
                    fprintf(archivo, "\tDisplayInteger @usr_%s\n\tnewLine 1\n",priOp);	// standardize var.
				} else {
                    fprintf(archivo, "\tdisplayString %s\n\tnewLine 1\n",estandarizar_nombre_ensamblador(priOp, get_std_type(get_type_in_ts(tmpTS,priOp))));	// standardize var.
                }

			} 
            else if (strcmp(polaca[i], "CMP")==0) {
			    segOp=pop(&pilaASM);
			    priOp=pop(&pilaASM);
			    /** ; OPERACIONES AL LEER CMP
                FLD	x	; ST(0) = x
                FLD y	; ST(0) = y ST(1) = X

                FXCH	; intercambio 0 y 1
                FCOM	; compara ST(0) con ST(1)
                FSTSW AX	; mueve los bits c3 y c0 a FLAGS
                SAHF*/
                agregar_operando(archivo, tmpTS, priOp);
                agregar_operando(archivo, tmpTS, segOp);

				fprintf(archivo, "\tFXCH\n\tFCOM\n\tFSTSW AX\n\tSAHF\n");
				i++; // me posiciono en el siguiente elemento
				char* tipo_salto = get_jump(polaca[i]);
				i++;
				fprintf(archivo, "\t%s TAG_%s\n",tipo_salto,polaca[i]);
				salto=atoi(polaca[i]);
			} else {
				priOp=pop(&pilaASM);
				segOp=pop(&pilaASM);
				fprintf(archivo, "\t%s\n",priOp);
			}
            
        } else if (strcmp(polaca[i], "BI")==0) {
			i++;
			fprintf(archivo, "\tJMP TAG_%s\n",polaca[i]);
			salto=atoi(polaca[i]);
			fprintf(archivo, "TAG_%d:\n",i+1);
		} else if (strcmp(polaca[i], "ET")==0) {
            fprintf(archivo, "TAG_%d:\n",i);
        } else if (strcmp(polaca[i], "RORD")==0) {
            fprintf(archivo, "\tdisplayString @sys_RORD_%d\n\tnewLine 1\n",i);
            for(;strcmp(polaca[i],"f@end")!=0;i++) ;
        } else if (polaca[i] != NULL && strncmp(polaca[i], "NCALC_", 6) == 0) {
            int nargs = 0;
            sscanf(polaca[i], "NCALC_%d", &nargs);

            // Sacar (pop) los argumentos de la pila ASM en orden correcto
            char **args = (char**) malloc(sizeof(char*) * nargs);
            int k;
            for (k = nargs - 1; k >= 0; k--) {
                args[k] = pop(&pilaASM); // pop devuelve el token (ID o constante)
            }

            // 1) Guardar todos los argumentos en el array temporal (_ncalc_args_i)
            for (k = 0; k < nargs; k++) {
                // cargar el operando en FPU y guardarlo en la posición k del array
                agregar_operando(archivo, tmpTS, args[k]); // emite "FLD <operando>"
                fprintf(archivo, "\tFSTP DWORD PTR _ncalc_args_%d + %d\n", i, 4 * k); // almacena y pop
            }

            // 2) Recorrer el array y para cada elemento:
            //    - cargar en temp, inspeccionar el bit de signo (mov->test)
            //    - si negativo: sumar en _ncalc_sum_i, multiplicar en _ncalc_prod_i, inc _ncalc_count_i
            for (k = 0; k < nargs; k++) {
                fprintf(archivo, "\tFLD DWORD PTR _ncalc_args_%d + %d\n", i, 4 * k);
                fprintf(archivo, "\tFSTP DWORD PTR _ncalc_temp_%d\n", i);            // temp = arg
                fprintf(archivo, "\tMOV EAX, DWORD PTR _ncalc_temp_%d\n", i);       // mov bits
                fprintf(archivo, "\tTEST EAX, 80000000h\n");                        // check sign bit
                fprintf(archivo, "\tJZ NCALC_%d_NOTNEG_%d\n", i, k);

                // es negativo -> sumNeg = sumNeg + temp
                fprintf(archivo, "\tFLD DWORD PTR _ncalc_sum_%d\n", i);
                fprintf(archivo, "\tFLD DWORD PTR _ncalc_temp_%d\n", i);
                fprintf(archivo, "\tFADD\n");
                fprintf(archivo, "\tFSTP DWORD PTR _ncalc_sum_%d\n", i);

                // prodNeg = prodNeg * temp
                fprintf(archivo, "\tFLD DWORD PTR _ncalc_prod_%d\n", i);
                fprintf(archivo, "\tFLD DWORD PTR _ncalc_temp_%d\n", i);
                fprintf(archivo, "\tFMUL\n");
                fprintf(archivo, "\tFSTP DWORD PTR _ncalc_prod_%d\n", i);

                // contador de negativos
                fprintf(archivo, "\tINC DWORD PTR _ncalc_count_%d\n", i);

                fprintf(archivo, "NCALC_%d_NOTNEG_%d:\n", i, k);
            }

            // 3) Decidir resultado según cantidad de negativos
            fprintf(archivo, "\tMOV EAX, DWORD PTR _ncalc_count_%d\n", i);
            fprintf(archivo, "\tCMP EAX, 0\n");
            fprintf(archivo, "\tJE NCALC_%d_ZERO\n", i);
            fprintf(archivo, "\tTEST EAX, 1\n");                      // par/impar
            fprintf(archivo, "\tJZ NCALC_%d_EVEN\n", i);
            // impar -> producto
            fprintf(archivo, "\tFLD DWORD PTR _ncalc_prod_%d\n", i);
            fprintf(archivo, "\tJMP NCALC_%d_END\n", i);
            // par -> suma
            fprintf(archivo, "NCALC_%d_EVEN:\n", i);
            fprintf(archivo, "\tFLD DWORD PTR _ncalc_sum_%d\n", i);
            fprintf(archivo, "\tJMP NCALC_%d_END\n", i);
            // 0 negativos -> 0.0
            fprintf(archivo, "NCALC_%d_ZERO:\n", i);
            fprintf(archivo, "\tFLDZ\n");
            fprintf(archivo, "NCALC_%d_END:\n", i);

            // Guardar el resultado en una variable temporal (para poder imprimirlo)
            fprintf(archivo, "\tFSTP DWORD PTR _ncalc_res_%d\n", i);

            // Mostrar en consola el resultado
            fprintf(archivo, "\tDisplayFloat _ncalc_res_%d, 2\n", i);
            fprintf(archivo, "\tnewLine 1\n");

            // Volver a cargarlo en FPU si querés que siga en la pila lógica
            fprintf(archivo, "\tFLD DWORD PTR _ncalc_res_%d\n", i);

            // Dejar resultado en FPU y representar por @tmp en la pila lógica
            push(&pilaASM, "@tmp");

            // liberar memoria auxiliar
            free(args);
        }else if(polaca[i] != NULL && strcmp(polaca[i], "TRIAMAX")==0){
          
             fprintf(archivo, "; --- CALCULO AREA TRIANGULO 1 Y 2 ---\n");
            
            char *tri_vals[12];
            char *tri_vals_aux[12];
            int tri_vals_count = 0;
            
            // Pop de 6 valores para el segundo triangulo y luego 6 para el primero
            int k;
            int l=0;
             for(k = 0; k < 22; k++) {
                char* token = pop(&pilaASM);
             //   printf("DEBUG: Comparando con toekn '%s'\n", token);

                if(strcmp(token,"f@end")!=0 && strcmp(token,"VERTEXLIST")!=0 && strcmp(token,"PUNTO")!=0 && strcmp(token,"VERTEXLIST")!=0 && strcmp(token,"=")!=0 && token!=NULL){
                    char* standardized_name;
                    char* var_type = get_type_in_ts(ptrTS, token);
                    
                    if (var_type != NULL) {
                        standardized_name = estandarizar_nombre_ensamblador(token, get_std_type(var_type));
                    } else {
                        standardized_name = estandarizar_nombre_ensamblador(token, TYPE_FLOAT);
                    }

                    tri_vals[l] = standardized_name;
                    l++;
                }
              
            }
            k=0;
            for(l=12 - 1; l >= 0; l--){
                tri_vals_aux[k]=tri_vals[l];
               k++;
            }

            
            // Calculo de área para el primer triángulo
            // Calculo de área para el primer triángulo
fprintf(archivo, "; --- CALCULO AREA TRIANGULO 1 ---\n");
// Formula: 0.5 * |x1(y2 - y3) + x2(y3 - y1) + x3(y1 - y2)|
// Se asume que los valores en tri_vals están en el orden: x1, y1, x2, y2, x3, y3
// del primer triangulo y luego los del segundo.

// Termino 1: x1(y2-y3)
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[3]); // y2
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[5]); // y3
fprintf(archivo, "\tFSUB\n");               // y2 - y3
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[0]); // x1
fprintf(archivo, "\tFMUL\n");               // x1 * (y2 - y3)
fprintf(archivo, "\tFSTP _area1\n");        // Guarda resultado temporalmente

// Termino 2: x2(y3-y1)
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[5]); // y3
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[1]); // y1
fprintf(archivo, "\tFSUB\n");               // y3 - y1
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[2]); // x2
fprintf(archivo, "\tFMUL\n");               // x2 * (y3 - y1)
fprintf(archivo, "\tFLD _area1\n");         // Carga el resultado del Termino 1
fprintf(archivo, "\tFADD\n");               // Suma los terminos 1 y 2
fprintf(archivo, "\tFSTP _area1\n");        // Guarda el resultado acumulado

// Termino 3: x3(y1-y2)
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[1]); // y1
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[3]); // y2
fprintf(archivo, "\tFSUB\n");               // y1 - y2
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[4]); // x3
fprintf(archivo, "\tFMUL\n");               // x3 * (y1 - y2)
fprintf(archivo, "\tFLD _area1\n");         // Carga el resultado acumulado
fprintf(archivo, "\tFADD\n");               // Suma el Termino 3

// Pasos finales
fprintf(archivo, "\tFABS\n");
fprintf(archivo, "\tFMUL _medio\n");
fprintf(archivo, "\tFSTP _area1\n\n");


            

// Calculo de área para el segundo triángulo
fprintf(archivo, "; --- CALCULO AREA TRIANGULO 2 ---\n");
// Se asume que los valores de tri_vals[6] a tri_vals[11] corresponden
// a las coordenadas del segundo triangulo.
// Termino 1: x1(y2-y3)
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[9]); // y2
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[11]);// y3
fprintf(archivo, "\tFSUB\n");               // y2 - y3
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[6]); // x1
fprintf(archivo, "\tFMUL\n");               // x1 * (y2 - y3)
fprintf(archivo, "\tFSTP _area2\n");        // Guarda resultado temporalmente

// Termino 2: x2(y3-y1)
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[11]);// y3
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[7]); // y1
fprintf(archivo, "\tFSUB\n");               // y3 - y1
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[8]); // x2
fprintf(archivo, "\tFMUL\n");               // x2 * (y3 - y1)
fprintf(archivo, "\tFLD _area2\n");         // Carga el resultado del Termino 1
fprintf(archivo, "\tFADD\n");               // Suma los terminos 1 y 2
fprintf(archivo, "\tFSTP _area2\n");        // Guarda el resultado acumulado

// Termino 3: x3(y1-y2)
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[7]); // y1
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[9]); // y2
fprintf(archivo, "\tFSUB\n");               // y1 - y2
fprintf(archivo, "\tFLD %s\n", tri_vals_aux[10]);// x3
fprintf(archivo, "\tFMUL\n");               // x3 * (y1 - y2)
fprintf(archivo, "\tFLD _area2\n");         // Carga el resultado acumulado
fprintf(archivo, "\tFADD\n");               // Suma el Termino 3



            
   
// Pasos finales
fprintf(archivo, "\tFABS\n");
fprintf(archivo, "\tFMUL _medio\n");
fprintf(archivo, "\tFSTP _area2\n");
fprintf(archivo, "\tFLD _area1\n");
fprintf(archivo, "\tFLD _area2\n");
fprintf(archivo, "\tFCOM\n");
fprintf(archivo, "\tFSTSW AX\n");
fprintf(archivo, "\tSAHF\n");
fprintf(archivo, "\tJGE AREA1_MAYOR\n");
fprintf(archivo, "\tFLD _area2\n");
fprintf(archivo, "\tFSTP _areaMax\n");
fprintf(archivo, "\tJMP FIN_AREAS\n");
fprintf(archivo, "AREA1_MAYOR:\n");
fprintf(archivo, "\tFLD _area1\n");
fprintf(archivo, "\tFSTP _areaMax\n");
fprintf(archivo, "FIN_AREAS:\n");
fprintf(archivo, "\tDisplayFloat _areaMax, 2\n");
fprintf(archivo, "\t newLine 1\n");

        }else {

            push(&pilaASM, polaca[i]);
        }
        fflush(archivo);
    }
    if (salto==i) {
        fprintf(archivo, "TAG_%d:\n",i);
        salto=-1;
    }

    fprintf(archivo,"\nFINAL:\n"
                    "\tMOV ah, 1 ; pausa, espera que oprima una tecla\n"
                    "\tINT 21h ; AH=1 es el servicio de lectura\n"
                    "\tMOV AX, 4C00h ; Sale del Dos\n"
                    "\tINT 21h       ; Enviamos la interripcion 21h\n"
                    "END START ; final del archivo.\n");

    printf("Ensamble Completo generado exitosamente en '%s'.\n", nombre_archivo);

    // Es crucial cerrar el archivo para guardar los cambios y liberar recursos
    fclose(archivo);
    printf("Archivo cerrado. Puedes verificar su contenido.\n");
    return EXIT_SUCCESS;
}

// Helper function to convert string dataType to VariableType enum
VariableType getVariableType(const char* dataType) {
    if (strcmp(dataType, "INTEGER") == 0 || strcmp(dataType, "CTE_INTEGER") == 0) {
        return TYPE_INT;
    }
    if (strcmp(dataType, "FLOAT") == 0 || strcmp(dataType, "CTE_FLOAT") == 0) {
        return TYPE_FLOAT;
    }
    if (strcmp(dataType, "STRING") == 0 || strcmp(dataType, "CTE_STRING") == 0) {
        return TYPE_STRING;
    }
    return TYPE_UNKNOWN;
}
