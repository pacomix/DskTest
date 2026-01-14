//
// Calculates and return the screen start address of the specified line taking
// a screen base start address. Usually 0x0000 | 0x4000 | 0x8000 | 0xC000
//
U8 *GetLineAddress(U8 nLine, U16 uBase) {
  return (U8 *)uBase + ((nLine / 8) * CPC_SCR_LINE_WIDTH_BYTES) + ((nLine % 8) * 2048);
}


//
// Initializes the screen mode without using the firmware routines by writting
// straight to the CRTC controller.
// Params:
//   uMode - Screen mode - 0|1|2|3
//
void InitModeNoFirm(U8 uMode) {
  cpc_SetMode(uMode);

#if defined (CPC_MODE0_128)
  __asm
  ; Ancho de pantalla en caracteres (32)
  ld bc,#0xbc01
  out (c),c
  ld bc,#0xbd00+32
  out (c),c

  ; Posici�n horizontal de la pantalla
  ld bc,#0xbc02
  out (c),c
  ld bc,#0xbd00+42
  out (c),c

  ; Altura de la pantalla en caracteres (32)
  ld bc,#0xbc06
  out (c),c
  ld bc,#0xbd00+32
  out (c),c

  ; Posici�n vertical de la pantalla
  ld bc,#0xbc07
  out (c),c
  ld bc,#0xbd00+34
  out (c),c

  ; Cada car�cter del CRTC tendr� 8 p�xeles de altura
  ld bc,#0xbc09
  out (c),c
  ld bc,#0xbd00+7
  out (c),c

  ;// Ajuste fino vertical a trav�s del registro 5 del crtc
  ;ld bc, #0xbc05				;; select CRTC register 05
  ;out (c),c
  ;ld b, #0xbd				;; B = I/O address for CRTC register write
  ;ld a, #0x09
  ;out (c),a

  __endasm;
#endif
}


//
// Set the palette colours not using the firmware.
// Params:
//  pPalette[CPC_MODE_COLORS_MAX] - char array with the colours to set for each
//                                  of the pens.
//
void SetPaletteNoFirm(const U8* pPalette) {
  U8 nColor;

  for(nColor = 0; nColor < CPC_MODE_COLORS_MAX; nColor++) {
    cpc_SetColour(nColor, pPalette[nColor]);
  }
}

// Sprite drawing routines
#if defined (CPC_MODE0_128)
#  include "graphics_draw.inc"
#else
#  error No graphic mode defined!!!
#endif
