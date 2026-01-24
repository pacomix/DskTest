// Copy a real (float) value to another place.
// HL - Target address where the value will be copied.
// DE - Source address where the value is stored (5 bytes)
// only CPC464: A exponent byte
// Exit: HL-target address Carry TRUE F-corrupt
#ifdef CPC_MODEL_464
#  define MOVE_REAL 0xBD3D
#else
#  define MOVE_REAL 0xBDC1
#endif
void firm_move_real(U8* pDest, U8* pSrc) __sdcccall(1) {
  (void) pDest;
  (void) pSrc;

__asm
  call MOVE_REAL
__endasm;
}

// Converts an integer value into real (float)
// HL - Integer value.
// DE - Target address where the result will be stored (5 bytes)
// A - b7-1=Integer negative 0=Integer positive
// only CPC464: A exponent byte
// Exit: HL-target address AF|DE corrupt
#ifdef CPC_MODEL_464
#  define INTEGER_TO_REAL 0xBD40
#else
#  define INTEGER_TO_REAL 0xBD64
#endif
void firm_integer_to_real(U16 uInteger, U8* pDest) __sdcccall(1) {
  (void) uInteger;
  (void) pDest;

__asm
  push af
  res   7,  a
  call INTEGER_TO_REAL
  pop af
__endasm;
}


// Converts a real value into integer (16bits) (float)
// HL - pointer to array of real value. 5bytes
// Exit: HL-integer value rounded up or down depending on the decimal content of
//   the entry real value.
#ifdef CPC_MODEL_464
#  define REAL_TO_INTEGER 0xBD46
#else
#  define REAL_TO_INTEGER 0xBD6A
#endif
U16 firm_real_to_integer(U8* pReal) __sdcccall(1) {
  (void) pReal;

__asm
  call  REAL_TO_INTEGER
  ex    de,hl
__endasm;
}


//
// Subtract the first real value from the second IN PLACE of the first one.
// HL - pointer to array of the first real value. 5bytes
// DE - pointer to array of the second real value. 5bytes
//
// Exit: HL-integer value rounded up or down depending on the decimal content of
//   the entry real value.
#ifdef CPC_MODEL_464
#  define REAL_SUB 0xBD5E
#else
#  define REAL_SUB 0xBD82
#endif
void firm_real_sub(U8* pFirst, U8* pSecond) __sdcccall(1) {
  (void) pFirst;
  (void) pSecond;

__asm
  call  REAL_SUB
__endasm;
}

//
// Add the first real value to the second IN PLACE of the first one.
// HL - pointer to array of the first real value. 5bytes
// DE - pointer to array of the second real value. 5bytes
//
// Exit: HL-integer value rounded up or down depending on the decimal content of
//   the entry real value.
#ifdef CPC_MODEL_464
#  define REAL_ADD 0xBD58
#else
#  define REAL_ADD 0xBD7C
#endif
void firm_real_add(U8* pFirst, U8* pSecond) __sdcccall(1) {
  (void) pFirst;
  (void) pSecond;

__asm
  call  REAL_ADD
__endasm;
}


// Converts IN PLACE an integer value into real (float)
// HL - Source address where the value is stored (5 bytes)
// Exit: HL-target address of the INT value (4 byte) B b7-sign bit
//   AF|B|IX corrupt
#ifdef CPC_MODEL_464
#  define REAL_INT 0xBD4F
#else
#  define REAL_INT 0xBD73
#endif
U16* firm_real_int(U8* uSrcReal) __sdcccall(1) {
  (void) uSrcReal;

__asm
  call REAL_INT
  ex  de, hl
  inc de
  inc de
__endasm;
}

// Multiplies IN PLACE of the first number two real (float) values
// HL - Source address of the first real and where the value is stored (5 bytes)
// DE - Source address of the second real (float) value. Unaltered. (5 bytes)
// Exit: AF|BC|DE|IX|IY corrupt
#ifdef CPC_MODEL_464
#  define REAL_MULTIPLICATION 0xBD61
#else
#  define REAL_MULTIPLICATION 0xBD85
#endif
void firm_real_multiplication(U8* pSrcReal, U8* pSecondReal) __sdcccall(1) {
  (void) pSrcReal;
  (void) pSecondReal;

__asm
  call REAL_MULTIPLICATION
__endasm;
}

// Divides IN PLACE of the first number two real (float) values
// HL - Source address of the first real and where the value is stored (5 bytes)
// DE - Source address of the second real (float) value. Unaltered. (5 bytes)
// Exit: AF|BC|DE|IX|IY corrupt
#ifdef CPC_MODEL_464
#  define REAL_DIVISION 0xBD64
#else
#  define REAL_DIVISION 0xBD88
#endif
void firm_real_division(U8* pSrc, U8* pSecond) __sdcccall(1) {
  (void) pSrc;
  (void) pSecond;

__asm
  call REAL_DIVISION
__endasm;
}
// TODO - Cambiar esto. Usar la jumpblock en lugar de las direcciones directas al firm...
