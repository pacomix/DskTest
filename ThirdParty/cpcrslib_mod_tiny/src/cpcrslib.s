; ******************************************************
; **       Librería de rutinas SDCC para Amstrad CPC  **
; **       Raúl Simarro (Artaburu)    -   2009, 2012  **
; ******************************************************


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


.globl	_cpc_SetModo

_cpc_SetModo::
 	;LD A,L
  	LD HL,#2
 	ADD HL,SP
 	LD a,(HL)			; COMPROBAR QUE EL VALOR VAYA A L!!
   	JP 0XBC0E


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


.globl _cpc_SetInk

_cpc_SetInk::
	LD HL,#2
	ADD HL,SP
	LD A,(HL)
	INC HL

	LD B,(HL)
	LD C,B
	JP 0XBC32



.globl _cpc_SetBorder

_cpc_SetBorder::
	LD HL,#2
	ADD HL,SP
	LD B,(HL)
	;LD B,A
	LD C,B
	JP 0XBC38



.globl _cpc_Random

_cpc_Random::


 ;          LD A,(#valor_previo)
 ;           LD C,A
;			LD L,A
;			LD A,R;
;			ADD L
;            AND #0xB8
;            SCF
;            JP PO,NO_CLR
;            CCF
;NO_CLR:      LD A,C
 ;           RLA
 ;           LD C,A
 ;           LD A,R
 ;           ADD C
 ;           LD (#valor_previo),A
 ;           LD L,A
;            RET

	LD A,(#valor_previo)
	LD L,A
	LD A,R
	ADD L ;LOS 2 ÚLTIMOS BITS DE A DIRÁN SI ES 0,1,2,3
	LD (#valor_previo),A
	LD L,A ;SE DEVUELVE L (CHAR)
	LD H,#0
	RET
valor_previo:
	.db #0xFF





.globl _cpc_ClrScr

_cpc_ClrScr::
	XOR A
	LD HL,#0xC000
	LD DE,#0xC001
	LD BC,#16383
	LD (HL),A
	LDIR
	RET


.globl _cpc_PrintStr

_cpc_PrintStr::
	LD IX,#2
	ADD IX,SP
   	LD l,0 (IX)
	LD h,1 (IX)	;TEXTO ORIGEN

;	LD HL,#2
 ;   ADD HL,SP
;	LD E,(HL)
;	INC HL
;	LD D,(HL)
;	EX DE,HL
bucle_imp_cadena:
	LD A,(HL)
	OR A
	JR Z,salir_bucle_imp_cadena
	CALL #0XBB5A
	INC HL
	JR bucle_imp_cadena
salir_bucle_imp_cadena:
	LD A,#0X0D				; PARA TERMINAR HACE UN SALTO DE LÍNEA
	CALL #0XBB5A
	LD A,#0X0A
	JP 0XBB5A


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

.globl 	_cpc_EnableFirmware

_cpc_EnableFirmware::
	DI
	LD DE,(backup_fw)
	LD HL,#0X0038
	LD (HL),E			;EI
	INC HL
	LD (HL),D			;RET
	EI
	RET




.globl _cpc_GetScrAddress

_cpc_GetScrAddress::

;	LD IX,#2
;	ADD IX,SP
;	LD A,0 (IX)
;	LD L,1 (IX)	;pantalla


	LD HL,#2
    ADD HL,SP
	LD A,(HL)
	INC HL
	LD L,(HL)
	;LD L,E
	;JP cpc_GetScrAddress0


cpc_GetScrAddress0:			;en HL están las coordenadas

	;LD A,H
	LD (#inc_ancho+1),A
	LD A,L
	SRL A
	SRL A
	SRL A
	; A indica el bloque a multiplicar x &50
	LD D,A						;D
	SLA A
	SLA A
	SLA A
	SUB L
	NEG
	; A indica el desplazamiento a multiplicar x &800
	LD E,A						;E
	LD L,D
	LD H,#0
	ADD HL,HL
	LD BC,#bloques
	ADD HL,BC
	;HL APUNTA AL BLOQUE BUSCADO
	LD C,(HL)
	INC HL
	LD H,(HL)
	LD L,C
	;HL TIENE EL VALOR DEL BLOQUE DE 8 BUSCADO
	PUSH HL
	LD D,#0
	LD HL,#sub_bloques
	ADD HL,DE
	LD A,(HL)
	POP HL
	ADD H
	LD H,A
inc_ancho:
	LD E,#0
	ADD HL,DE
	RET

bloques:
.DW #0XC000,#0XC050,#0XC0A0,#0XC0F0,#0XC140,#0XC190,#0XC1E0,#0XC230,#0XC280,#0XC2D0,#0XC320,#0XC370,#0XC3C0,#0XC410,#0XC460,#0XC4B0,#0XC500,#0XC550,#0XC5A0,#0XC5F0,#0XC640,#0XC690,#0XC6E0,#0XC730,#0XC780
sub_bloques:
.DB #0X00,#0X08,#0X10,#0X18,#0X20,#0X28,#0X30,#0X38







.globl _cpc_RLI		;rota las líneas que se le digan hacia la izq y mete lo rotado por la derecha.

_cpc_RLI::
	LD IX,#2
	ADD IX,SP
	LD L,0 (IX)
	LD H,1 (IX)	;posición inicial
	LD A,2 (IX)	;lineas
	LD (alto_cpc_RLI+1),A
	LD A,3 (IX)	;ancho
	LD (ancho_cpc_RLI+1),A
	DEC HL
alto_cpc_RLI:
	LD A,#8					;; parametro
ciclo0_cpc_RLI:
	PUSH AF
	PUSH HL
	INC HL
	LD A,(HL)
	LD D,H
	LD E,L
	DEC HL
	LD B, #0
ancho_cpc_RLI:
	LD C,#50	; parametro
	LDDR
	INC HL
	LD (HL),A
	POP HL
	POP AF
	DEC A
	RET Z
	LD BC,#0X800	;salto de línea, ojo salto caracter.
	ADD HL,BC
	JP NC,ciclo0_cpc_RLI ;sig_linea_2zz		;si no desborda va a la siguiente linea
	LD BC,#0XC050
	ADD HL,BC
	JP ciclo0_cpc_RLI


.globl _cpc_RRI
;cpc_RRI(unsigned int pos, unsigned char w, unsigned char h);
_cpc_RRI::
	LD IX,#2
	ADD IX,SP
	LD L,0 (IX)
	LD H,1 (IX)	;posición inicial
	LD A,2 (IX)	;lineas
	LD (alto_cpc_RRI+1),A
	LD A,3 (IX)	;ancho
	LD (ancho_cpc_RRI+1),A
	INC HL
alto_cpc_RRI:
	LD A,#8					;; parametro
ciclo0_cpc_RRI:
	PUSH AF
	PUSH HL
	DEC HL
	LD A,(HL)
	LD D,H
	LD E,L
	INC HL		; SOLO MUEVE 1 BYTE
	LD B, #0
ancho_cpc_RRI:
	LD C,#50	; PARAMETRO
	LDIR
	DEC HL
	LD (HL),A
	POP HL
	POP AF
	DEC A
	RET Z
	LD BC,#0X800	;salto de línea, ojo salto caracter
	ADD HL,BC
	JP NC,ciclo0_cpc_RRI ;sig_linea_2zz		;si no desborda va a la siguiente linea
	LD BC,#0XC050
	ADD HL,BC
	JP ciclo0_cpc_RRI


	
.globl _cpc_CollSp
	
_cpc_CollSp::
;first parameter sprite 
;second parameter value
	ld hl,#2
	add hl,sp
	
	;ld ix,#2
	;add ix,sp
;	ld e,2 (ix)
;	ld d,3 (ix)
	;A=x value
;	ld l,0 (ix)
;	ld h,1 (ix)
	
	ld e,(hl)
	inc hl
	ld d,(hl)
	push de
	inc hl
	ld e,(hl)
	inc hl
	ld d,(hl)
	push de
	
	pop iy	;ix sprite2 data
  
    pop ix	;iy sprite1 data
    
    ;Sprite coords & sprite dims
				
;COLISION_sprites



;entran sprite1 y sprite 2 y se actualizan los datos
;ix apunta a sprite1
;iy apunta a sprite2

;coordenadas
	ld l,8 (ix)
	ld h,9 (ix)
	LD (#SPR2X),HL
	
	ld l,8 (iy)
	ld h,9 (iy)
	LD (#SPR1X),HL	

;dimensiones sprite 1
	ld l,0 (ix)
	ld h,1 (ix)
	ld b,(hl)
	inc hl
	ld c,(hl)
;dimensiones sprite 12
	ld l,0 (iy)
	ld h,1 (iy)
	ld d,(hl)
	inc hl
	ld e,(hl)	
	
	
	;ld e,(ix+6)
	;ld d,(ix+7)	
	
	

;ld de,DIMENSIONES_SP_PPAL	;dimensiones sprite 2
;ld bc,DIMENSIONES_SP_PPAL	;dimensiones sprite 1
CALL TOCADO
;RET NC ;vuelve si no hay colision
ld h,#0
JP nc,no_colision
;Aquí hay colisión
ld l,#1
RET		

no_colision:
ld l,h
ret
	
TOCADO:
	LD HL,#SPR2X	
	LD A,(#SPR1X)
	CP (HL)
	jp C,C1
	LD A,(HL)
	ADD A,B	;alto del sprite1
	LD B,A
	LD A,(#SPR1X)
	SUB B
	RET NC
	jp COMPROBAR
C1:
	ADD A,D	;alto sprite2
	LD D,A
	LD A,(HL)
	SUB D
	RET NC
COMPROBAR:
	INC HL
	LD A,(#SPR1Y)
	CP (HL)
	jp C,C2
	LD A,(HL)
	ADD A,C
	LD C,A
	LD A,(#SPR1Y)
	SUB C
	RET
C2:
	ADD A,E
	LD E,A
	LD A,(HL)
	SUB E
	RET

SPR1X: 
.db 0
SPR1Y: 
.db 0
SPR2X: 
.db 0
SPR2Y: 
.db 0


