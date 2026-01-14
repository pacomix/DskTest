// Set routine to be called for every interrupt (1/300)
// HL - Jump address
// AF/DE/HL corrupted afterwards
#define KL_NEW_FAST_TICKER 0xBCE0
#define KL_DEL_FAST_TICKER 0xBCE6
void firm_set_int_callback(U8* pFn) __sdcccall(1) {
__asm
  call KL_DEL_FAST_TICKER
__endasm;
}



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


// Gets the amount of interruptions happened since the machine is on or since the
// last time KL_TIME_SET was called.
// Returns DEHL
#define KL_TIME_PLEASE 0xBD0D
U16 firm_get_interruption_count() __sdcccall(1) {
__asm
  call KL_TIME_PLEASE
  ex    de, hl  ; __sdcccall(1) requires 16 bit values be put in DE, and the result
                ; is put in DEHL, hence we put the least significant bytes in DE
                ; and we discard the rest. Yes we are limited to 65535 ints.
                ; Thats a total of approx. 3.5 minutes counting.
__endasm;
}

// Set the interrupt count.
// DEHL - interrupt count to set
// AF corrupt
#define KL_TIME_SET 0xBD10
void firm_set_interruption_count(U16 lsb, U16 msb) __sdcccall(1) {
__asm
  call KL_TIME_SET
__endasm;
}
