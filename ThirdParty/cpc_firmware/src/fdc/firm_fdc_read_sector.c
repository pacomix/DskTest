#include "fdc.h"

// Reads a sector from a disc into memory
// HL - Target address
// E - Drive NUMBER
// D - Track number
// C - Sector ID
// Exit: Flags corrupted
//  OK    - Carry TRUE A=0
//  FAIL  - Carry FALSE A=ERROR NUMBER HL corrupt
#define BIOS_READ_SECTOR 0xC03C
U8 firm_fdc_read_sector(U8* target_buffer, U8 track, U8 drive, U8 sector) __sdcccall(0) {
  (void) target_buffer;
  (void) track;
  (void) drive;
  (void) sector;
__asm
  push  ix
  ld    ix, #0
  add   ix, sp

  ld    l,  4 (ix)
  ld    h,  5 (ix)
  ld    e,  6 (ix)
  ld    d,  7 (ix)
  ld    c,  8 (ix)

  call  BIOS_READ_SECTOR
  ld    l,  a   ; return value for sdcccall 0

  pop ix
__endasm;
}
