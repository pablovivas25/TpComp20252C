:: Usar un archivo de prueba como parametro o el default
set TEST_FILE=%1
echo "el archivo encontrado es %TEST_FILE%"
if not "%TEST_FILE%"=="" (
    echo "corriendo test con %TEST_FILE%"
) else (
    set TEST_FILE=test.txt
)

:: Script para windows
flex Lexico.l
bison -dyv Sintactico.y

gcc.exe ./funciones/lista.c ./funciones/funciones.c ./funciones/PilaEstaticaASM.c ./funciones/AssemblerUtils.c lex.yy.c y.tab.c -o compilador.exe


compilador.exe "%TEST_FILE%"

@echo off
del compilador.exe
del lex.yy.c
del y.tab.c
del y.tab.h
del y.output

pause