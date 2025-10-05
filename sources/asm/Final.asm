include macros2.asm
include number.asm

.MODEL  LARGE
.386
.STACK 200h

MAXTEXTSIZE equ 50

.DATA
; definicion de constantes float y enteras
	_float_n_1_5                       	DD	-1.5
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

; definicion de constantes string
	_strh_7a1c4c5                      	DB  "---- TEMA  triangleAreaMaximum ----",'$'
	s@_strh_7a1c4c5                   	EQU ($ - _strh_7a1c4c5)
	_strh_3f6361cc                     	DB  "@sdADaSjfla%dfg",'$'
	s@_strh_3f6361cc                  	EQU ($ - _strh_3f6361cc)
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

	@sys_RORD_168                      	DB	"[9-x,x+3,1+1]",'$'

	@tmp	DD ?
	_float_p_0_5_	DD 0.5
	@tmp_area1	DD ?
	@tmp_area2	DD ?
	_ncalc_args_192	DD	5 DUP(?)
	_ncalc_temp_192	DD	?
	_ncalc_sum_192	DD	0.0
	_ncalc_prod_192	DD	1.0
	_ncalc_count_192	DD	0
	_ncalc_res_192	DD	0.0
	_ncalc_args_202	DD	4 DUP(?)
	_ncalc_temp_202	DD	?
	_ncalc_sum_202	DD	0.0
	_ncalc_prod_202	DD	1.0
	_ncalc_count_202	DD	0
	_ncalc_res_202	DD	0.0

; definicion de variables
	bufferIn	DB	20,?,20 DUP('$')
	@usr_a                             	DD  ?
	@usr_a1                            	DD  ?
	@usr_areaMax                       	DD  ?
	@usr_b                             	DB	MAXTEXTSIZE dup (?),'$'
	@usr_b1                            	DD  ?
	@usr_c                             	DD  ?
	@usr_contador                      	DD  ?
	@usr_d                             	DD  ?
	@usr_f                             	DD  ?
	@usr_h                             	DD  ?
	@usr_j                             	DD  ?
	@usr_numeroFloat                   	DD  ?
	@usr_p1                            	DB	MAXTEXTSIZE dup (?),'$'
	@usr_p2                            	DB	MAXTEXTSIZE dup (?),'$'
	@usr_p3                            	DB	MAXTEXTSIZE dup (?),'$'
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
	FMULP
	FSTP @tmp
	FLD @tmp
	FLD @usr_a
	FADDP
	FSTP @tmp
	FLD @tmp
	FSTP @usr_x1
	DisplayFloat @usr_x1,2
	newLine 1
	FLD _float_p_2_5
	FSTP @usr_b1
	FLD _int_p_10
	FLD @usr_b1
	FDIVP
	FSTP @tmp
	FLD @tmp
	FLD @usr_a
	FADDP
	FSTP @tmp
	FLD @tmp
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
	FADDP
	FSTP @tmp
	FLD @tmp
	FSTP @usr_c
	JMP TAG_111
TAG_126:
	FLD _float_n_1_5
	FSTP @usr_b1
	DisplayFloat @usr_b1,2
	newLine 1
	FLD _int_p_3
	FSTP @usr_r
	FLD _int_p_2
	FSTP @usr_j
	FLD _int_p_27
	FLD @usr_c
	FSUBP
	FSTP @tmp
	FLD @tmp
	FSTP @usr_x
	DisplayFloat @usr_x,2
	newLine 1
	FLD @usr_r
	FLD _int_p_500
	FADDP
	FSTP @tmp
	FLD @tmp
	FSTP @usr_x
	FLD _int_p_34
	FLD _int_p_3
	FMULP
	FSTP @tmp
	FLD @tmp
	FSTP @usr_x
	FLD @usr_z
	FLD @usr_f
	FDIVP
	FSTP @tmp
	FLD @tmp
	FSTP @usr_x1
	FLD @usr_r
	FLD @usr_j
	FMULP
	FSTP @tmp
	FLD @tmp
	FLD _int_p_2
	FSUBP
	FSTP @tmp
	FLD @tmp
	FSTP @usr_x
	DisplayFloat @usr_x,2
	newLine 1
	displayString @sys_RORD_168
	newLine 1
	FLD _float_p_4_1
	FSTP @usr_a
	FLD _float_n_1_7
	FSTP @usr_f
;funcion NCALC_
	FLD _float_p_3_5
	FSTP DWORD PTR _ncalc_args_192 + 0
	FLD _float_n_2_0
	FSTP DWORD PTR _ncalc_args_192 + 4
	FLD @usr_a
	FSTP DWORD PTR _ncalc_args_192 + 8
	FLD @usr_f
	FSTP DWORD PTR _ncalc_args_192 + 12
	FLD _float_n_3_0
	FSTP DWORD PTR _ncalc_args_192 + 16
	FLD DWORD PTR _ncalc_args_192 + 0
	FSTP DWORD PTR _ncalc_temp_192
	MOV EAX, DWORD PTR _ncalc_temp_192
	TEST EAX, 80000000h
	JZ NCALC_192_NOTNEG_0
	FLD DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_temp_192
	FADD
	FSTP DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_prod_192
	FLD DWORD PTR _ncalc_temp_192
	FMUL
	FSTP DWORD PTR _ncalc_prod_192
	INC DWORD PTR _ncalc_count_192
