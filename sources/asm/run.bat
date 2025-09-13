PATH=C:\TASM;

tasm numbers.asm
tasm Final.asm
tlink Final.obj numbers.obj
final.exe
del final.obj 
del numbers.obj 
del final.exe
del final.map