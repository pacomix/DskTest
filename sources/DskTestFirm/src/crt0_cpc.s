;; FILE: crt0.s
;; Based on the crt0.s by H. Hansen 2003 provided in SDCC

  .module crt0

  .globl	s__INITIALIZER
  .globl	l__INITIALIZER
  .globl	s__INITIALIZED

	.globl	_main

	.area _HEADER (ABS)
  .area _BOOT (ABS)

  ;; Reset vector
	.org 	0x4000
__bootstrap::   ; Tag used by the build script to identify the load/entry point.
	jp	init

;;_g_sysint:: .dw #0xb941
;; Variables defined below with initialized value. Declare that in the C code
;; without the _ prefix as follows:
;;
;;    extern (unsigned|signed) (char|integer) varname ([__SIZE__]);
;;
;; For example:
;;    extern unsigned char uTrack;
;;    extern signed integer g_sTime;
;;    extern unsigned char g_szBytes[6];
;;
_old_int:: .dw #0x0000
_uKeyPressed:: .db #0x00
_uSelectedOption:: .db #0x00
_uTrack:: .db #0x10
_uSectorID:: .db #0xC1
_uFoundErrorSectorID:: .db #0x00
_uMotor:: .db #0x00
_uDrive:: .db #0x00
_uRPMs:: .dw #0x0000
_uRPMsDec:: .db #0x00
_uLoops:: .dw #0x0000
_uPartialSecs:: .db #0x02
_uPartialInts:: .db #0x0258
_g_szBytes:: .db #0x00,#0x00,#0x00,#0x00,#0x00,#0x00
_g_sTime:: .dw #0x0000

_g_realTime:: .ds 5
_g_realLoops:: .ds 5
_g_realConstant18000:: .ds 5  ; TODO - precalc this one
_g_realHalf:: .ds 5           ; TODO - precalc this one

.macro  CHAR_ACCENT c1
  .ascii c1
  .db #0x08, #0x16, #0x01
  .db #0xA1
  .db #0x16, #0x02
.endm

.macro CURSOR_AT x,y
  .db #0x1F, x, y
.endm

.macro CURSOR_RESET
  .db #0x1E
.endm

.macro ASCII_AT  x,y,txt
  CURSOR_AT x, y
  .ascii txt
.endm

.macro SCR_MODE x
  .db #0x04, x
.endm

_szInfoMsg::  SCR_MODE 2
              ASCII_AT 32, 16, ^|"::: WARNING :::"|
              ASCII_AT 28, 18, ^|"USE IT AT YOUR OWN RISK"|

              CURSOR_AT 1, 24
              .ascii "DskTest v2.0-RC1\r\nFrancisco Jos"
              CHAR_ACCENT ^|"e"|
              .ascii " <PACOMIX> S"
              CHAR_ACCENT ^|"a"|
              .ascii "nchez - https://linkedin.com/in/pacomix"

_szOptions::  CURSOR_RESET
              .ascii  " DRIVE:\r\n"

              .ascii  " MOTOR:\r\n"
              .ascii  " TRACK:\r\n"

              .ascii  " SECTOR:"
              ASCII_AT 28,4,^|"FOUND:\r\n"|

              .ascii  " RPMs. :\r\n"
              .ascii  " UpdSec:\r\n"

              .asciz  ""


  init:
    call  #0xBCCB   ;; KL ROM WALK
    call  gsinit    ;; Initialise global variables
	  call _main      ;; and call our entry point.
	  jp   _exit      ;; Exit our program finally

	;; Ordering of segments for the linker.
	.area	_HOME
	.area	_CODE
  .area _INITIALIZER
  .area   _GSINIT
  .area   _GSFINAL

	.area	_DATA
  .area _INITIALIZED
  .area   _BSS
  .area   _HEAP

  .area   _CODE
__clock::
	ret

_exit::
	ret

	.area   _GSINIT
gsinit::
  ; Inicializa los valores de las variables globales.
  ld	bc, #l__INITIALIZER
  ld	a, b
  or	a, c
  jr	Z, gsinit_next
  ld	bc, #l__INITIALIZER
  ld	de, #s__INITIALIZED
  ld	hl, #s__INITIALIZER
  ldir
gsinit_next:
.area   _GSFINAL
    	ret
