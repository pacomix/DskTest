; ******************************************************
; **       Librería de rutinas SDCC para Amstrad CPC  **
; **       Raúl Simarro (Artaburu)    -   2009, 2012  **
; ******************************************************


;*************************************
; SPRITES
;*************************************

.globl _cpc_PutSprite

_cpc_PutSprite::
;*************************************
; SPRITE ROUTINE WITHOUT TRANSPARENCY
; Supplied by Tim Riemann
; from a German forum
; DE = source address of the sprite
;      (includes header with 1B width [64byte maximum!], 1B height)
; HL = destination address
;*************************************

	POP AF
	POP HL	;DESTINATION ADDRESS
	POP DE	;SPRITE DATA
	PUSH AF
    ;EX DE,HL
    LD A,#64
    SUB (HL)
    ADD A
    LD (width1+1),A
    XOR A
    SUB (HL)
    LD (width2+1),A
    INC HL
    LD A,(HL)
    INC HL
width0:
		;ex de,hl
width1:
	JR width1 				;cada LDI es un byte
    LDI						;se hace el salto al byte correspondiente (64-ancho)
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
	LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
    LDI
width2:
   LD BC,#0X700
   EX DE,HL
   ADD HL,BC
   JP NC,width3
   LD BC,#0XC050
   ADD HL,BC
width3:
   EX DE,HL
   DEC A
   JP NZ, width1
   RET




.globl _cpc_PutSp

