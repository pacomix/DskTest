#include "fdc.h"

// Action	Sets the parameters which effect the disc speed
// Entry	HL holds the address of the nine bytes which make up the parameter block
// Exit	AF, BC, DE and HL are corrupt, and all other registers are preserved
// Notes	The parameter block is arranged as follows
// bytes 0&1 - the motor on time in 20ms units; the default is &0032; the fastest is &0023
// bytes 2&3 - the motor off time in 20ms units; the default is &00FA; the fastest is &00C8
// byte 4 - the write off time in l0æs units; the default is &AF; should not be changed
// byte 5 - the head settle time in 1ms units; the default is &0F; should not be changed
// byte 6 - the step rate time in 1ms units; the default is &0C; the fastest is &0A
// byte 7 - the head unload delay; the default is &01; should not be changed
// byte 8 - a byte of &03 and this should be left unaltered
#define BIOS_SETUP_DISC 0xC036
void firm_fdc_setup(void) __sdcccall(1) __naked {
__asm
  ld    hl, #__firm_fdc_setup_params
  call  BIOS_SETUP_DISC
  ret
__firm_fdc_setup_params:
  .dw #0x0032
  .dw #0x00FA
  .db #0xAF
  .db #0x0F
  .db #0x0C
  .db #0x01
  .db #0x03
__endasm;
}