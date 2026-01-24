#define DRAWCHAR_ROW_HIGH         \
  __asm ;Draw a row __endasm; \
  __asm ex de,hl    __endasm; \
  __asm ldi         __endasm; \
  __asm ldi         __endasm; \
  __asm             __endasm


// The #0x0002 is the width in bytes of a character
#define SCR_LINE_JUMP_CHAR                          \
  __asm ;Scr line jump                   __endasm;  \
  __asm ld bc, #0x0800-#0x0002            __endasm;  \
  __asm ex de, hl                        __endasm;  \
  __asm add hl,bc                        __endasm;  \
  __asm                                  __endasm


// REG hl pAddress
// REG de pSprite
void drawTile2x8Unrolled(U8* pAddress, U8* pSprite) {
  // TODO - Optimize this for code size by creating a loop.
  DRAWCHAR_ROW_HIGH;  SCR_LINE_JUMP_CHAR;
  DRAWCHAR_ROW_HIGH;  SCR_LINE_JUMP_CHAR;
  DRAWCHAR_ROW_HIGH;  SCR_LINE_JUMP_CHAR;
  DRAWCHAR_ROW_HIGH;  SCR_LINE_JUMP_CHAR;
  DRAWCHAR_ROW_HIGH;  SCR_LINE_JUMP_CHAR;
  DRAWCHAR_ROW_HIGH;  SCR_LINE_JUMP_CHAR;
  DRAWCHAR_ROW_HIGH;  SCR_LINE_JUMP_CHAR;
  DRAWCHAR_ROW_HIGH;
}
