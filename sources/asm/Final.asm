include macros2.asm
include number.asm

.MODEL  LARGE
.386
.STACK 200h

MAXTEXTSIZE equ 50

.DATA
; definicion de constantes float y enteras
	_float_n_1_7                       	DD	-1.7
	_float_n_2_0                       	DD	-2.0
	_float_n_3_0                       	DD	-3.0
	_float_n_4_0                       	DD	-4.0
	_float_p_9999                      	DD	.9999
	_int_p_0                           	DD	0.0
	_int_p_1                           	DD	1.0
	_int_p_10                          	DD	10.0
	_int_p_12                          	DD	12.0
	_int_p_2                           	DD	2.0
	_float_p_2                         	DD	2.
	_float_p_2_3                       	DD	2.3
	_float_p_2_5                       	DD	2.5
	_int_p_27                          	DD	27.0
	_int_p_3                           	DD	3.0
	_float_p_3_5                       	DD	3.5
	_float_p_3_8                       	DD	3.8
	_int_p_34                          	DD	34.0
	_float_p_4_0                       	DD	4.0
	_float_p_4_1                       	DD	4.1
	_int_p_5                           	DD	5.0
	_float_p_5_6                       	DD	5.6
	_int_p_500                         	DD	500.0
	_int_p_6                           	DD	6.0
	_float_p_6_0                       	DD	6.0
	_int_p_9                           	DD	9.0
	_float_p_99999_99                  	DD	99999.99
	_medio                             	DD	0.5
	_area1                             	DD	?
	_area2                             	DD	?
	_areaMax                           	DD	?

; definicion de constantes string
	_strh_5e3e3852                     	DB  "-------------",'$'
	s@_strh_5e3e3852                  	EQU ($ - _strh_5e3e3852)
	_strh_3f6361cc                     	DB  "@sdADaSjfla%dfg",'$'
	s@_strh_3f6361cc                  	EQU ($ - _strh_3f6361cc)
	_strh_d6f23335                     	DB  "Numero ingresado:",'$'
	s@_strh_d6f23335                  	EQU ($ - _strh_d6f23335)
	_strh_357861d                      	DB  "a es mas chico o igual a a1",'$'
	s@_strh_357861d                   	EQU ($ - _strh_357861d)
	_strh_e9342091                     	DB  "a es mas grande que a1",'$'
	s@_strh_e9342091                  	EQU ($ - _strh_e9342091)
	_strh_8b8f1e89                     	DB  "asldk  fh sjf",'$'
	s@_strh_8b8f1e89                  	EQU ($ - _strh_8b8f1e89)
	_strh_a17ac89f                     	DB  "d no es mas grande que c. check NOT",'$'
	s@_strh_a17ac89f                  	EQU ($ - _strh_a17ac89f)
	_strh_e04b63a7                     	DB  "muestro hasta que c sea mayor que d",'$'
	s@_strh_e04b63a7                  	EQU ($ - _strh_e04b63a7)
	_strh_b2ba1036                     	DB  "variable1 > d o c > d",'$'
	s@_strh_b2ba1036                  	EQU ($ - _strh_b2ba1036)
	_strh_ed4c6100                     	DB  "variable1 > d y c > d",'$'
	s@_strh_ed4c6100                  	EQU ($ - _strh_ed4c6100)

	@sys_RORD_159                      	DB	"[9-x,x+3,1+1]",'$'
	@sys_RORD_172                      	DB	"[r*j-2,+3,-x,+1]",'$'

	_ncalc_args_202	DD	5 DUP(?)
	_ncalc_temp_202	DD	?
	_ncalc_sum_202	DD	0.0
	_ncalc_prod_202	DD	1.0
	_ncalc_count_202	DD	0
	_ncalc_res_202	DD	0.0
	_ncalc_args_212	DD	4 DUP(?)
	_ncalc_temp_212	DD	?
	_ncalc_sum_212	DD	0.0
	_ncalc_prod_212	DD	1.0
	_ncalc_count_212	DD	0
	_ncalc_res_212	DD	0.0

; definicion de variables
	@usr_a                             	DD  ?
	@usr_a1                            	DD  ?
	@usr_b                             	DB	MAXTEXTSIZE dup (?),'$'
	@usr_b1                            	DD  ?
	@usr_c                             	DD  ?
	@usr_contador                      	DD  ?
	@usr_d                             	DD  ?
	@usr_f                             	DD  ?
	@usr_h                             	DD  ?
	@usr_j                             	DD  ?
	@usr_l                             	DD  ?
	@usr_numeroEntero                  	DD  ?
	@usr_p1                            	DB	MAXTEXTSIZE dup (?),'$'
	@usr_p2                            	DB	MAXTEXTSIZE dup (?),'$'
	@usr_p3                            	DB	MAXTEXTSIZE dup (?),'$'
	@usr_q                             	DD  ?
	@usr_r                             	DD  ?
	@usr_s                             	DD  ?
	@usr_variable1                     	DD  ?
	@usr_x                             	DD  ?
	@usr_x1                            	DD  ?
	@usr_z                             	DD  ?

