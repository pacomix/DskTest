// FDC u765 commands
#define FDC_CMD_MOVE_TO_TRACK 0b00001111

// FDC u7665 command result flags
#define FDC_CMD_IS_OVER       0b00100000

extern U8 FDCMOTOR;
extern U8 FDCDRIVE;
extern U8 FDCHEAD;
extern U8 FDCIDDR;

extern U8 ST0;
extern U8 ST1;
extern U8 ST2;

__sfr __banked __at(0xFA7E) fdc_motor_port;
__sfr __banked __at(0xFB7E) fdc_master_register_port;
__sfr __banked __at(0xFB7F) fdc_data_port;

// Can not even use the below C code since it generates
// a in a, (address) instruction that does not
// change status flag register
/* 
#define DO_WHILE_PARITY(...) \
  __asm ex af,af' ; Bakcup a' __endasm; \
  __asm PARITY: __endasm; \
  __VA_ARGS__; \
  __asm JP P, PARITY __endasm; \
  __asm ex af,af' ; get back backed up a' register __endasm;
 */
//
// Wait until the fdc is ready to accept data
// Can not write this as C code since there is no way
// in C code to check for parity after reading the result
// of a port.
//
void fdc_wait_ready_to_data(void) {

  //DO_WHILE_PARITY(fdc_master_register_port);

__asm
  LD   BC, #_fdc_master_register_port

WAIT_DRIVE_READY_TO_DATA:
  IN   A,(C)
  JP   P,WAIT_DRIVE_READY_TO_DATA       ; Wait until drive is ready to accept data

  ret
__endasm;

__asm 
; Variables declared like this do not generate initialization code.
  _FDCMOTOR: .db #0      ; Motor status   (0-OFF 1-ON)
  _FDCDRIVE: .db #0      ; Selected drive (0-3)
  _FDCHEAD:  .db #0      ; Head used (0-1)
  _FDCIDDR:  .db #0      ; Drive+Head | b0-1 - Drive number | b2 - Head number

  _ST0: .db #0           ; Buffer to store data sent back by the FDC
  _ST1: .db #0
  _ST2: .db #0
__endasm;
}

//
// Sends data to the FDC controller
//
// Register A - uData 
//
void fdc_sendData(U8 uData) {
  fdc_wait_ready_to_data();
  fdc_data_port = uData;
}

//
// Read data from the FDC controller
//
// Return Register A - data read from the fdc
//
U8 fdc_getData(void) {
  fdc_wait_ready_to_data();
  return fdc_data_port;
}

//
// Wait until the end of the current command
// being executed by the fdc (using _ST0)
//
void fdc_wait_command_to_finish(void) /* __naked */ {
  // This could be translated to the below code but does not work fine.
  // TODO - refine it. Test it with the gototrack function
  do {
    fdc_sendData(8);
    ST0 = fdc_getData();
    fdc_getData();
    // ST1=0; TODO - For the serious FDC library get always all the results
    // ST2=0;
  } while(!(FDC_CMD_IS_OVER & ST0));
/* __asm
  loop_fdc_wait_command_to_finish:
    LD   A,#0b00001000        ; TODO - Check what is this command. Transform it to defined for better readability.
    CALL _fdc_sendData
    CALL _fdc_getData         ;Get ST0
    LD   (_ST0),A
    CALL _fdc_getData
    ;XOR  A
    ;LD   (_ST1),A        ;Reset ST1 and ST2 TODO - check if this is mandatory
    ;LD   (_ST2),A

    ; Check if instruction is finished (Bit 5 of ST0)
    LD   A,(_ST0)
    BIT  5,A            ;Instruction over ?
    JR   Z,loop_fdc_wait_command_to_finish
  RET
__endasm; */
}

//
// Wait a bit - 180 interrupts - 0.6 seconds
//
void fdc_MotorWait(void) {
__asm
  LD B,#180

  MotorWAIT:
    HALT
    DJNZ MotorWAIT
__endasm;
}


//
// Switch FDC motor on including the waiting time.
// Waiting time is required to ensure the spin be constant.
//
void fdc_TurnMotorOn(void) {
  fdc_motor_port = MOTOR_ON;
  fdc_MotorWait();
/* 
__asm
  LD   BC,#0xFA7E
  LD   A,#1
  OUT  (C),A
  call _fdc_MotorWait
__endasm;
 */
}


