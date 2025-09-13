   
    
 ;macros2.asm
;These are macros for Assembly Language Programming
;Myron Berg
;Dickinson State University
;4/5/99


;++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
getString                    macro  string        ;read string from keyboard
local  label1, label2, label3, label4, label5, label6, label7, label8

                        pushad
                        push    di
                        push    si
                      

                        lea    si, string
                        mov    bx, si

label1:                mov    ah, 1
                        int    21h
                        cmp    al, 0Dh
                        je      label2

                        cmp    al, 8
                        je      label8
                        jmp    label7

label8:                dec    si
                        cmp    si, bx
                        jl      label6
                      jmp    label1

label6:                mov    si, bx
                        jmp    label1
                        

label7:                mov    [si], al
                        inc    si
                        jmp    label1
label2:                mov    byte ptr [si], '$'

                        pop    si
                        pop    di
                        popad

endm    

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
displayString                  macro  string          ;write string on screen

                        push    dx
                        push    ax

                        lea    dx, string
                        mov    ah, 9
                        int    21h

                        pop    ax
                        pop    dx

endm

; ----------------------------------------------------
; Macro: GET_STRING_LENGTH
; Prop?sito: Calcula la longitud de una cadena terminada en nulo.
; Entrada:   Requiere que SI contenga la direcci?n de inicio de la cadena.
; Salida:    CX -> Longitud de la cadena (sin incluir el terminador nulo).
;            DI -> Apunta al byte NULL que termina la cadena.
;            AX se usa internamente (no se restaura).
;            CX es el resultado. SI se modifica temporalmente y luego se restaura.
; ----------------------------------------------------
GET_STRING_LENGTH MACRO
    LOCAL __start_scan, __end_scan ; Etiquetas locales para evitar conflictos de nombres

    ; Preservar SI si no quieres que cambie despu?s de la macro
    PUSH SI

    MOV DI, SI      ; DI debe apuntar al inicio de la cadena (SCASB usa ES:DI)
    MOV AL, 0       ; Byte a buscar: el terminador nulo (0)
    MOV CX, 0FFFFh  ; M?ximo n?mero de caracteres a buscar

    CLD             ; Asegurar que DI se incrementa

    __start_scan:   ; Etiqueta para el bucle
        SCASB       ; Compara AL con byte en ES:DI, incrementa DI, decrementa CX.
        JNE __start_scan ; Repite si el byte no es el terminador nulo

    ; Cuando el bucle termina, DI apunta al byte DESPUES del nulo, y CX es 0xFFFF - longitud + 1.
    NOT CX          ; Invierte todos los bits de CX
    DEC CX          ; Decrementa CX
                    ; Ahora CX contiene la longitud de la cadena.

    POP SI          ; Restaurar SI
ENDM ; Fin de la macro GET_STRING_LENGTH

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
assignToString          macro origin_str, dest_str, size          ;write string on screen
                        PUSH SI

                        ; asigna el string 
                        MOV SI, OFFSET origin_str
                        MOV DI, OFFSET dest_str
                        MOV CX, size
                        CLD           ; Asegura que SI y DI se incrementen
                        REP MOVSB     ; Copia CX bytes de [DS:SI] a [ES:DI]
                        
                        POP SI          ; Restaurar SI

endm

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
scrollup                macro      number    ;scroll screen up

                        pusha 
                        
                        mov    ah, 6          ;up
                        mov    al, number    ;lines
                        mov    ch, 0          ;starting at 0,0
                        mov    cl, 0
                        mov    dh, 24        ;ending at 24, 79
                        mov    dl, 79
                        mov    bh, 15          ;black background, white fore
                        int    10h

                        popa

endm

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
clearScreen            macro
                        scrollup 0
endm

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
newLine                macro  number        ;line feed and carriage return
local  Repeat

                        pusha

                        ifnb <number>
                        mov    cx, number
                        endif

Repeat:                mov    al, 0Dh
                        mov    ah, 0Eh
                        int    10h
                        mov    al, 0Ah  
                        mov    ah, 0Eh
                        int    10h
  
                        ifnb    <number>
                            loop    Repeat
                        endif

                        popa
      
endm

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
getdate                macro  near            ;get date
                        
                        mov    ah, 2Ah
                        Int    21h    
                        ;AL = day of week (0-7)
                        ;CX = year (1980-2099)
                        ;DL = month (1-12)
                        ;DH = day (1-31)
                        

endm
;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
gettime                macro  near            ;get time

                        mov    ah, 2Ch
                        Int    21h
                        ;AL = hour (0-23)
                        ;CX = minute (0-59)
                        ;DL = second (0-59)
                        ;DH = hundredth (0-99)

endm

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
setCurPos                macro  row, column    ;sets cursor position        

                        mov    dl, row
                        mov    dh, column
                        mov    ah, 2
                      int    10h

endm

;+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

beep                    macro                    ;beeps speaker

                        pusha

                        mov    dl, 7
                        mov    ah, 2
                        int    21h

                        popa

endm
  
  
 
  
  