.CODE
START:
	MOV AX,@DATA
	MOV DS,AX
	MOV ES,AX
	FINIT; Inicializa el coprocesador
	FLD _float_p_99999_99
	FSTP @usr_a
	FLD _float_p_2
	FSTP @usr_a
	FLD _float_p_9999
	FSTP @usr_a1
	FLD _float_p_6_0
	FSTP @usr_b1
	FLD _int_p_1
	FLD @usr_b1
	FMUL
	FLD @usr_a
	FADD
	FSTP @usr_x1
	DisplayFloat @usr_x1,2
	newLine 1
	FLD _float_p_2_5
	FSTP @usr_b1
	FLD _int_p_10
	FLD @usr_b1
	FDIV
	FLD @usr_a
	FADD
	FSTP @usr_x1
	DisplayFloat @usr_x1,2
	newLine 1
	assignToString _strh_3f6361cc, @usr_b, s@_strh_3f6361cc
	assignToString _strh_8b8f1e89, @usr_p1, s@_strh_8b8f1e89
	FLD @usr_a
	FLD @usr_a1
	FXCH
	FCOM
	FSTSW AX
	SAHF
	JBE TAG_48
	displayString _strh_e9342091
	newLine 1
	JMP TAG_50
TAG_48:
	displayString _strh_357861d
	newLine 1
TAG_50:
	FLD _int_p_5
	FSTP @usr_variable1
	FLD _int_p_1
	FSTP @usr_d
	FLD _int_p_2
	FSTP @usr_c
	FLD @usr_variable1
	FLD @usr_d
	FXCH
	FCOM
	FSTSW AX
	SAHF
	JBE TAG_71
	FLD @usr_c
	FLD @usr_d
	FXCH
	FCOM
	FSTSW AX
	SAHF
	JBE TAG_71
	displayString _strh_ed4c6100
	newLine 1
TAG_71:
	FLD _int_p_3
	FSTP @usr_variable1
	FLD _int_p_1
	FSTP @usr_d
	FLD _int_p_2
	FSTP @usr_c
	FLD @usr_variable1
	FLD @usr_d
	FXCH
	FCOM
	FSTSW AX
	SAHF
	JA TAG_85
TAG_85:
	FLD @usr_c
	FLD @usr_d
	FXCH
	FCOM
	FSTSW AX
	SAHF
	JBE TAG_92
	displayString _strh_b2ba1036
	newLine 1
TAG_92:
	FLD _int_p_1
	FSTP @usr_d
	FLD _int_p_2
	FSTP @usr_c
	FLD @usr_d
	FLD @usr_c
	FXCH
	FCOM
	FSTSW AX
	SAHF
	JA TAG_105
	displayString _strh_a17ac89f
	newLine 1
TAG_105:
	FLD _int_p_1
	FSTP @usr_c
	FLD _int_p_3
	FSTP @usr_d
TAG_111:
	FLD @usr_c
	FLD @usr_d
	FXCH
	FCOM
	FSTSW AX
	SAHF
	JAE TAG_126
	displayString _strh_e04b63a7
	newLine 1
	FLD @usr_c
	FLD _int_p_1
	FADD
	FSTP @usr_c
	JMP TAG_111
TAG_126:
	displayString _strh_d6f23335
	newLine 1
	DisplayInteger @usr_numeroEntero
	newLine 1
	displayString _strh_5e3e3852
	newLine 1
	FLD _int_p_27
	FLD @usr_c
	FSUB
	FSTP @usr_x
	FLD @usr_r
	FLD _int_p_500
	FADD
	FSTP @usr_x
	FLD _int_p_34
	FLD _int_p_3
	FMUL
	FSTP @usr_x
	FLD @usr_z
	FLD @usr_f
	FDIV
	FSTP @usr_x1
	FLD @usr_r
	FLD @usr_j
	FMUL
	FLD _int_p_2
	FSUB
	FSTP @usr_x
	displayString @sys_RORD_159
	newLine 1
	displayString @sys_RORD_172
	newLine 1
	FLD _float_p_4_1
	FSTP @usr_a
	FLD _float_n_1_7
	FSTP @usr_f
	FLD _float_p_3_5
	FSTP DWORD PTR _ncalc_args_202 + 0
	FLD _float_n_2_0
	FSTP DWORD PTR _ncalc_args_202 + 4
	FLD @usr_a
	FSTP DWORD PTR _ncalc_args_202 + 8
	FLD @usr_f
	FSTP DWORD PTR _ncalc_args_202 + 12
	FLD _float_n_3_0
	FSTP DWORD PTR _ncalc_args_202 + 16
	FLD DWORD PTR _ncalc_args_202 + 0
	FSTP DWORD PTR _ncalc_temp_202
	MOV EAX, DWORD PTR _ncalc_temp_202
	TEST EAX, 80000000h
	JZ NCALC_202_NOTNEG_0
	FLD DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_temp_202
	FADD
	FSTP DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_prod_202
	FLD DWORD PTR _ncalc_temp_202
	FMUL
	FSTP DWORD PTR _ncalc_prod_202
	INC DWORD PTR _ncalc_count_202
