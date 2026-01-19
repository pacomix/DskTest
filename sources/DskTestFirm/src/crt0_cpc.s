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
_uFoundErrorSectorID:: .db #0x01
_uMotor:: .db #0x00
_uDrive:: .db #0x00
_uRPMs:: .dw #0x0000
_uRPMsDec:: .db #0x00
_uLoops:: .dw #0x0005
_uElapsedSeconds:: .dw #0x0000
_g_szBytes:: .db #0x00,#0x00,#0x00,#0x00,#0x00,#0x00
_g_sTime:: .dw #0x012C
_g_uX:: .db #0x00
_g_uY:: .db #0x00
_uDrives:: .db #0x00,#0x00

_g_realTime:: .ds 5
_g_realLoops:: .ds 5
_g_realConstant18000:: .ds 5  ; TODO - precalc this one
_g_realHalf:: .ds 5           ; TODO - precalc this one

_szOption0:: .asciz "DRIVE:"
_szOption1:: .ascii "TOGGLE MOTOR:  ("
             .db    #0xFF
             .asciz ")"

_szOption2:: .ascii "SEEK TRACK:    ("
             .db    #0xFF
             .asciz ")"

_szOption3:: .asciz "CALIBRATE"
_szOption4:: .ascii "FIND SEC. ID:  ("
             .db    #0xFF
             .asciz ")\tFOUND:"
_szOption5:: .asciz "MEASURE SPIN:\t     RPMs"

_p_szOptions:: .dw #_szOption0
               .dw #_szOption1
               .dw #_szOption2
               .dw #_szOption3
               .dw #_szOption4
               .dw #_szOption5

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
