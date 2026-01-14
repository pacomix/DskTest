;Exomizer 2 Z80 decoder
; by Metalbrain
;
; optimized by Antonio Villena
;
; compression algorithm by Magnus Lind
;
;simple version:
;		no literal sequences (compress with -c option)
;		you MUST define exo_mapbasebits aligned to a 256 boundary

;input: 	hl=compressed data start
;		de=uncompressed destination start

deexo:		ld	iy,exo_mapbasebits
		ld	a,(hl)
		inc	hl
		ld	b,52
		push	de
exo_initbits:	ex	af,af'
		ld	a,b
		sub	4
		and	15
		jr	nz,exo_node1
		ld	de,1		;DE=b2
exo_node1:	ld	c,16
		ex	af,af'
exo_get4bits:	call	exo_getbit
		rl	c
		jr	nc,exo_get4bits
		ld	(iy+0),c	;bits[i]=b1
		push	hl
		ld	hl,1
		defb	210		;3 bytes nop (JP NC)
exo_setbit:	add	hl,hl
		dec	c
		jr	nz,exo_setbit
		ld	(iy+52),e
		ld	(iy+104),d	;base[i]=b2
		add	hl,de
		ex	de,hl
		inc	iy
		pop	hl
		djnz	exo_initbits
		pop	de
      		db	218		;3 bytes nop (JP C) --> jr exo_mainloop
exo_literalcopy:ldi
exo_mainloop:	call	exo_getbit	;literal?
		jr	c,exo_literalcopy
		ld	c,255
exo_getindex:	inc	c
		call	exo_getbit
		jr	nc,exo_getindex
		bit	4, c
		ret	nz
		push	de
		call	exo_getpair
		push	bc
		pop	ix
		ld	de,512+48	;2 bits, 48 offset
		inc	b
		djnz	exo_dontgo
		dec	c
		jr	z, exo_goforit
		dec	c		;2?
exo_dontgo:	ld	de,1024+32	;4 bits, 32 offset
		jr	z,exo_goforit
		ld	e,16		;16 offset
exo_goforit:	call	exo_getbits
		ex	af, af'
		ld	a,e
		add	a,c
		ld	c,a
		ex	af, af'
		call	exo_getpair	;bc=offset
		pop	de		;de=destination
		push	hl		
		ld	h,d
		ld	l,e
		sbc	hl,bc		;hl=origin
		push	ix
		pop	bc		;bc=lenght
		ldir
		pop	hl		;Keep HL, DE is updated
		jr	exo_mainloop	;Next!

exo_getpair:	ld	iyl,c
		ld	d,(iy+0)
		call	exo_getbits
		push	hl
		ld	l,(iy+52)
		ld	h,(iy+104)
		add	hl, bc		;Always clear C flag
      		ld	b, h
      		ld	c, l
		pop	hl
		ret

exo_getbits:	ld	bc,0		;get D bits in BC
exo_gettingbits:dec	d
		ret	m
		call	exo_getbit
		rl	c
		rl	b
		jr	exo_gettingbits

exo_getbit:	srl	a		;get one bit
		ret	nz
		ld	a,(hl)
		inc	hl
		rra
		ret