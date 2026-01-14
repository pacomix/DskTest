typedef struct {
  U16 address;
  U8  rom;
} FARCALL;

/*
_FIRM_FDC_SET_MESSAGE::     .dw 0xC033
                            .db 0x07
_FIRM_FDC_SET_UP_DISC::     .dw 0xC036
                            .db 0x07
_FIRM_FDC_SELECT_FORMAT::   .dw 0xC039
                            .db 0x07
_FIRM_FDC_READ_SECTOR::     .dw 0xC03C
                            .db 0x07
_FIRM_FDC_WRITE_SECTOR::    .dw 0xC03F
                            .db 0x07
_FIRM_FDC_FORMAT_TRACK::    .dw 0xC042
                            .db 0x07
_FIRM_FDC_MOVE_TRACK::      .dw 0xC045
                            .db 0x07
_FIRM_FDC_GET_DR_STATUS::   .dw 0xC048
                            .db 0x07
_FIRM_FDC_SET_RETRY_COUNT:: .dw 0xC04B
                            .db 0x07
                            */
extern FARCALL FIRM_FDC_SET_MESSAGE;
extern FARCALL FIRM_FDC_SET_UP_DISC;
extern FARCALL FIRM_FDC_SELECT_FORMAT;
extern FARCALL FIRM_FDC_READ_SECTOR;
extern FARCALL FIRM_FDC_WRITE_SECTOR;
extern FARCALL FIRM_FDC_FORMAT_TRACK;
extern FARCALL FIRM_FDC_MOVE_TRACK;
extern FARCALL FIRM_FDC_GET_DR_STATUS;
extern FARCALL FIRM_FDC_SET_RETRY_COUNT;

bool firm_fdc_init_routines() __sdcccall(1) {
  U8 uRSXCMD = 0x81;
  FARCALL* pDst = &FIRM_FDC_SET_MESSAGE;

  do {
    if (!firm_kl_find_cmd(&uRSXCMD, pDst)) {
      return false;
    }
  } while(++pDst, ++uRSXCMD < 0x8A);
  return true;
}

// Moves the disc drive head to the specified track
// E - Drive number D - Track
// Exit: Flags corrupt
//   OK   - Carry TRUE A=0 HL preserved
//   FAIL - Carry FALSE A=ERROR NUMBER  HL corrupt
#define BIOS_MOVE_TRACK 0xC045
bool firm_fdc_goto_track(U8 drive_number, U8 track) __sdcccall(1) {
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




// Action	Enables or disables disc error messages
// Entry	To enable messages, A holds &00; to disable messages, A holds &FF
// Exit	A holds the previous state, HL and the flags are corrupt, and all others are preserved
// Notes	Enabling and disabling the messages can also be achieved by poking &BE78 with &00 or &FF
#define BIOS_SET_MESSAGE 0xC033
void firm_fdc_messages(U8 uVal) __sdcccall(1) {
__asm
  ld    hl, #_FIRM_FDC_SET_MESSAGE
  ld    (__FAR_CALL+1), hl
  call  __FAR_CALL
__endasm;
}

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
void firm_fdc_setup() __sdcccall(1) __naked {
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

void my_far_cry() __sdcccall(1) __naked {
__asm
__FAR_CALL: rst 24
            .dw 0x0000
            ret
__endasm;
}

// Action	Sets a format for a disc
// Entry	A holds the type of format that is to be selected
// Exit	AF, BC, DE and HL are corrupt, and all the other registers are preserved
// Notes	To select one of the normal disc formats, the following values should be put into the A register
// Data format - &C1
// System format - &41 - Used by CP/M
// IBM format - &01 - compatible with CP/M-86
// This routine sets the extended disc parameter block (XDPB) at &A890 to &A8A8
// - to set other formats, the XDPB must be altered directly
#define BIOS_SELECT_FORMAT 0xC039
void firm_fdc_select_format(U8 uFormat) __sdcccall(1) {
  __asm
    ld    hl, #_FIRM_FDC_SELECT_FORMAT
    ld    (__FAR_CALL+1), hl
    call  __FAR_CALL
    xor   a               ; set return code to true
__endasm;
}

// Return status of target drive
// A - Drive number
// Exit:
//   OK   - Carry TRUE A=Status b6-write protect or disc missing b5-ready with
// disc b4-head in track0
//   FAIL - Carry FALSE A=corrupt HL-Address of status byte-1
#define BIOS_GET_STATUS 0xC048
bool firm_fdc_get_status(U8 drive) __sdcccall(1) {
__asm
  ld    hl, #_FIRM_FDC_GET_DR_STATUS
  ld    (__FAR_CALL+1), hl
  call  __FAR_CALL
  jr    c, __firm_fdc_get_status_get_result
  inc   hl
  ld    a, (hl)

__firm_fdc_get_status_get_result:
__endasm;
}