NCALC_192_NOTNEG_0:
	FLD DWORD PTR _ncalc_args_192 + 4
	FSTP DWORD PTR _ncalc_temp_192
	MOV EAX, DWORD PTR _ncalc_temp_192
	TEST EAX, 80000000h
	JZ NCALC_192_NOTNEG_1
	FLD DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_temp_192
	FADD
	FSTP DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_prod_192
	FLD DWORD PTR _ncalc_temp_192
	FMUL
	FSTP DWORD PTR _ncalc_prod_192
	INC DWORD PTR _ncalc_count_192
NCALC_192_NOTNEG_1:
	FLD DWORD PTR _ncalc_args_192 + 8
	FSTP DWORD PTR _ncalc_temp_192
	MOV EAX, DWORD PTR _ncalc_temp_192
	TEST EAX, 80000000h
	JZ NCALC_192_NOTNEG_2
	FLD DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_temp_192
	FADD
	FSTP DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_prod_192
	FLD DWORD PTR _ncalc_temp_192
	FMUL
	FSTP DWORD PTR _ncalc_prod_192
	INC DWORD PTR _ncalc_count_192
NCALC_192_NOTNEG_2:
	FLD DWORD PTR _ncalc_args_192 + 12
	FSTP DWORD PTR _ncalc_temp_192
	MOV EAX, DWORD PTR _ncalc_temp_192
	TEST EAX, 80000000h
	JZ NCALC_192_NOTNEG_3
	FLD DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_temp_192
	FADD
	FSTP DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_prod_192
	FLD DWORD PTR _ncalc_temp_192
	FMUL
	FSTP DWORD PTR _ncalc_prod_192
	INC DWORD PTR _ncalc_count_192
NCALC_192_NOTNEG_3:
	FLD DWORD PTR _ncalc_args_192 + 16
	FSTP DWORD PTR _ncalc_temp_192
	MOV EAX, DWORD PTR _ncalc_temp_192
	TEST EAX, 80000000h
	JZ NCALC_192_NOTNEG_4
	FLD DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_temp_192
	FADD
	FSTP DWORD PTR _ncalc_sum_192
	FLD DWORD PTR _ncalc_prod_192
	FLD DWORD PTR _ncalc_temp_192
	FMUL
	FSTP DWORD PTR _ncalc_prod_192
	INC DWORD PTR _ncalc_count_192
NCALC_192_NOTNEG_4:
	MOV EAX, DWORD PTR _ncalc_count_192
	CMP EAX, 0
	JE NCALC_192_ZERO
	TEST EAX, 1
	JZ NCALC_192_EVEN
	FLD DWORD PTR _ncalc_prod_192
	JMP NCALC_192_END
NCALC_192_EVEN:
	FLD DWORD PTR _ncalc_sum_192
	JMP NCALC_192_END
NCALC_192_ZERO:
	FLDZ
NCALC_192_END:
	FSTP DWORD PTR _ncalc_res_192
	DisplayFloat _ncalc_res_192, 2
	newLine 1
	FLD DWORD PTR _ncalc_res_192
	FLD @tmp
	FSTP @usr_x1
	FLD _float_n_1_7
	FSTP @usr_b1
;funcion NCALC_
	FLD _float_n_4_0
	FSTP DWORD PTR _ncalc_args_202 + 0
	FLD _float_p_2_3
	FSTP DWORD PTR _ncalc_args_202 + 4
	FLD @usr_b1
	FSTP DWORD PTR _ncalc_args_202 + 8
	FLD _float_p_5_6
	FSTP DWORD PTR _ncalc_args_202 + 12
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
	FLD @tmp
	FSTP @usr_f
	displayString _strh_7a1c4c5
	newLine 1
	FLD _float_p_3_8
	FSTP @usr_z
	FLD _int_p_2
	FSTP @usr_x
	; --- TRIANGLE AREA MAXIMUM ---
	; Calcular área triángulo 1
	FLD _int_p_12
	FSUB _int_p_3
	FMUL _int_p_0
	FLD _int_p_3
	FSUB @usr_z
	FMUL _float_p_4_0
	FADD
	FLD @usr_z
	FSUB _int_p_12
	FMUL _float_p_2_5
	FADD
	FABS
	FLD _float_p_0_5_
	FMUL
	FSTP @tmp_area1
	; Calcular área triángulo 2
	FLD _int_p_0
	FSUB _int_p_2
	FMUL @usr_z
	FLD _int_p_2
	FSUB _int_p_0
	FMUL _int_p_6
	FADD
	FLD _int_p_0
	FSUB _int_p_0
	FMUL @usr_x
	FADD
	FABS
	FLD _float_p_0_5_
	FMUL
	FSTP @tmp_area2
	FLD @tmp_area2
	FLD @tmp_area1
	FCOMPP
	FSTSW AX
	SAHF
	JA mayor_area_233
	FLD @tmp_area2
	JMP fin_triarea_233
mayor_area_233:
	FLD @tmp_area1
fin_triarea_233:
	FSTP @tmp
	FLD @tmp
	FSTP @usr_areaMax
	DisplayFloat @usr_areaMax,2
	newLine 1

FINAL:
	MOV ah, 1 ; pausa, espera que oprima una tecla
	INT 21h ; AH=1 es el servicio de lectura
	MOV AX, 4C00h ; Sale del Dos
	INT 21h       ; Enviamos la interripcion 21h
END START ; final del archivo.
