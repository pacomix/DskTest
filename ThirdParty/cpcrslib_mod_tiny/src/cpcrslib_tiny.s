.globl _cpc_SetMode
_cpc_SetMode::
	;ld a,l
	LD HL,#2
	ADD HL,SP
	LD L,(HL)				; Comprobar que el valor vaya a L!!
	LD BC,#0x7F00          ;Gate array port
	LD D,#140 ;@10001100	   ;Mode  and  rom  selection  (and Gate Array function)
	ADD D
	OUT (C),A
	RET

.globl  _cpc_SetColour
_cpc_SetColour::		;El número de tinta 17 es el borde
    LD HL,#2
    ADD HL,SP
  	LD A,(HL)
    INC HL
  	;INC HL
    LD E,(HL)
  	LD BC,#0x7F00                     ;Gate Array
	OUT (C),A                       ;Número de tinta
	LD A,#64 ;@01000000              	;Color (y Gate Array)
	ADD E
	OUT (C),A
	RET
  
.globl _cpc_DisableFirmware
_cpc_DisableFirmware::
	DI
	LD HL,(#0X0038)
	LD (backup_fw),HL
	LD HL,#0X0038
	LD (HL),#0XFB		;EI
	INC HL
	LD (HL),#0XC9		;RET
	EI
	RET
backup_fw:
	.DW  #0

.globl _cpc_ScanKeyboard
_cpc_ScanKeyboard::
    DI              ;1 #0X#0X%%#0X#0X C P C   VERSION #0X#0X%%#0X#0X   FROM CPCWIKI
    LD HL,#keymap    ;3
    LD BC,#0XF782     ;3
    OUT (C),C       ;4
    LD BC,#0XF40E     ;3
    LD E,B          ;1
    OUT (C),C       ;4
    LD BC,#0XF6C0     ;3
    LD D,B          ;1
    OUT (C),C       ;4
    LD C,#0          ;2
    OUT (C),C       ;4
    LD BC,#0XF792     ;3
    OUT (C),C       ;4
    LD A,#0X40        ;2
    LD C,#0X4A        ;2 44
loop_cpc_scankeyboard:
	LD B,D          ;1
    OUT (C),A       ;4 SELECT LINE
    LD B,E          ;1
    INI             ;5 READ BITS AND WRITE INTO KEYMAP
    INC A           ;1
    CP C            ;1
    JR C,loop_cpc_scankeyboard       ;2/3 9*16+1*15=159
    LD BC,#0XF782     ;3
    OUT (C),C       ;4
    EI              ;1 8 =211 MICROSECONDS
    RET

.globl _cpc_AssignKey
_cpc_AssignKey::
	LD HL,#2
  ADD HL,SP
	LD E,(HL)		;E-> numero tecla
	INC HL
	;INC HL
    LD A,(HL)					;linea, byte
    INC HL
    LD B,(HL)					;DE tiene el valor de la tecla a escribir en la tabla
								; En A se tiene el valor de la tecla seleccionada a comprobar [0..11]
								;___________________________________________________________________
								;	;En A viene la tecla a redefinir (0..11)
	SLA E
	LD D,#0
	LD HL, #tabla_teclas
	ADD HL,DE 					;Nos colocamos en la tecla a redefinir y la borramos
	LD (HL),#0XFF
	INC HL
	LD (HL),#0XFF
	DEC HL
	PUSH HL
								;call ejecutar_deteccion_teclado ;A tiene el valor del teclado
								; A tiene el byte (<>0)
								; B tiene la linea
								;guardo linea y byte
	POP HL
	LD (HL),A ;byte
	INC HL
	LD (HL),B
	RET

.globl _cpc_TestKeyF
_cpc_TestKeyF::
	LD HL,#2
    ADD HL,SP
    LD L,(HL)
	SLA L
	INC L
	LD H,#0
	LD DE,#tabla_teclas
	ADD HL,DE
	LD A,(HL)
	SUB #0X40
	EX DE,HL
	LD HL,#keymap	;; LEE LA LÍNEA BUSCADA DEL KEYMAP
	LD C,A
	LD B,#0
	ADD HL,BC
	LD A,(HL)
	EX DE,HL
	DEC HL						; PERO SÓLO NOS INTERESA UNA DE LAS TECLAS.
	AND (HL) ;PARA FILTRAR POR EL BIT DE LA TECLA (PUEDE HABER VARIAS PULSADAS)
	CP (HL)	;COMPRUEBA SI EL BYTE COINCIDE
	LD H,#0
	JP NZ,#pulsado_cpc_TestKeyF
	LD L,H
	RET
pulsado_cpc_TestKeyF:
	LD L,#1
	RET
  
  
  
  
  
  
  
  
  
  
; Used tables
keymap:
	.DB #0
	.DB #0
	.DB #0
	.DB #0
	.DB #0
	.DB #0
	.DB #0
	.DB #0
	.DB #0
	.DB #0

;tecla_0: .DW #0x0204
;teclado_usable					; teclas del cursor, cada tecla está definida por su bit y su línea.
tabla_teclas:
tecla_0_x: 	.DW #0x4002		; bit 0, línea 2
tecla_1_x: 	.DW #0x4101		; bit 1, línea 1
tecla_2_x: 	.DW #0x4001		; bit 0, línea 1
tecla_3_x: 	.DW #0x4004		; bit 0, línea 4
tecla_4_x:	.DW #0x4002		; bit 0, línea 2
tecla_5_x:  .DW #0x4101		; bit 1, línea 1
tecla_6_x:  .DW #0x4001		; bit 0, línea 1
tecla_7_x:  .DW #0x4004		; bit 0, línea 4
tecla_8_x:  .DW #0x4801		; bit 0, línea 4
tecla_9_x:  .DW #0x4802		; bit 0, línea 4
tecla_10_x:  .DW #0x4702		; bit 0, línea 4
tecla_11_x:  .DW #0x4204		; bit 0, línea 4
tecla_12_x:  .DW #0xffff		; bit 0, línea 4
tecla_13_x:  .DW #0x4204		; bit 0, línea 4
tecla_14_x:  .DW #0x4001		; bit 0, línea 4
tecla_15_x:  .DW #0x4004		; bit 0, línea 4
; For increasing keys available just increase this word table
.DB #0