NCALC_202_NOTNEG_0:
	FLD DWORD PTR _ncalc_args_202 + 4
	FSTP DWORD PTR _ncalc_temp_202
	MOV EAX, DWORD PTR _ncalc_temp_202
	TEST EAX, 80000000h
	JZ NCALC_202_NOTNEG_1
	FLD DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_temp_202
	FADD
	FSTP DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_prod_202
	FLD DWORD PTR _ncalc_temp_202
	FMUL
	FSTP DWORD PTR _ncalc_prod_202
	INC DWORD PTR _ncalc_count_202
NCALC_202_NOTNEG_1:
	FLD DWORD PTR _ncalc_args_202 + 8
	FSTP DWORD PTR _ncalc_temp_202
	MOV EAX, DWORD PTR _ncalc_temp_202
	TEST EAX, 80000000h
	JZ NCALC_202_NOTNEG_2
	FLD DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_temp_202
	FADD
	FSTP DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_prod_202
	FLD DWORD PTR _ncalc_temp_202
	FMUL
	FSTP DWORD PTR _ncalc_prod_202
	INC DWORD PTR _ncalc_count_202
NCALC_202_NOTNEG_2:
	FLD DWORD PTR _ncalc_args_202 + 12
	FSTP DWORD PTR _ncalc_temp_202
	MOV EAX, DWORD PTR _ncalc_temp_202
	TEST EAX, 80000000h
	JZ NCALC_202_NOTNEG_3
	FLD DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_temp_202
	FADD
	FSTP DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_prod_202
	FLD DWORD PTR _ncalc_temp_202
	FMUL
	FSTP DWORD PTR _ncalc_prod_202
	INC DWORD PTR _ncalc_count_202
NCALC_202_NOTNEG_3:
	FLD DWORD PTR _ncalc_args_202 + 16
	FSTP DWORD PTR _ncalc_temp_202
	MOV EAX, DWORD PTR _ncalc_temp_202
	TEST EAX, 80000000h
	JZ NCALC_202_NOTNEG_4
	FLD DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_temp_202
	FADD
	FSTP DWORD PTR _ncalc_sum_202
	FLD DWORD PTR _ncalc_prod_202
	FLD DWORD PTR _ncalc_temp_202
	FMUL
	FSTP DWORD PTR _ncalc_prod_202
	INC DWORD PTR _ncalc_count_202
NCALC_202_NOTNEG_4:
	MOV EAX, DWORD PTR _ncalc_count_202
	CMP EAX, 0
	JE NCALC_202_ZERO
	TEST EAX, 1
	JZ NCALC_202_EVEN
	FLD DWORD PTR _ncalc_prod_202
	JMP NCALC_202_END
NCALC_202_EVEN:
	FLD DWORD PTR _ncalc_sum_202
	JMP NCALC_202_END
NCALC_202_ZERO:
	FLDZ
NCALC_202_END:
	FSTP DWORD PTR _ncalc_res_202
	DisplayFloat _ncalc_res_202, 2
	newLine 1
	FLD DWORD PTR _ncalc_res_202
	FLD _float_n_1_7
	FSTP @usr_b1
	FLD _float_n_4_0
	FSTP DWORD PTR _ncalc_args_212 + 0
	FLD _float_p_2_3
	FSTP DWORD PTR _ncalc_args_212 + 4
	FLD @usr_b1
	FSTP DWORD PTR _ncalc_args_212 + 8
	FLD _float_p_5_6
	FSTP DWORD PTR _ncalc_args_212 + 12
	FLD DWORD PTR _ncalc_args_212 + 0
	FSTP DWORD PTR _ncalc_temp_212
	MOV EAX, DWORD PTR _ncalc_temp_212
	TEST EAX, 80000000h
	JZ NCALC_212_NOTNEG_0
	FLD DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_temp_212
	FADD
	FSTP DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_prod_212
	FLD DWORD PTR _ncalc_temp_212
	FMUL
	FSTP DWORD PTR _ncalc_prod_212
	INC DWORD PTR _ncalc_count_212
