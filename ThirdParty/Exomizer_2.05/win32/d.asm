; b3 [f0..87] nolit= 201 bytes
        ld      iy, 4208
        ld      a, 128
        ld      b, 52
        push    de
        cp      a
exinit: ld      c, 16
        jr      nz, exget4
        ld      de, 1
        ld      ixl, c
        defb    218
exgb4:  ld      a, (hl)
        dec     hl
exget4: adc     a, a
        jr      z, exgb4
        rl      c
        jr      nc, exget4
        ex      af, af'
        ld      a, c
        cp      8
        jr      c, exget5
        xor     136
exget5: inc     a
        ld      (iy-112), a
        push    hl
        ld      hl, 1
        ex      af, af'
        defb    210
exsetb: add     hl, hl
        dec     c
        jr      nz, exsetb
        ld      (iy-60), e
        ld      (iy-8), d
        add     hl, de
        ex      de, hl
        inc     iyl
        pop     hl
        dec     ixl
        djnz    exinit
        pop     de
exlit:  ldd
exloop: add     a, a
        jr      z, exgbm
        jr      c, exlit
exgbmc: ld      c, 111
exgeti: add     a, a
        jr      z, exgbi
exgbic: inc     c
        jr      c, exgeti
        ret     m
        push    de
        ld      iyl, c
        ld      de, 0
        ld      b, (iy-112)
        dec     b
        call    nz, exgbts
        ex      de, hl
        ld      c, (iy-60)
        ld      b, (iy-8)
        add     hl, bc
        ex      de, hl
        push    de
        ld      bc, 672
        dec     e
        jr      z, exgoit
        dec     e
        ld      bc, 1168
        jr      z, exgoit
        ld      c, 128
        ld      e, 0
exgoit: ld      d, e
        call    exlee8
        ld      iyl, c
        add     iy, de
        ld      e, d
        ld      b, (iy-112)
        dec     b
        call    nz, exgbts
        ex      de, hl
        ld      c, (iy-60)
        ld      b, (iy-8)
        add     hl, bc
        ex      de, hl
        pop     bc
        ex      (sp), hl
        ex      de, hl
        add     hl, de
        lddr
        pop     hl
        jr      exloop
exgbm:  ld      a, (hl)
        dec     hl
        adc     a, a
        jr      nc, exgbmc
        jp      exlit
exgbi:  ld      a, (hl)
        dec     hl
        adc     a, a
        jp      exgbic
exgbts: jp      p, exlee8
        ld      e, (hl)
        dec     hl
        rl      b
        ret     z
        srl     b
        defb    250
exxopy: ld      a, (hl)
        dec     hl
exl16:  adc     a, a
        jr      z, exxopy
        rl      d
        djnz    exl16
        ret
excopy: ld      a, (hl)
        dec     hl
exlee8: adc     a, a
        jr      z, excopy
        rl      e
        djnz    exlee8
        ret
