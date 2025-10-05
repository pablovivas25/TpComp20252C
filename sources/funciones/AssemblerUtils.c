#include <stdio.h>
#include <string.h> // For strstr, strlen, strncpy
#include <stdlib.h> // For malloc, realloc, free
#include <stdlib.h> // Para EXIT_FAILURE
#include <ctype.h>  // Para isalnum, isalpha
#include "PilaEstaticaASM.h"
#include "AssemblerUtils.h"
#include "funciones.h"

tList listaTS;


// --- Función de Hashing: DJB2 ---
// Fuente: http://www.cse.yorku.ca/~oz/hash.html
unsigned long hash_string_djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
    {
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
char *estandarizar_nombre_ensamblador(const char *nombre_original, VariableType tipo)
{
    // --- Caso variables de usuario ---
    // Si el nombre NO es un literal numérico ni string entrecomillado,
    // devolvemos directamente @usr_<nombre>
    if (tipo == TYPE_INT || tipo == TYPE_FLOAT) {
        // Chequear si es número
        int es_numero = 1;
        int i;
        for (i = 0; nombre_original[i] != '\0'; i++) {
            if (!isdigit(nombre_original[i]) && nombre_original[i] != '.' && nombre_original[i] != '-') {
                es_numero = 0;
                break;
            }
        }

        if (!es_numero) {
            // Es variable de usuario
            size_t len = strlen(nombre_original) + strlen("@usr_") + 1;
            char *nombre_usr = (char *)malloc(len);
            if (!nombre_usr) {
                perror("Error de memoria en estandarizar_nombre_ensamblador");
                exit(1);
            }
            snprintf(nombre_usr, len, "@usr_%s", nombre_original);
            return nombre_usr;
        }
    }

    // Manejo de nombres nulos o vacíos
    if (nombre_original == NULL || strlen(nombre_original) == 0)
    {
        char *empty_name_prefix;
        if (tipo == TYPE_INT)
        {
            empty_name_prefix = "_int_empty";
        }
        else if (tipo == TYPE_FLOAT)
        {
            empty_name_prefix = "_float_empty";
        }
        else if (tipo == TYPE_STRING)
        {
            empty_name_prefix = "_strh_empty"; // Para string vacío o nulo
        }
        else
        {
            empty_name_prefix = "_empty_var";
        }
        char *new_name = strdup(empty_name_prefix);
        if (new_name == NULL)
        {
            perror("Error de asignacion de memoria para nombre de variable vacia");
        }
        return new_name;
    }

    // Si es un STRING, generamos un nombre basado en el hash
    if (tipo == TYPE_STRING)
    {
        unsigned long hashed_value = hash_string_djb2(nombre_original);
        // buffer_size: "_strh_" + 16 chars para hash hex (max de unsigned long) + '\0'
        // Un unsigned long es 64 bits, por lo que 16 caracteres hexadecimales.
        size_t buffer_size = strlen("_strh_") + 16 + 1;
        char *hashed_name = (char *)malloc(buffer_size);
        if (hashed_name == NULL)
        {
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
    if (tipo == TYPE_INT)
    {
        prefijo_len = strlen("_int_s_");
    }
    else if (tipo == TYPE_FLOAT)
    {
        prefijo_len = strlen("_float_s_");
    }

    // Un tamaño generoso para la mayoría de los casos de estandarización.
    // +2 para '_' inicial + '\0' o similar
    char *nombre_estandarizado = (char *)malloc(strlen(nombre_original) + prefijo_len + 5);
    if (nombre_estandarizado == NULL)
    {
        perror("Error de asignacion de memoria para el nombre estandarizado");
        return NULL;
    }

    int j = 0; // Índice para nombre_estandarizado

    // --- Agregar prefijo basado en el tipo (solo INT y FLOAT aquí) ---
    if (tipo == TYPE_INT)
    {
        if (nombre_original[0] != '-')
        {
            strcpy(&nombre_estandarizado[j], "_int_p_");
        }
        else
        {
            strcpy(&nombre_estandarizado[j], "_int_n_");
        }

        j += strlen("_int_s_");
    }
    else if (tipo == TYPE_FLOAT)
    {
        if (nombre_original[0] != '-')
        {
            strcpy(&nombre_estandarizado[j], "_float_p_");
        }
        else
        {
            strcpy(&nombre_estandarizado[j], "_float_n_");
        }
        j += strlen("_float_s_");
    }

    // --- Lógica de estandarización de caracteres ---
    int i = 0;
    /** Si la cadena original comienza con un dígito o un carácter no alfanumérico/guion_bajo,
     * añadir un '_' inicial (solo si no hay prefijo de tipo ya). isalpha controla si un char es [a-zA-Z]
     * isdigit si un char es [0-9] */
    if (j == 0 && (isdigit(nombre_original[0]) || (!isalpha(nombre_original[0]) && nombre_original[0] != '_')))
    {
        nombre_estandarizado[j++] = '_';
    }

    for (i = 0; nombre_original[i] != '\0'; i++)
    {
        char c = nombre_original[i];
        if (isalnum(c) || c == '_')
        { // isalnum devuelve 0 si c es [a-zA-Z0-9]
            nombre_estandarizado[j++] = tolower(c);
        }
        else
        {
            // Reemplazar caracteres no válidos con un guion bajo,
            // evitando guiones bajos consecutivos si ya hay uno.
            if (j > 0 && nombre_estandarizado[j - 1] != '_')
            {
                nombre_estandarizado[j++] = '_';
            }
        }
    }

    // Eliminar guiones bajos finales
    while (j > 0 && nombre_estandarizado[j - 1] == '_')
    {
        j--;
    }
    nombre_estandarizado[j] = '\0'; // Asegurar terminador nulo

    // Manejar caso de nombre resultante solo con "_"
    if (strcmp(nombre_estandarizado, "_") == 0 && strlen(nombre_original) > 0)
    {
        free(nombre_estandarizado);
        if (tipo == TYPE_INT)
        {
            return strdup("_int_renamed_var");
        }
        else if (tipo == TYPE_FLOAT)
        {
            return strdup("_float_renamed_var");
        }
        else
        { // Para UNKNOWN (STRING ya se maneja arriba)
            return strdup("_renamed_var");
        }
    }
    return nombre_estandarizado;
}

// Función para verificar si un string comienza con un prefijo
int startsWith(const char *str, const char *prefix)
{
    // 1. Obtener la longitud del prefijo
    size_t prefix_len = strlen(prefix);
    // 2. Obtener la longitud de la cadena principal
    size_t str_len = strlen(str);

    // 3. Si la cadena principal es más corta que el prefijo,
    //    es imposible que comience con ese prefijo.
    if (str_len < prefix_len)
    {
        return 0; // Falso
    }

    // 4. Comparar los primeros 'prefix_len' caracteres
    //    Si son idénticos, strncmp devuelve 0.
    if (strncmp(str, prefix, prefix_len) == 0)
    {
        return 1; // Verdadero
    }
    else
    {
        return 0; // Falso
    }
}

int es_operador(char *e) {
    if (!strcmp(e, "+")) return 1;
    if (!strcmp(e, "-")) return 2;
    if (!strcmp(e, "*")) return 3;
    if (!strcmp(e, "/")) return 4;
    if (!strcmp(e, "=:")) return 5;
    if (!strcmp(e, ":=")) return 9;
    if (!strcmp(e, "="))  return 10;
    if (!strcmp(e, "write")) return 6;
    if (!strcmp(e, "CMP")) return 7;
    if (!strcmp(e, "read")) return 8;
    return 0;
}

VariableType get_std_type(const char *type_in_str)
{
    if (strcmp(type_in_str, "CTE_STRING") == 0)
    {
        return TYPE_STRING;
    }
    else if (strcmp(type_in_str, "CTE_FLOAT") == 0)
    {
        return TYPE_FLOAT;
    }
    else if (strcmp(type_in_str, "CTE_INTEGER") == 0)
    {
        return TYPE_INT;
    }

    else if (strcmp(type_in_str, "INTEGER") == 0)
        return TYPE_INT;
    else if (strcmp(type_in_str, "FLOAT") == 0)
        return TYPE_FLOAT;
    else if (strcmp(type_in_str, "STRING") == 0)
        return TYPE_STRING;
    return TYPE_UNKNOWN;
}

char *get_jump(char *operador)
{
    if (strcmp(operador, "BLT") == 0)
    {
        return ("JB");
    }
    else if (strcmp(operador, "BLE") == 0)
    {
        return ("JBE");
    }
    else if (strcmp(operador, "BGT") == 0)
    {
        return ("JA");
    }
    else if (strcmp(operador, "BGE") == 0)
    {
        return ("JAE");
    }
    else if (strcmp(operador, "BEQ") == 0)
    {
        return ("JE");
    }
    else if (strcmp(operador, "BNE") == 0)
    {
        return ("JNE");
    }
    return NULL;
}

void agregar_operando(FILE *archivo, tList *ptrTS, const char *operando)
{
    // Caso especial: temporales internos
    if (strcmp(operando, "@tmp") == 0 || strncmp(operando, "_ncalc_res_", 11) == 0) {
        fprintf(archivo, "\tFLD %s\n", operando);
        return;
    }

    const char *tipo = get_type_in_ts(ptrTS, operando);
    if (tipo == NULL) {
        // No está en TS, cargar tal cual
        fprintf(archivo, "\tFLD %s\n", operando);
        return;
    }

    if (startsWith(tipo, "CTE_") == 0) {
        fprintf(archivo, "\tFLD @usr_%s\n", operando);
    } else {
        fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(operando, get_std_type(tipo)));
    }
}

char **obtener_reorder_list(char **polaca, int size, PilaEstatica *pilaASM)
{
    int i, j = 0, contArg;
    char *priOp;
    char *segOp;
    char **reorder_list = (char **)malloc(cont_fct_reord * sizeof(char *));
    char buffer[50];

    for (i = 0; i < size; i++)
    {
        if (strcmp(polaca[i], "RORD") == 0)
        {
            reorder_list[j] = (char *)malloc(MAX_LENGTH_RORD * sizeof(char)); // Cada string puede tener MAX_LENGTH_RORD chars
            reorder_list[j][0] = '\0';
            // sprintf(reorder_list[j], "\t%-35s_%d\tDB	\"[","@sys_RORD", i); // ¡Peligro de desbordamiento!
            snprintf(buffer, 50, "@sys_RORD_%d", i);
            snprintf(reorder_list[j], MAX_LENGTH_RORD, "\t%-35s\tDB	\"[", buffer);
            i++;
            contArg = 0;
            while (strcmp(polaca[i], "f@end") != 0 && i < size)
            {
                if (es_operador(polaca[i]))
                {
                    if (contArg == 2)
                    {
                        segOp = pop(pilaASM);
                        priOp = pop(pilaASM);
                        strcat(reorder_list[j], priOp);
                        strcat(reorder_list[j], polaca[i]);
                        strcat(reorder_list[j], segOp);
                        contArg = 0;
                    }
                    else
                    {
                        priOp = pop(pilaASM);
                        strcat(reorder_list[j], polaca[i]);
                        strcat(reorder_list[j], priOp);
                    }
                    if (strcmp(polaca[i + 1], ",") == 0)
                    {
                        strcat(reorder_list[j], ",");
                    }
                }
                else
                {
                    if (strcmp(polaca[i], ",") != 0)
                    {
                        push(pilaASM, polaca[i]);
                        contArg++;
                    }
                }
                i++;
            }
            strcat(reorder_list[j], "]\",'$'\n");
            // fprintf(archivo, "\t%-35s\tDB	%s", tmpVar, reorder_list[i]);
            j++;
        }
    }
    reorder_list[j] = NULL;
    return reorder_list;
}

int generar_assembler(tList *ptrTS, char **polaca, int rpn_size)
{
    if (rpn_size > 0)
    {
        printf("La polaca tiene %d ELEMENTOS\n\n", rpn_size);
    }
    else
    {
        printf("La polaca NO tiene ELEMENTOS: size %d\n\n", rpn_size);
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
    FILE *archivo = fopen(nombre_archivo, "w");

    // Verificar si el archivo se abrió/creó correctamente
    if (archivo == NULL)
    {
        perror("Error al abrir o crear el archivo"); // Muestra un mensaje de error del sistema
        return EXIT_FAILURE;                         // Sale del programa con un código de error
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
    while (*tmpTS)
    {
        if (strcmp((*tmpTS)->dataType, "CTE_FLOAT") == 0)
        {
#ifdef DEBUG_MODE
            printf("DEBUG: GENERADO %s value(%s), type(%s)\n", (*tmpTS)->name, (*tmpTS)->value, (*tmpTS)->dataType);
#endif // DEBUG_MODE

            fprintf(archivo, "\t%-35s	DD	%s\n", estandarizar_nombre_ensamblador((*tmpTS)->value, TYPE_FLOAT), (*tmpTS)->value);
        }
        else if (strcmp((*tmpTS)->dataType, "CTE_INTEGER") == 0)
        {
#ifdef DEBUG_MODE
            printf("DEBUG: GENERADO %s value(%s), type(%s)\n", (*tmpTS)->name, (*tmpTS)->value, (*tmpTS)->dataType);
#endif // DEBUG_MODE
            fprintf(archivo, "\t%-35s	DD	%s.0\n", estandarizar_nombre_ensamblador((*tmpTS)->value, TYPE_INT), (*tmpTS)->value);
        }
        tmpTS = &(*tmpTS)->next;
    }
    fprintf(archivo, "\n"); // FIN DE CONSTANTES ENTERAS Y FLOTANTES

    /** CONSTANTES STRING */
    fprintf(archivo, "; definicion de constantes string\n");
    tmpTS = ptrTS;
    char *tmpVar;
    char varName[80] = "";
    while (*tmpTS)
    {

        if (strcmp((*tmpTS)->dataType, "CTE_STRING") == 0)
        {
#ifdef DEBUG_MODE
            printf("DEBUG: ESTE ES %s value(%s), type(%s)\n", (*tmpTS)->name, (*tmpTS)->value, (*tmpTS)->dataType);
#endif // DEBUG_MODE

            varName[0] = '"';
            varName[1] = '\0';
            strcat(varName, (*tmpTS)->value);
            strcat(varName, "\"");
            tmpVar = estandarizar_nombre_ensamblador(varName, TYPE_STRING);
            fprintf(archivo, "\t%-35s\tDB  %s,'$'\n", tmpVar, varName);
            fprintf(archivo, "\ts@%-32s\tEQU ($ - %s)\n", tmpVar, tmpVar);
        }
        tmpTS = &(*tmpTS)->next;
    }
    fprintf(archivo, "\n"); // FIN DE CONSTANTES STRING

    /** DECLARACIÓN DE STRING DE REORDER */
    char **reorder_list = obtener_reorder_list(polaca, rpn_size, &pilaASM);
    i = 0;
    while (reorder_list[i] != NULL)
    {
        fprintf(archivo, "%s", reorder_list[i]);
        i++;
    }

    fprintf(archivo, "\n");


    fprintf(archivo, "\t@tmp\tDD ?\n");
    fprintf(archivo, "\t_float_p_0_5_\tDD 0.5\n");
    fprintf(archivo, "\t@tmp_area1\tDD ?\n");
    fprintf(archivo, "\t@tmp_area2\tDD ?\n");

  
    // Declarar temporales para cada NCALC encontrado en la polaca
    for (i = 0; i < rpn_size; i++)
    {
        if (polaca[i] != NULL && strncmp(polaca[i], "NCALC_", 6) == 0)
        {
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
    fprintf(archivo, "\tbufferIn\tDB\t20,?,20 DUP('$')\n");
    tmpTS = ptrTS;
    while (*tmpTS)
    {
        if (strcmp((*tmpTS)->dataType, "STRING") == 0)
        {
#ifdef DEBUG_MODE
            printf("DEBUG: ESTa ES %s type(%s)\n", (*tmpTS)->name, (*tmpTS)->dataType);
#endif                                                                                       // DEBUG_MODE
            fprintf(archivo, "\t@usr_%-30s\tDB	MAXTEXTSIZE dup (?),'$'\n", (*tmpTS)->name); /** DECLARACIÓN DE Asignación a STRING*/
        }
        else if (strcmp((*tmpTS)->dataType, "FLOAT") == 0 || strcmp((*tmpTS)->dataType, "INTEGER") == 0)
        {
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
    int salto = -1;
    int codeOperador;
    tmpTS = ptrTS;
    for (i = 0; i < rpn_size; i++)
    {
        // Escribir algo en el archivo
        if (salto == i)
        {
            fprintf(archivo, "TAG_%d:\n", i);
            salto = -1;
        }
        codeOperador = es_operador(polaca[i]);
        if (codeOperador)
        {
            char *priOp;
            char *segOp;
            if (codeOperador == 5) { // =:
                priOp = pop(&pilaASM); // destino (id)
                segOp = pop(&pilaASM); // source (puede ser @tmp, cte o var)

                if (strcmp(get_type_in_ts(tmpTS, priOp), "STRING") == 0) {
                    tmpVar = estandarizar_nombre_ensamblador(segOp, get_std_type(get_type_in_ts(tmpTS, segOp)));
                    fprintf(archivo, "\tassignToString %s, @usr_%s, s@%s\n", tmpVar, priOp, tmpVar);
                } else {
                    if (strcmp(segOp, "@tmp") == 0) {
                        fprintf(archivo, "\tFLD @tmp\n");
                        fprintf(archivo, "\tFSTP @usr_%s\n", priOp);
                    } else {
                        fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(segOp, get_std_type(get_type_in_ts(tmpTS, segOp))));
                        fprintf(archivo, "\tFSTP @usr_%s\n", priOp);
                    }
                }
            }
            else if (codeOperador >= 1 && codeOperador <= 4) { // +, -, *, /
                segOp = pop(&pilaASM);   // derecha
                priOp = pop(&pilaASM);   // izquierda

                // Cargar SIEMPRE ambos operandos en la FPU
                if (strcmp(priOp, "@tmp") == 0) {
                    fprintf(archivo, "\tFLD @tmp\n"); // izquierda
                } else {
                    agregar_operando(archivo, tmpTS, priOp); // izquierda
                }
                agregar_operando(archivo, tmpTS, segOp);     // derecha

                switch (codeOperador) {
                    case 1: // +
                        // pri + seg: usar FADDP para ordenar y limpiar pila
                        fprintf(archivo, "\tFADDP\n");  // ST(1)=ST(1)+ST(0); pop
                        break;
                    case 2: // -
                        // pri - seg
                        fprintf(archivo, "\tFSUBP\n");  // ST(1)=ST(1)-ST(0); pop
                        break;
                    case 3: // *
                        fprintf(archivo, "\tFMULP\n");  // ST(1)=ST(1)*ST(0); pop
                        break;
                    case 4: // /
                        // pri / seg
                        fprintf(archivo, "\tFDIVP\n");  // ST(1)=ST(1)/ST(0); pop
                        break;
                }

                // Ahora el resultado quedó en ST(0). Guardo y apilo.
                fprintf(archivo, "\tFSTP @tmp\n");
                push(&pilaASM, "@tmp");
            }
            else if (codeOperador == 9) { // :=
                char *dest = pop(&pilaASM);
                char *src  = pop(&pilaASM);
                if (strcmp(src, "@tmp") == 0) {
                    fprintf(archivo, "\tFLD @tmp\n");
                } else {
                    fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(src, get_std_type(get_type_in_ts(tmpTS, src))));
                }
                fprintf(archivo, "\tFSTP @usr_%s\n", dest);
            }
            else if (codeOperador == 10) { // =
                // Asumimos que negativeCalculation dejó en @tmp o bien en _ncalc_res_k
                char *dest = pop(&pilaASM);
                char *src  = pop(&pilaASM); // podría ser "@tmp" o "_ncalc_res_k"
                if (strcmp(src, "@tmp") == 0) {
                    fprintf(archivo, "\tFLD @tmp\n");
                } else {
                    fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(src, get_std_type(get_type_in_ts(tmpTS, src))));
                }
                fprintf(archivo, "\tFSTP @usr_%s\n", dest);
            }
            else if (strcmp(polaca[i], "write") == 0) {
                priOp = pop(&pilaASM);

                if (strcmp(priOp, "@tmp") == 0) {
                    fprintf(archivo, "\tDisplayFloat @tmp,2\n\tnewLine 1\n");
                } else {
                    char *var_type = get_type_in_ts(tmpTS, priOp);
                    if (strcmp(var_type, "STRING") == 0) {
                        fprintf(archivo, "\tdisplayString @usr_%s\n\tnewLine 1\n", priOp);
                        // PARA ESCRIBIR CORRECTAMENTE VARIABLES INTEGER INGRESADAS POR TECLADO HAY QUE SEPARAR ESTE IF
                        // PERO HAY QUE VERIFIAR LA INTEGRIDAD CON EL RESTO DE LAS FUNCIONALIDADES
                    } else if (strcmp(var_type, "FLOAT") == 0 || strcmp(var_type, "INTEGER") == 0) {
                        // Tus enteros están como DD .0 → podés mostrarlos con DisplayFloat también si querés homogeneidad
                        fprintf(archivo, "\tDisplayFloat @usr_%s,2\n\tnewLine 1\n", priOp);
                    } else {
                        fprintf(archivo, "\tdisplayString %s\n\tnewLine 1\n",
                            estandarizar_nombre_ensamblador(priOp, get_std_type(get_type_in_ts(tmpTS, priOp))));
                    }
                }
            }
            else if (strcmp(polaca[i], "read") == 0)
            {
                // El siguiente token ya fue insertado en la polaca (el ID fue empujado al stack)
                priOp = pop(&pilaASM); // priOp contiene el nombre de la variable, por ejemplo "variable1"

                // Obtener tipo de la TS
                const char *tipoID = get_type_in_ts(&listaTS, priOp);

                //printf("DEBUG: %s\n", tipoID);

                if (!tipoID)
                {
                    printf("ERROR: Variable '%s' no fue declarada\n", priOp);
                    exit(1);
                }

                // Emitir código ASM según tipo. Usar @usr_<nombre> como etiqueta donde está la variable
                if (strcmp(tipoID, "FLOAT") == 0)
                {
                    fprintf(archivo, "\t; Lectura de variable float %s\n", priOp);
                    fprintf(archivo, "\tGetFloat @usr_%s\n", priOp);
                }
                else if (strcmp(tipoID, "INTEGER") == 0)
                {
                    fprintf(archivo, "\t; Lectura de variable int %s\n", priOp);
                    fprintf(archivo, "\tGetInteger @usr_%s\n", priOp);
                }
                else
                {
                    printf("ERROR: Tipo '%s' no soportado en read para %s\n", tipoID, priOp);
                    exit(1);
                }
                // fprintf(archivo, "\tnewLine 1\n");
            }
            else if (strcmp(polaca[i], "CMP") == 0)
            {
                segOp = pop(&pilaASM);
                priOp = pop(&pilaASM);
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
                char *tipo_salto = get_jump(polaca[i]);
                i++;
                fprintf(archivo, "\t%s TAG_%s\n", tipo_salto, polaca[i]);
                salto = atoi(polaca[i]);
            }
            else
            {
                priOp = pop(&pilaASM);
                segOp = pop(&pilaASM);
                fprintf(archivo, "\t%s\n", priOp);
            }
        }
        else if (strcmp(polaca[i], "BI") == 0)
        {
            i++;
            fprintf(archivo, "\tJMP TAG_%s\n", polaca[i]);
            salto = atoi(polaca[i]);
            fprintf(archivo, "TAG_%d:\n", i + 1);
        }
        else if (strcmp(polaca[i], "ET") == 0)
        {
            fprintf(archivo, "TAG_%d:\n", i);
        }
        else if (strcmp(polaca[i], ":=") == 0 || strcmp(polaca[i], "=:") == 0) {
            char *dest = pop(&pilaASM); // variable destino
            char *src  = pop(&pilaASM); // valor calculado o literal

            // Si el valor no está en @tmp, cargarlo y guardarlo en @tmp
            if (strcmp(src, "@tmp") != 0) {
                fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(src, get_std_type(get_type_in_ts(tmpTS, src))));
                fprintf(archivo, "\tFSTP @tmp\n");
            }

            // Guardar @tmp en la variable destino
            fprintf(archivo, "\tFLD @tmp\n");
            fprintf(archivo, "\tFSTP @usr_%s\n", dest);
        }
        else if (strcmp(polaca[i], "RORD") == 0)
        {
            fprintf(archivo, "\tdisplayString @sys_RORD_%d\n\tnewLine 1\n", i);
            for (; strcmp(polaca[i], "f@end") != 0; i++)
                ;
        }
        else if (polaca[i] != NULL && strncmp(polaca[i], "NCALC_", 6) == 0)
        {
            int nargs = 0;
            sscanf(polaca[i], "NCALC_%d", &nargs);

            fprintf(archivo, ";funcion NCALC_\n");

            // Sacar (pop) los argumentos de la pila ASM en orden correcto
            char **args = (char **)malloc(sizeof(char *) * nargs);
            int k;
            for (k = nargs - 1; k >= 0; k--)
            {
                args[k] = pop(&pilaASM); // pop devuelve el token (ID o constante)
            }

            // 1) Guardar todos los argumentos en el array temporal (_ncalc_args_i)
            for (k = 0; k < nargs; k++)
            {
                // cargar el operando en FPU y guardarlo en la posición k del array
                agregar_operando(archivo, tmpTS, args[k]);                            // emite "FLD <operando>"
                fprintf(archivo, "\tFSTP DWORD PTR _ncalc_args_%d + %d\n", i, 4 * k); // almacena y pop
            }

            // 2) Recorrer el array y para cada elemento:
            //    - cargar en temp, inspeccionar el bit de signo (mov->test)
            //    - si negativo: sumar en _ncalc_sum_i, multiplicar en _ncalc_prod_i, inc _ncalc_count_i
            for (k = 0; k < nargs; k++)
            {
                fprintf(archivo, "\tFLD DWORD PTR _ncalc_args_%d + %d\n", i, 4 * k);
                fprintf(archivo, "\tFSTP DWORD PTR _ncalc_temp_%d\n", i);     // temp = arg
                fprintf(archivo, "\tMOV EAX, DWORD PTR _ncalc_temp_%d\n", i); // mov bits
                fprintf(archivo, "\tTEST EAX, 80000000h\n");                  // check sign bit
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
            fprintf(archivo, "\tTEST EAX, 1\n"); // par/impar
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
        }
        else if (strcmp(polaca[i], "TRIAMAX") == 0) {
            fprintf(archivo, "\t; --- TRIANGLE AREA MAXIMUM ---\n");           

            char *coord;
            char *tipo;
            char *coords[12];
            int k = 11;

            while (k >= 0) {
                coord = pop(&pilaASM);
                if (strcmp(coord, "PUNTO") == 0 || strcmp(coord, "VERTEXLIST") == 0) continue;
                tipo = get_type_in_ts(tmpTS, coord);
                if (!tipo) {
                    printf("ERROR: Símbolo '%s' no encontrado en la TS\n", coord);
                    exit(1);
                }
                // printf("DEBUG: mostrar i: %d\n",coord);
                coords[k] = strdup(coord);
                k--;
            }            

            //area=0.5⋅∣x1​(y2​−y3​)+x2​(y3​−y1​)+x3​(y1​−y2​)∣
            // --- Área triángulo 1 ---
            fprintf(archivo, "\t; Calcular área triángulo 1\n");
            // x1*(y2 - y3)
            fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(coords[3], get_std_type(get_type_in_ts(tmpTS, coords[3]))));
            fprintf(archivo, "\tFSUB %s\n", estandarizar_nombre_ensamblador(coords[5], get_std_type(get_type_in_ts(tmpTS, coords[5]))));
            fprintf(archivo, "\tFMUL %s\n", estandarizar_nombre_ensamblador(coords[0], get_std_type(get_type_in_ts(tmpTS, coords[0]))));

            // + x2*(y3 - y1)
            fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(coords[5], get_std_type(get_type_in_ts(tmpTS, coords[5]))));
            fprintf(archivo, "\tFSUB %s\n", estandarizar_nombre_ensamblador(coords[1], get_std_type(get_type_in_ts(tmpTS, coords[1]))));
            fprintf(archivo, "\tFMUL %s\n", estandarizar_nombre_ensamblador(coords[2], get_std_type(get_type_in_ts(tmpTS, coords[2]))));
            fprintf(archivo, "\tFADD\n");

            // + x3*(y1 - y2)
            fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(coords[1], get_std_type(get_type_in_ts(tmpTS, coords[1]))));
            fprintf(archivo, "\tFSUB %s\n", estandarizar_nombre_ensamblador(coords[3], get_std_type(get_type_in_ts(tmpTS, coords[3]))));
            fprintf(archivo, "\tFMUL %s\n", estandarizar_nombre_ensamblador(coords[4], get_std_type(get_type_in_ts(tmpTS, coords[4]))));
            fprintf(archivo, "\tFADD\n");

            fprintf(archivo, "\tFABS\n");
            fprintf(archivo, "\tFLD _float_p_0_5_\n");
            fprintf(archivo, "\tFMUL\n");
            fprintf(archivo, "\tFSTP @tmp_area1\n");

            // PARA DEBUG
            // fprintf(archivo, "\tDisplayFloat @tmp_area1, 2\n");
            // fprintf(archivo, "\tnewLine 1\n");

            // --- Área triángulo 2 ---
            fprintf(archivo, "\t; Calcular área triángulo 2\n");
            // x4*(y5 - y6)
            fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(coords[9], get_std_type(get_type_in_ts(tmpTS, coords[9]))));
            fprintf(archivo, "\tFSUB %s\n", estandarizar_nombre_ensamblador(coords[11], get_std_type(get_type_in_ts(tmpTS, coords[11]))));
            fprintf(archivo, "\tFMUL %s\n", estandarizar_nombre_ensamblador(coords[6], get_std_type(get_type_in_ts(tmpTS, coords[6]))));

            // + x5*(y6 - y4)
            fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(coords[11], get_std_type(get_type_in_ts(tmpTS, coords[11]))));
            fprintf(archivo, "\tFSUB %s\n", estandarizar_nombre_ensamblador(coords[7], get_std_type(get_type_in_ts(tmpTS, coords[7]))));
            fprintf(archivo, "\tFMUL %s\n", estandarizar_nombre_ensamblador(coords[8], get_std_type(get_type_in_ts(tmpTS, coords[8]))));
            fprintf(archivo, "\tFADD\n");

            // + x6*(y4 - y5)
            fprintf(archivo, "\tFLD %s\n", estandarizar_nombre_ensamblador(coords[7], get_std_type(get_type_in_ts(tmpTS, coords[7]))));
            fprintf(archivo, "\tFSUB %s\n", estandarizar_nombre_ensamblador(coords[9], get_std_type(get_type_in_ts(tmpTS, coords[9]))));
            fprintf(archivo, "\tFMUL %s\n", estandarizar_nombre_ensamblador(coords[10], get_std_type(get_type_in_ts(tmpTS, coords[10]))));
            fprintf(archivo, "\tFADD\n");

            fprintf(archivo, "\tFABS\n");
            fprintf(archivo, "\tFLD _float_p_0_5_\n");
            fprintf(archivo, "\tFMUL\n");
            fprintf(archivo, "\tFSTP @tmp_area2\n");

            // PARA DEBUG
            // fprintf(archivo, "\tDisplayFloat @tmp_area2, 2\n");
            // fprintf(archivo, "\tnewLine 1\n");

            // Comparar y dejar el mayor en @tmp
            fprintf(archivo, "\tFLD @tmp_area2\n");
            fprintf(archivo, "\tFLD @tmp_area1\n");            
            fprintf(archivo, "\tFCOMPP\n");
            fprintf(archivo, "\tFSTSW AX\n");
            fprintf(archivo, "\tSAHF\n");
            fprintf(archivo, "\tJA mayor_area_%d\n", i);
            fprintf(archivo, "\tFLD @tmp_area2\n");
            fprintf(archivo, "\tJMP fin_triarea_%d\n", i);
            fprintf(archivo, "mayor_area_%d:\n", i);
            fprintf(archivo, "\tFLD @tmp_area1\n");
            fprintf(archivo, "fin_triarea_%d:\n", i);
            fprintf(archivo, "\tFSTP @tmp\n");

            push(&pilaASM, "@tmp");
        }
        else
        {
            push(&pilaASM, polaca[i]);
        }
        fflush(archivo);
    }
    if (salto == i)
    {
        fprintf(archivo, "TAG_%d:\n", i);
        salto = -1;
    }

    fprintf(archivo, "\nFINAL:\n"
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