//
// Turn FDC off.
//
void fdc_TurnMotorOff(void) {
  fdc_motor_port = MOTOR_OFF;
/*   
__asm
  LD   BC,#0xFA7E
  XOR  A
  OUT  (C),A
__endasm;
 */
}


//
// Moves the head to the specified track
//
void fdc_GoToTrack(U8 uTrack) { // Register A - uTrack
  // (void) uTrack;
  fdc_sendData(FDC_CMD_MOVE_TO_TRACK);  // 0b00001111 CMD - Move head to track
  fdc_sendData(FDCIDDR);                //                  Using this drive and head
  fdc_sendData(uTrack);                 //                  To target track
  fdc_wait_command_to_finish();

// __asm
//   PUSH AF

//   LD   A, #0b00001111
//   CALL _fdc_sendData         ; Send command to move the head? (TODO - Verify this and create proper defines)
//   LD   A, (_FDCIDDR)
//   CALL _fdc_sendData         ; Tell the drive and head to use.

//   POP  AF
//   CALL _fdc_sendData         ; Send the track parameter for the command

//   CALL _fdc_wait_command_to_finish        ; Wait until the command is fully executed
// __endasm;
}


//
// Sets the drive and head to use for the commands.
//
void fdc_SelectDrive(U8 uDrive, U8 uHead) {
  FDCDRIVE = uDrive;
  FDCHEAD = uHead;
  FDCIDDR = (uHead << 2) | uDrive;  // Drive+Head | b0-1 - Drive number | b2 - Head number
}

//
// Recalibrate current drive. Moves the head to track 0.
//
void fdc_Calibrate(void) __naked {
__asm
  call  Calibrate_Sub
  call  Calibrate_Sub
  jp    Calibrate_END

Calibrate_Sub:
  LD    A,#0b00000111
  CALL _fdc_sendData          ; Command to move the head? (TODO - Verify this and create proper defines)

  ld a, (_FDCIDDR)
  CALL _fdc_sendData          ; send the the drive and head parameter to use for the command

  CALL _fdc_wait_command_to_finish

Calibrate_END:
    ret
__endasm;
}


//
// Ask if the specified drive is available and stores the result in uResult.
//
bool fdc_DriveReady(U8 uDrive) { // REG a: uDrive Exit: REG a: uResult
  (void) uDrive;

__asm
  push  af              ; Save uDrive (Reg a)

  ld    a,  #0b00000100
  CALL _fdc_sendData          ; Send Sense drive command

  pop   af              ; Recover uDrive again
  CALL _fdc_sendData          ; and send the target drive parameter for the command.

  LD    BC, #_fdc_master_register_port     ; Master register port
WAITCMDAV:
  IN    A,  (C)         ; Wait until the command is processed by the FDC
  AND   #0b10000000     ; Wait until the command phase is finished. That is when
                        ; the drive is ready to send us data.
  JR    Z,  #WAITCMDAV  ; NOTE: That could mean that the sector id was found and
                        ; it is starting with the transfer or that the sector
                        ; was not found and we can start reading the results.

  CALL _fdc_getData          ; Read the result and put it in the ST1 variable and ret
  LD    (_ST1),  A
  AND   #0b00100000

__endasm;
}


