# Repo - tpCompilador_1C_2025

Repositorio para trabajos de la material Lenguajes y Compiladores en UNLaM

## Información de la Asignatura

- **Materia**: Lenguajes y Compiladores (01124-3663)
- **Cuatrimestre**: Segundo Cuatrimestre
- **Año**: 2025

## Trayecto Infraestructura

- **Año académico**: Cuarto año
- **Carga horaria semanal**: 4 hs
- **Carga horaria total**: 64 hs
- **Modalidad**: Semipresencial

## Integrantes

| Nombre | DNI |
| ------ | --- |

| Vivas, Pablo | |

## Docentes

- Hernan Villarreal
- Eleazar Pin Etchave
- Daniel Carrizo

# Generalidades sobre el proyecto

El proyecto está destinado al desarrollo de un Compilador dado un lenguaje proporcionado.
En el mismo se involucran las etapas y consideraciones necesarias para que dado un programa esté correctamente definido.

A continuación se definen las reglas de sintaxis que el compilador acepta:

### Tipo de Asignaciones admitidas

- Asignación normal: variable := valor

  ```
  a := 99999.99
  a := 99.
  a := .9999

  b := "@sdADaSjfla%dfg"
  b := "asldk  fh sjf"
  ```

- Asignación de operación aritmética: variable =: operación
  ```
  x =: 27 - c
  x =: r + 500
  x =: 34 * 3
  x =: z / f
  ```
- Asignación de funciones: variable = negativeCalculation

  ```
  a := 4.1
  b := -1.5

  x = negativeCalculation(3.5, -2.0, a, b, -3.0)
  // Cantidad de negativos: 3 (impar)
  // x := (-2.0) * (-1.5) * (-3.0) = -9.0
  ```

### Declaración de Variables

```
init {
    a1, b1 : Float
    variable1 : Int
    p1, p2, p3 : String
}
```

### Comentarios

```
#+ Esto es un comentario +#
```

### Entrada y Salida

- Funcion "Read"
  ```
  read(base) #+ base es una variable +#
  ```
- Funcion "Write"
  ```
  write(“ewr”)  #+ “ewr” es una cte string +#
  write(var1)  #+ var1 es una variable numérica definida previamente +#
  ```

### Condicionales

- While

  ```
  a := 1
  b := 3

  while (a > b)
  {
      write("a es mas grande que b")
      a := a + 1
  }
  ```

- If - Else
  ```
  if (a > b)
  {
      write("a es mas grande que b")
  }
  else
  {
      write("a es mas chico o igual a b")
  }
  ```

### Operaciones Aritméticas

Operaciones Permitidas

- Suma: "+"
- Resta: "-"
- Multiplicación: "\*"
- División: "/"

Ejemplo:

```
x =: 27 - c
x =: r + 500
x =: 34 * 3
x =: z / f
```

### Operaciones Lógicas

Operaciones Permitidas

- AND

  ```
  a := 1
  b := 1
  c := 2

  if (a > b AND c > b)
  {
      write("a es mas grande que b y c es mas grande que b")
  }
  ```

- OR

  ```
  a := 1
  b := 1
  c := 2

  if (a > b OR c > b)
  {
      write("a es mas grande que b o c es mas grande que b")
  }
  ```

- NOT

  ```
  a := 1
  b := 1
  c := 2

  if (NOT a > b)
  {
      write("a no es mas grande que b")
  }
  ```

### Funciones Especiales

- <b>reorder</b>

  La función especial reorder recibe tres parámetros:

  - <b>Lista de expresiones</b>: La lista está entre corchetes y las expresiones separadas por comas.
  - <b>Valor booleano</b>: Un valor que indica la dirección del reordenamiento. Si el valor es 1, la parte de la lista que debe reordenarse será la que está a la izquierda del pivote. Si el valor es 0, será la de la derecha.
  - <b>Entero pivote</b>: Un valor entero que marca la posición en la lista desde la cual se inicia el reordenamiento. Este índice actúa como punto de referencia para dividir la lista en dos partes.

  La función reorder tomará la lista de expresiones y, dependiendo del valor del booleano, realizará un reordenamiento de las expresiones a partir de la posición indicada por el pivote. Si el valor booleano es 1, la parte de la lista a la izquierda del pivote debe ser reordenada de forma que la primera expresión ocupe la posición del pivote, la segunda la posición anterior, y así sucesivamente. Si el valor booleano es 0, se reordenará la parte de la lista a la derecha del pivote de forma similar.

  Ejemplos:

  ```
  reorder([x+3, 1+1, 9-x],1,2)  // la primer posición es cero

  //[9-x, 1+1, x+3]  luego de reordenar
  ```

  ```
  reorder([r*j-2, x+3, 1+1, 9-x],0,2)  // la primer posición es cero

  //[r*j-2, x+3, 9-x, 1+1]  luego de reordenar
  ```

  ```
  reorder([r*j-2, x+3, 1+1, 9-x],0,3)  // no reordena porque no hay expresiones del lado izq del pivote
  ```

  ```
  reorder([r*j-2, x+3, 1+1, 9-x],1,0)  // no reordena porque no hay expresiones del lado derecho del pivote
  ```

- <b>negativeCalculation</b>

  Función que recibe como único parámetro una lista de variables y/o constantes de tipo Float, donde los elementos pueden ser tanto negativos como positivos.

  La lista estará encerrada entre paréntesis y sus elementos estarán separados por comas.
  La función analiza la cantidad de valores negativos dentro de la lista:
  Si la cantidad de números negativos es par, la función deberá sumar todos los valores negativos y devolver el resultado en una variable.
  Si la cantidad de números negativos es impar, la función multiplica todos los valores negativos y devuelve el resultado en una variable.

  Ejemplo:

  ```
  a := 4.1
  b := -1.5

  x = negativeCalculation(3.5, -2.0, a, b, -3.0)
  // Cantidad de negativos: 3 (impar)
  // x := (-2.0) * (-1.5) * (-3.0) = -9.0

  ```

  ```
  c := -1.7

  x = negativeCalculation(-4.0, 2.3, c, 5.6)
  // Cantidad de negativos: 2 (par)
  // x := (-4.0) + (-1.7) = -5.7

  ```

- <b>áreaMáxima del triángulo</b>

Se desea implementar una función llamada TriangleAreaMaximum que, dadas las coordenadas de dos triángulos en el plano cartesiano, determine cuál de los dos tiene mayor área y devuelva dicho valor numérico.

La función recibirá dos triángulos, cada uno definido por las coordenadas de sus vértices en el plano cartesiano. Dichas coordenadas pueden ser constantes o variables, enteras o flotantes. La función tendrá el formato del ejemplo:

Ejemplo:

```
Integer z := 2;
Float x := 3.8;
areaMax = triangleAreaMaximum( [0,x; 4.0,12; 2.5,3] ; [x,0; 6,0; z,2] )  //areaMax = 11.85
Con los valores dados:

Triángulo 1 → área = 11,85; Triángulo 2 → área = 2,2; Área máxima = 11,85

```