_cpc_PutSp::	; dibujar en pantalla el sprite
		; Entradas	bc-> Alto Ancho
		;			de-> origen
		;			hl-> destino
		; Se alteran hl, bc, de, af

	LD IX,#2
	ADD IX,SP
	LD E,0 (IX)
	LD D,1 (IX)
	LD A,3 (IX)
   	LD L,4 (IX)
	LD H,5 (IX)

    LD (#ancho0+1),A		;actualizo rutina de captura
    ;ld (anchot+1),a	;actualizo rutina de dibujo
	SUB #1
	CPL
	LD (#suma_siguiente_linea0+1),A    ;COMPARTEN LOS 2 LOS MISMOS VALORES.

	LD A,2 (IX)
	;JP cpc_putsp0





pc_PutSp0:
	.DB #0XFD
	LD H,A		;ALTO, SE PUEDE TRABAJAR CON HX DIRECTAMENTE
	LD B,#7
ancho0:
loop_alto_2_pc_PutSp0:
	LD C,#4
loop_ancho_2_pc_PutSp0:
	LD A,(DE)
	LD (HL),A
	INC DE
	INC HL
	DEC C
	JP NZ,loop_ancho_2_pc_PutSp0
	.DB #0XFD
	DEC H
	RET Z

suma_siguiente_linea0:
salto_linea_pc_PutSp0:
	LD C,#0XFF			;&07F6 			;SALTO LINEA MENOS ANCHO
	ADD HL,BC
	JP nc,loop_alto_2_pc_PutSp0 ;sig_linea_2zz		;si no desborda va a la siguiente linea
	LD BC,#0XC050

	ADD HL,BC
	LD B,#7			;SÓLO SE DARÍA UNA DE CADA 8 VECES EN UN SPRITE
	JP loop_alto_2_pc_PutSp0


.globl _cpc_PutSp4x14

_cpc_PutSp4x14::	; dibujar en pantalla el sprite
		; Entradas	bc-> Alto Ancho
		;			de-> origen
		;			hl-> destino
		; Se alteran hl, bc, de, af

	LD IX,#2
	ADD IX,SP
	LD e,0 (IX)
	LD d,1 (IX) ;sprite
   	LD l,2 (IX)
	LD h,3 (IX) ;address
	ld A,#14

pc_PutSp0X:
	.DB #0XFD
	LD H,A		;ALTO, SE PUEDE TRABAJAR CON HX DIRECTAMENTE
	LD B,#7
ancho0X:
loop_alto_2_pc_PutSp0X:
	LD C,#4
loop_ancho_2_pc_PutSp0X:
	EX DE,HL
	LDI
	LDI
	LDI
	LDI
	EX DE,HL
	;LD A,(DE)
	;LD (HL),A
	;INC DE
	;INC HL
	;DEC C
	;JP NZ,loop_ancho_2_pc_PutSp0X
	.DB #0XFD
	DEC H
	RET Z

suma_siguiente_linea0X:
salto_linea_pc_PutSp0X:
	LD C,#0XFC			;&07F6 			;SALTO LINEA MENOS ANCHO
	ADD HL,BC
	JP nc,loop_alto_2_pc_PutSp0X ;sig_linea_2zz		;si no desborda va a la siguiente linea
	LD BC,#0XC050
	ADD HL,BC
	LD B,#7			;SÓLO SE DARÍA UNA DE CADA 8 VECES EN UN SPRITE
	JP loop_alto_2_pc_PutSp0X




.globl _cpc_PutSpXOR

_cpc_PutSpXOR::	; dibujar en pantalla el sprite
	; Entradas	bc-> Alto Ancho
	;			de-> origen
	;			hl-> destino
	; Se alteran hl, bc, de, af

	LD IX,#2
	ADD IX,SP
	LD E,0 (IX)
	LD D,1 (IX)
	LD A,3 (IX)
   	LD L,4 (IX)
	LD H,5 (IX)

    LD (#anchox0+#1),A		;actualizo rutina de captura
	SUB #1
	CPL
	LD (#suma_siguiente_lineax0+#1),A    ;comparten los 2 los mismos valores.

	LD A,2 (IX)
	JP cpc_PutSpXOR0


.globl _cpc_PutSpriteXOR

_cpc_PutSpriteXOR::	; dibujar en pantalla el sprite
		; Entradas	bc-> Alto Ancho
		;			de-> origen
		;			hl-> destino
		; Se alteran hl, bc, de, af
	POP AF
	POP HL
	POP DE
	PUSH AF
	LD A,(HL)		;ANCHO
	INC HL
    LD (#anchox0+#1),A		;ACTUALIZO RUTINA DE CAPTURA
    ;LD (ANCHOT+1),A	;ACTUALIZO RUTINA DE DIBUJO
	SUB #1
	CPL
	LD (#suma_siguiente_lineax0+1),A    ;COMPARTEN LOS 2 LOS MISMOS VALORES.
	LD A,(HL)	;ALTO
	INC HL
	EX DE,HL
	;LD A,(IX+4)
	JP cpc_PutSpXOR0


cpc_PutSpXOR0:
	.DB #0XFD
	LD H,A		;ALTO, SE PUEDE TRABAJAR CON HX DIRECTAMENTE
	LD B,#7
anchox0:
loop_alto_2x:
	LD C,#4
loop_ancho_2x:
	LD A,(DE)
	XOR (HL)
	LD (HL),A
	INC DE
	INC HL
	DEC C
	JP NZ,loop_ancho_2x
	.DB #0XFD
	DEC H
	RET Z

suma_siguiente_lineax0:
salto_lineax:
	LD C,#0XFF			;&07F6 			;SALTO LINEA MENOS ANCHO
	ADD HL,BC
	JP NC,loop_alto_2x ;SIG_LINEA_2ZZ		;SI NO DESBORDA VA A LA SIGUIENTE LINEA
	LD BC,#0XC050
	ADD HL,BC
	LD B,#7			;SÓLO SE DARÍA UNA DE CADA 8 VECES EN UN SPRITE
	JP loop_alto_2x




.globl _cpc_PutSpTr

_cpc_PutSpTr::	; dibujar en pantalla el sprite
		; Entradas	bc-> Alto Ancho
		;			de-> origen
		;			hl-> destino
		; Se alteran hl, bc, de, af
	LD IX,#2
	ADD IX,SP
	LD E,0 (IX)
	LD D,1 (IX)
	LD A,4 (IX)
   	LD L,6 (IX)
	LD H,7 (IX)


    LD (#anchot+1),A	;actualizo rutina de dibujo
	SUB #1
	CPL
	LD (#suma_siguiente_lineat+1),A    ;comparten los 2 los mismos valores.

	LD A,2 (IX)
	;JP  cpc_PutSpTr0

cpc_PutSpTr0:
	.DB #0XFD
	LD H,A		;ALTO, SE PUEDE TRABAJAR CON HX DIRECTAMENTE
anchot:
loop_alto_2t:
	LD B,#0
loop_ancho_2t:
	LD A,(DE)
	AND #0XAA
	JP Z,sig_pixn_der_2
	LD C,A ;B es el único registro libre
	LD A,(HL) ;pixel actual donde pinto
	AND #0X55
	OR C
	LD (HL),A ;y lo pone en pantalla
sig_pixn_der_2:
	LD A,(DE) ;pixel del sprite
	AND #0X55
	JP Z,pon_buffer_der_2
	LD C,A ;B es el único registro libre
	LD A,(HL) ;PIXEL ACTUAL DONDE PINTO
	AND #0XAA
	OR C
	LD (HL),A
pon_buffer_der_2:
	INC DE
	INC HL
	DEC B
	JP NZ,loop_ancho_2t
	.DB #0XFD
	DEC H
	RET Z
suma_siguiente_lineat:
salto_lineat:
	LD BC,#0X07FF			;&07f6 			;salto linea menos ancho
	ADD HL,BC
	JP NC,loop_alto_2t ;sig_linea_2zz		;si no desborda va a la siguiente linea
	LD BC,#0XC050
	ADD HL,BC
	;ld b,7			;sólo se daría una de cada 8 veces en un sprite
	JP loop_alto_2t

	LD A,H
	ADD #0X08
	LD H,A
	SUB #0XC0
	JP NC,loop_alto_2t ;sig_linea_2
	LD BC,#0XC050
	ADD HL,BC
	JP loop_alto_2t




.globl _cpc_GetSp

_cpc_GetSp::

	LD IX,#2
	ADD IX,SP
	LD E,0 (IX)
	LD D,1 (IX)
	LD A,3 (IX)
   	LD L,4 (IX)
	LD H,5 (IX)



	LD (#loop_alto_2x_GetSp0+1),A


	SUB #1
	CPL
	LD (#salto_lineax_GetSp0+1),A    ;comparten los 2 los mismos valores.

	LD A,2 (IX)
	;JP cpc_GetSp0

cpc_GetSp0::
	.DB #0XFD
	LD H,A		;ALTO, SE PUEDE TRABAJAR CON HX DIRECTAMENTE
	LD B,#7
loop_alto_2x_GetSp0:
	LD C,#0
loop_ancho_2x_GetSp0:
	LD A,(HL)
	LD (DE),A
	INC DE
	INC HL
	DEC C
	JP NZ,loop_ancho_2x_GetSp0
	.DB #0XFD
	DEC H
	RET Z
salto_lineax_GetSp0:
	LD C,#0XFF					;salto linea menos ancho
	ADD HL,BC
	JP NC,loop_alto_2x_GetSp0 	;sig_linea_2zz		;si no desborda va a la siguiente linea
	LD BC,#0XC050
	ADD HL,BC
	LD B,#7						;sólo se daría una de cada 8 veces en un sprite
	JP loop_alto_2x_GetSp0






;void  						cpc_PutMaskSprite(int *sprite, int *posicion);
;void    					cpc_PutMaskSp(int *sprite, char alto, char ancho, int *posicion);
.globl _cpc_PutMaskSp

_cpc_PutMaskSp::	; dibujar en pantalla el sprite
		; Entradas	bc-> Alto Ancho
		;			de-> origen
		;			hl-> destino
		; Se alteran hl, bc, de, af

	LD IX,#2
	ADD IX,SP
	LD L,4 (IX)
	LD H,5 (IX)
	LD A,3 (IX)
   	LD E,0 (IX)
	LD D,1 (IX)
    ld (#loop_alto_2m_PutMaskSp0+#1),a		;actualizo rutina de captura
	SUB #1
	CPL
	LD (#salto_lineam_PutMaskSp0+#1),A    ;comparten los 2 los mismos valores.
	ld A,2(IX)
	;JP cpc_PutMaskSp0

cpc_PutMaskSp0:
	.DB #0XFD
	LD H,A		;ALTO, SE PUEDE TRABAJAR CON HX DIRECTAMENTE
	LD B,#7
loop_alto_2m_PutMaskSp0:
	LD C,#4
	EX DE,HL
loop_ancho_2m_PutMaskSp0:
	LD A,(DE)	;LEO EL BYTE DEL FONDO
	AND (HL)	;LO ENMASCARO
	INC HL
	OR (HL)		;LO ENMASCARO
	LD (DE),A	;ACTUALIZO EL FONDO
	INC DE
	INC HL
	DEC C
	JP NZ,loop_ancho_2m_PutMaskSp0
	.DB #0XFD
	DEC H
	RET Z
	EX DE,HL
salto_lineam_PutMaskSp0:
	LD C,#0XFF
	ADD HL,BC
	JP nc,loop_alto_2m_PutMaskSp0
	LD BC,#0XC050
	ADD HL,BC
	LD B,#7
	JP loop_alto_2m_PutMaskSp0







.globl _cpc_PutMaskSprite

_cpc_PutMaskSprite::	; dibujar en pantalla el sprite
		; Entradas	bc-> Alto Ancho
		;			de-> origen
		;			hl-> destino
		; Se alteran hl, bc, de, af

	POP AF
	POP HL
	POP DE
	PUSH AF
	LD A,(HL)		;ANCHO
	INC HL
    ld (#loop_alto_2m_PutMaskSp0+#1),a		;ACTUALIZO RUTINA DE CAPTURA
    ;LD (ANCHOT+1),A	;ACTUALIZO RUTINA DE DIBUJO
	SUB #1
	CPL
	LD (#salto_lineam_PutMaskSp0+#1),A    ;COMPARTEN LOS 2 LOS MISMOS VALORES.
	LD A,(HL)	;ALTO
	INC HL
	EX DE,HL
	jp cpc_PutMaskSp0




.globl _cpc_PutMaskSp2x8
; imprime un sprite de 8x8 en modo 1
; El formato del sprite es el siguiente por cada línea:
; defb byte1,byte2,byte3,byte4
; siendo byte1 y byte3 son las máscaras de los bytes 2 y 4
; se recibe de entrada el sprite y la posición.
_cpc_PutMaskSp2x8::
	LD IX,#2
	ADD IX,SP
	LD L,2 (IX)
	LD H,3 (IX)
	LD E,0 (IX)
	LD D,1 (IX)
	.DB #0XFD
	LD H,#8		;ALTO, SE PUEDE TRABAJAR CON HX DIRECTAMENTE
	LD B,#7
loop_alto_mask_2x8:
	EX DE,HL
	LD A,(DE)	;leo el byte del fondo
	AND (HL)	;lo enmascaro
	INC HL
	OR (HL)		;lo enmascaro
	LD (DE),A	;actualizo el fondo
	INC DE
	INC HL
	;COMO SOLO SON 2 BYTES, es más rápido y económico desplegar la rutina
	LD A,(DE)
	AND (HL)
	INC HL
	OR (HL)
	LD (DE),A
	INC DE
	INC HL
	.DB #0XFD
	DEC H
	RET Z
	EX DE,HL
	LD C,#0XFE
	ADD HL,BC
	JP NC,loop_alto_mask_2x8
	LD BC,#0XC050
	ADD HL,BC
	LD B,#7
	JP loop_alto_mask_2x8



.globl _cpc_PutMaskSp4x16

_cpc_PutMaskSp4x16::

	LD IX,#2
	ADD IX,SP
	LD L,2 (IX)
	LD H,3 (IX)
	LD E,0 (IX)
	LD D,1 (IX)
	.DB #0XFD
	LD H,#16
	LD B,#7
loop_alto_mask_4x16:
	EX DE,HL
	LD A,(DE)	;leo el byte del fondo
	AND (HL)	;lo enmascaro
	INC HL
	OR (HL)		;lo enmascaro
	LD (DE),A	;actualizo el fondo
	INC DE
	INC HL
	;COMO SOLO SON 4 BYTES, es más rápido y económico desplegar la rutina
	LD A,(DE)	;leo el byte del fondo
	AND (HL)	;lo enmascaro
	INC HL
	OR (HL)		;lo enmascaro
	LD (DE),A	;actualizo el fondo
	INC DE
	INC HL
	LD A,(DE)	;leo el byte del fondo
	AND (HL)	;lo enmascaro
	INC HL
	OR (HL)		;lo enmascaro
	LD (DE),A	;actualizo el fondo
	INC DE
	INC HL
	LD A,(DE)	;leo el byte del fondo
	AND (HL)	;lo enmascaro
	INC HL
	OR (HL)		;lo enmascaro
	LD (DE),A	;actualizo el fondo
	INC DE
	INC HL
	.DB #0XFD
	DEC H
	RET Z
	EX DE,HL
	LD C,#0XFC
	ADD HL,BC
	JP NC,loop_alto_mask_4x16
	LD BC,#0XC050
	ADD HL,BC
	LD B,#7
	JP loop_alto_mask_4x16