//
// Moves the head to the specified track and try to find a sector with the
// specified Sector ID.
//
// Register A - uSectorID
// Register L - uTrack
// Stack: uFoundSectorID
//
void fdc_FindSector(U8 uSectorID, U8 uTrack, U8* uFoundSectorID) {
  (void) uSectorID;
  (void) uTrack;
  (void) uFoundSectorID;

__asm
  ; Saves the IX reg. since we use it to access the stack.

  push  ix
  ld    ix, #0
  add   ix, sp

  ;
  ; Read sector.
  ;
  ; Reg A - Track
  ; Reg B - Sector ID to search for
  ;
  ; Returns
  ;   Reg A | State | 0-ok 1-disc missing 2-read fail 3-file not found
  ;   Carry | 1-ok
  ;
  ; HL=Where new data should be loaded (LOADWHER)
  ;
  ld    B,  A           ; uSectorID
  ld    A,  l           ; uTrack

  READSECT:
    LD    (RSPIST+1),A    ; Hardcode the track number in the code. See tag RSPIST
    ; TODO - all the info can be put using ld (VAR+XXX),A in the configureFDC, GoToTrack, etc... functions
    PUSH  BC              ; Save the SectorID since PUTFDC modifies BC

  ; READ Sector command: 06
  ; Parameter bytes:
  ;   HU  b0,1=Unit/Drive Number, b2=Physical Head Number, other bits zero
  ;   TR  Track-ID (usually same value as TP)
  ;   HD  Head-ID
  ;   SC  First Sector-ID (sector you want to read)
  ;   SZ  Sector Size (80h shl n) (default=02h for 200h bytes)
  ;   LS  Last Sector-ID (should be same as SC when reading a single sector)
  ;   GP  Gap (default=2Ah except command 0D: default=52h)
  ;   SL  Sectorlen if SZ=0 (default=FFh)

  ; Send the command plus the required parameters to the FDC
    LD    A,  #0b01000110  ; Send read sector command
    CALL _fdc_sendData

  ; Send the required parameters.
    LD    A,  (_FDCIDDR)   ; HU
    CALL _fdc_sendData

  RSPIST: ; Tag to modify the parameter of the LD A,#0 instruction below.
    LD    A,  #0          ; TR
    CALL _fdc_sendData

    XOR   A               ; HD  ; TODO - Give support for double headed drives.
    CALL _fdc_sendData

    POP   BC              ; SC
    LD    A,  B
    PUSH  AF
    CALL _fdc_sendData

    LD    A,  #2          ; SZ
    CALL _fdc_sendData

    POP   AF              ; LS
    CALL _fdc_sendData

    LD    A,  #0x52       ; GP
    CALL _fdc_sendData

    LD    A,  #0xFF       ; SL
    CALL _fdc_sendData

  ; Wait until command is processed by the FDC
  ; TODO consider to put this in the _storage function since it is used in more than one place.
    LD    BC, #_fdc_master_register_port     ; Master register port
  
  WAITCMDEND:
    IN    A,  (C)         ; Wait until the command is processed by the FDC
    AND   #0b10000000     ; Wait until the command phase is finished. That is when the drive is ready to send us data.
    JR    Z,  #WAITCMDEND ; NOTE: That could mean that the sector id was found and it is starting with the transfer or
                          ; that the sector was not found and we can start
                          ; reading the results.
  ; At this point the FDC has processed the command and started executing it.
  ; Now we wait until the execution phase is finished
  
  WAITEXEEND:
    IN    A,  (C)
    AND   #0b00100000     ; At this point if the sector does exist it will read
                          ; it and the bit 5 will be reset once it finishes.
                          ; But if the sector does not exist, it will enter this
                          ; state with the bit 5 already reset.
                          ; TODO - Write here the FDC docu.
    JR    NZ, #WAITEXEEND ; Wait until the command is finished executing.

  ; Retrieve the result bytes through the data register. We ignore most of the
  ; results since we do not need them.
  ; TODO - Add here the FDC info for the other values.
    CALL _fdc_getData
    CALL _fdc_getData
    LD    (_ST1), A        ; This is the result we are interested in.
                          ; Missing Addres Mark flag or SectorID not found.
                          ; Set after 2 index pulses if SectorID is not found.
    CALL _fdc_getData
    CALL _fdc_getData
    CALL _fdc_getData
    CALL _fdc_getData
    CALL _fdc_getData

  ; Store the result in the function parameter.
  ; TODO - Consider changing this to use return registers instead.
    ld    l,  4 (IX)
    ld    h,  5 (IX)

    LD    A,  (_ST1)
    AND   #0b00000101

    ;JR Z, #FindSector_FOUND

  ; Not found red color
  FindSector_NOTFOUND:
    ;ld bc,#0x7f10
    ;out (c),c
    ;ld c,#0x55
    ;out (c),c
    ;JP FindSector_END

  ; Found. Green color
  FindSector_FOUND:
    ;ld bc,#0x7f10
    ;out (c),c
    ;ld c,#0x56
    ;out (c),c

  FindSector_END:
    LD    (HL), A

  ; Recover the IX register at the end
    pop ix
__endasm;
}
