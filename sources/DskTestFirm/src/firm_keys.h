#define CHAR_ENTER_BIG 0x0D
#define CHAR_ENTER_SMALL 0x8B
#define CHAR_COPY 0xE0

#define CHAR_CURSOR_UP 0xF0
#define CHAR_CURSOR_DOWN 0xF1
#define CHAR_CURSOR_LEFT 0xF2
#define CHAR_CURSOR_RIGHT 0xF3


// Waits for a key to be pressed
// Carry TRUE | A character
#define KM_WAIT_KEY 0xBB18
U8 firm_get_key_wait(void) __sdcccall(1) {
__asm
  call KM_WAIT_KEY
__endasm;
}

// TODO - SDCC no excluye este tipo de funciones definidas si no se usan... 
// Conseguir esto compilando los ficheros como bibliotecas, y teoricamente el enlazador debe omitir las funciones no usadas.

// Read key without waiting
// Carry TRUE | A character
#define KM_READ_KEY 0xBB18
U8 firm_read_key(void) __sdcccall(1) {
__asm
  call KM_READ_KEY
  jp c, firm_get_key_wait_end
  xor a
firm_get_key_wait_end:
__endasm;
}
