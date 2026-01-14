
/* TEXT FUNCTIONS */
// Initializes the TEXT VDU whatever that be
// Exit: AF|BC|HL|DE corrupt
#define TXT_INITIALIZE 0xBB4E
void firm_txt_init(void) __sdcccall(1) {
__asm
  call TXT_INITIALIZE
__endasm;
}

// Resets the text VDU indirections and the control code table
// Exit: AF|BC|HL|DE corrupt
#define TXT_RESET 0xBB51
void firm_txt_reset(void) __sdcccall(1) {
__asm
  call TXT_RESET
__endasm;
}


// Allow chars to be printed on the screen in the current stream
// Exit: AF corrupt
#define TXT_VDU_ENABLE 0xBB54
void firm_txt_vdu_enable(void) __sdcccall(1) {
__asm
  call TXT_VDU_ENABLE
__endasm;
}

#define TXT_VDU_DISABLE 0xBB57
void firm_txt_vdu_disable(void) __sdcccall(1) {
__asm
  call TXT_VDU_DISABLE
__endasm;
}

// Outputs a char to the screen (where?????)
// A - Char to output
#define TXT_OUTPUT 0xBB5A
void firm_put_char(U8 character) __sdcccall(1) {
  (void) character;
  
__asm
  call TXT_OUTPUT
__endasm;
}

// Outputs a char to the current cursor location in screen
// A - Char to output
// AF|BC|DE|HL corrupt
#define TXT_WR_CHAR 0xBB5D
void firm_put_char_at_cursor(U8 character) __sdcccall(1) {
  (void) character;

__asm
  call TXT_WR_CHAR
__endasm;
}

// Outputs a char to the specfied location in screen
// A - Char to output H-Column L-Line
// AF|BC|DE|HL corrupt
#define TXT_WRITE_CHAR 0xBDD3
void firm_put_char_at(U8 character, U8 line, U8 column) __sdcccall(1) {
  (void) character;
  (void) line;
  (void) column;

__asm
  push  ix
  ld    ix, #0
  add   ix, sp

  ld    h,  4 (ix)
  call  TXT_WRITE_CHAR

  pop ix
__endasm;
}

// Set cursor location
// H - column L - line
// Exits: AF|HL corrupt
#define TXT_SET_CURSOR 0xBB75
void firm_set_cursor_at(U8 column, U8 line) __sdcccall(1) {
  (void) column;
  (void) line;

__asm
  ld    h,  a
  call  TXT_SET_CURSOR
__endasm;
}

#define TXT_SET_COLUMN 0xBB6F
void firm_set_cursor_at_x(U8 column) __sdcccall(1) {
  (void) column;

__asm
  call  TXT_SET_COLUMN
__endasm;
}

#define TXT_SET_ROW 0xBB72
void firm_set_cursor_at_y(U8 row) __sdcccall(1) {
  (void) row;

__asm
  call  TXT_SET_ROW
__endasm;
}

#define TXT_INVERSE 0xBB9C
void firm_set_inverse(void) __sdcccall(1) {
__asm
  call  TXT_INVERSE
__endasm;
}

// Action	Sets the character write mode to either opaque or transparent
// Entry	For transparent mode, A must be non-zero; for opaque mode, A has to hold zero
// Exit	AF and HL are corrupt, and all other registers are preserved
// Notes	Setting the character write mode has no effects on the graphics VDU
#define TXT_SET_BACK 0xBB9F
void firm_set_blend(bool bBlend) __sdcccall(1) {
  (void) bBlend;

__asm
  call  TXT_SET_BACK
__endasm;
}
