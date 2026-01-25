#include "types.h"

// Action	Searches an RSX, background ROM or foreground ROM, to find a command
// in its table.
// Entry	HL contains the address of the command name (in RAM only) which is
// being searched for
// Exit	If the name was found in a RSX or background ROM then Carry is true,
// C contains the ROM select address, and HL contains the address of the routine;
// if the command was not found, then Carry is false, C and HL are corrupt;
// in either case, A, B and DE are corrupt, and all others are preserved
// Notes	The command names should be in upper case and the last character should have &80 added to it; the sequence of searching is RSXs, then ROMs with lower numbers before ROMs with higher numbers
#define KL_FIND_COMMAND 0xBCD4
bool firm_kl_find_cmd(U8* uCmd, void* pStorage) __sdcccall(1) {
  (void)uCmd;
  (void)pStorage;
__asm
  push  de
  call  KL_FIND_COMMAND
  pop   de
  jr    nc, _firm_kl_find_cmd_exit_error

  ex    de, hl
  ld    (hl), e
  inc   hl
  ld    (hl), d
  inc   hl
  ld    (hl), c

_firm_kl_find_cmd_exit_ok:
  ld a, #true               ; set return code to true
  ret

_firm_kl_find_cmd_exit_error:
  xor   a
__endasm;
}
