#include "fdc.h"

// Moves the disc drive head to the specified track
// E - Drive number D - Track
// Exit: Flags corrupt
//   OK   - Carry TRUE A=0 HL preserved
//   FAIL - Carry FALSE A=ERROR NUMBER  HL corrupt
#define BIOS_MOVE_TRACK 0xC045
bool firm_fdc_goto_track(U8 drive_number, U8 track) __sdcccall(1) {
  (void) drive_number;
  (void) track;
__asm
  ld    e,  a
  ld    d,  l

  ld    hl, #_FIRM_FDC_SELECT_FORMAT
  ld    (__FAR_CALL+1), hl
  call  __FAR_CALL

  jr    nc, _firm_fdc_goto_track_error
  ld    a, #1
  ret

_firm_fdc_goto_track_error:
  xor   a
__endasm;
}