NCALC_212_NOTNEG_0:
	FLD DWORD PTR _ncalc_args_212 + 4
	FSTP DWORD PTR _ncalc_temp_212
	MOV EAX, DWORD PTR _ncalc_temp_212
	TEST EAX, 80000000h
	JZ NCALC_212_NOTNEG_1
	FLD DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_temp_212
	FADD
	FSTP DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_prod_212
	FLD DWORD PTR _ncalc_temp_212
	FMUL
	FSTP DWORD PTR _ncalc_prod_212
	INC DWORD PTR _ncalc_count_212
NCALC_212_NOTNEG_1:
	FLD DWORD PTR _ncalc_args_212 + 8
	FSTP DWORD PTR _ncalc_temp_212
	MOV EAX, DWORD PTR _ncalc_temp_212
	TEST EAX, 80000000h
	JZ NCALC_212_NOTNEG_2
	FLD DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_temp_212
	FADD
	FSTP DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_prod_212
	FLD DWORD PTR _ncalc_temp_212
	FMUL
	FSTP DWORD PTR _ncalc_prod_212
	INC DWORD PTR _ncalc_count_212
NCALC_212_NOTNEG_2:
	FLD DWORD PTR _ncalc_args_212 + 12
	FSTP DWORD PTR _ncalc_temp_212
	MOV EAX, DWORD PTR _ncalc_temp_212
	TEST EAX, 80000000h
	JZ NCALC_212_NOTNEG_3
	FLD DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_temp_212
	FADD
	FSTP DWORD PTR _ncalc_sum_212
	FLD DWORD PTR _ncalc_prod_212
	FLD DWORD PTR _ncalc_temp_212
	FMUL
	FSTP DWORD PTR _ncalc_prod_212
	INC DWORD PTR _ncalc_count_212
NCALC_212_NOTNEG_3:
	MOV EAX, DWORD PTR _ncalc_count_212
	CMP EAX, 0
	JE NCALC_212_ZERO
	TEST EAX, 1
	JZ NCALC_212_EVEN
	FLD DWORD PTR _ncalc_prod_212
	JMP NCALC_212_END
NCALC_212_EVEN:
	FLD DWORD PTR _ncalc_sum_212
	JMP NCALC_212_END
NCALC_212_ZERO:
	FLDZ
NCALC_212_END:
	FSTP DWORD PTR _ncalc_res_212
	DisplayFloat _ncalc_res_212, 2
	newLine 1
	FLD DWORD PTR _ncalc_res_212
	FLD _float_p_3_8
	FSTP @usr_l
	FLD _int_p_2
	FSTP @usr_q
; --- CALCULO AREA TRIANGULO 1 Y 2 ---
; --- CALCULO AREA TRIANGULO 1 ---
	FLD _int_p_12
	FLD _int_p_3
	FSUB
	FLD _int_p_0
	FMUL
	FSTP _area1
	FLD _int_p_3
	FLD _float_p_3_8
	FSUB
	FLD _float_p_4_0
	FMUL
	FLD _area1
	FADD
	FSTP _area1
	FLD _float_p_3_8
	FLD _int_p_12
	FSUB
	FLD _float_p_2_5
	FMUL
	FLD _area1
	FADD
	FABS
	FMUL _medio
	FSTP _area1

; --- CALCULO AREA TRIANGULO 2 ---
	FLD _int_p_0
	FLD _int_p_2
	FSUB
	FLD _float_p_3_8
	FMUL
	FSTP _area2
	FLD _int_p_2
	FLD _int_p_0
	FSUB
	FLD _int_p_6
	FMUL
	FLD _area2
	FADD
	FSTP _area2
	FLD _int_p_0
	FLD _int_p_0
	FSUB
	FLD _int_p_2
	FMUL
	FLD _area2
	FADD
	FABS
	FMUL _medio
	FSTP _area2
	FLD _area1
	FLD _area2
	FCOM
	FSTSW AX
	SAHF
	JGE AREA1_MAYOR
	FLD _area2
	FSTP _areaMax
	JMP FIN_AREAS
AREA1_MAYOR:
	FLD _area1
	FSTP _areaMax
FIN_AREAS:
	DisplayFloat _areaMax, 2
	 newLine 1

FINAL:
	MOV ah, 1 ; pausa, espera que oprima una tecla
	INT 21h ; AH=1 es el servicio de lectura
	MOV AX, 4C00h ; Sale del Dos
	INT 21h       ; Enviamos la interripcion 21h
END START ; final del archivo.
