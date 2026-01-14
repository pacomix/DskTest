
void _storage(U16* pIndexHoleCounter) __naked {
  (void) pIndexHoleCounter;

__asm
  ;
  ; Wait for the end of the current instruction (using ST0).
  ;
  WAITEND:
    LD   A,#0b00001000
    CALL PUTFDC
    CALL GETFDC         ;Get ST0
    LD   (ST0),A
    CALL GETFDC
    XOR  A
    LD   (ST1),A        ;Reset ST1 and ST2
    LD   (ST2),A

    ; Check if instruction is finished (Bit 5 of ST0)
    LD   A,(ST0)
    BIT  5,A            ;Instruction over ?
    JR   Z,WAITEND
  RET


  ;
  ; Send data to FDC
  ;   A - data to send
  ;
  PUTFDC:
    ex af,af'           ; 'Backup register a
    LD   BC,#0xFB7E

  PUTFD2:
    IN   A,(C)
    JP   P,PUTFD2       ; Wait until drive is ready to accept data

    ex af,af'           ; 'Recover our data to be sent from register a
    inc c
    OUT  (C),A          ; Send the data
    RET


  ;
  ; Get data from FDC
  ;
  ;   Returns
  ;     A - data from FDC
  ;
  GETFDC:
    LD   BC,#0xFB7E

  GETFD2:
    IN   A,(C)
    JP   P,GETFD2       ; Wait until the FDC is ready to send the data

    inc c
    IN   A,(C)          ; Read the data
    RET


  ; Variables
  FDCMOTOR: .db #0      ; Motor status   (0-OFF 1-ON)
  FDCDRIVE: .db #0      ; Selected drive (0-3)
  FDCHEAD:  .db #0      ; Head used (0-1)
  FDCIDDR:  .db #0      ; Drive+Head | b0-1 - Drive number | b2 - Head number

  ST0: .db #0           ; Buffer to store data sent back by the FDC
  ST1: .db #0
  ST2: .db #0
__endasm;
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
__asm
  LD   BC,#0xFA7E
  LD   A,#1
  OUT  (C),A
  call _fdc_MotorWait
__endasm;
}


//
// Turn FDC off.
//
void fdc_TurnMotorOff(void) {
__asm
  LD   BC,#0xFA7E
  XOR  A
  OUT  (C),A
  call _fdc_MotorWait
__endasm;
}


//
// Moves the head to the specified track
//
void fdc_GoToTrack(U8 uTrack) { // REG a: uTrack
  uTrack;
__asm
  PUSH AF

  LD   A, #0b00001111
  CALL PUTFDC         ; Send command to move the head? (TODO - Verify this and create proper defines)
  LD   A, (FDCIDDR)
  CALL PUTFDC         ; Tell the drive and head to use.

  POP  AF
  CALL PUTFDC         ; Send the track parameter for the command

  CALL WAITEND        ; Wait until the command is fully executed
__endasm;
}


//
// Sets the drive and head to use for the commands.
//
// Reg a - drive (0-3)
// Reg l - head (0-1)
//
void fdc_SelectDrive(U8 uDrive, U8 uHead) { // REG a: uDrive REG l: uHead
  (void) uDrive;
  (void) uHead;

__asm
  FDCVARS:
    ld (FDCDRIVE),a
    ld c,a
    ld a,l
    ld (FDCHEAD),a
    rla
    rla
    and #0b00000100
    or c
    ld (FDCIDDR),a
__endasm;
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
  CALL  PUTFDC          ; Command to move the head? (TODO - Verify this and create proper defines)

  ld a, (FDCIDDR)
  CALL  PUTFDC          ; send the the drive and head parameter to use for the command

  CALL  WAITEND

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
  call  PUTFDC          ; Send Sense drive command

  pop   af              ; Recover uDrive again
  call  PUTFDC          ; and send the target drive parameter for the command.

  LD    BC, #0xFB7E     ; Master register port
WAITCMDAV:
  IN    A,  (C)         ; Wait until the command is processed by the FDC
  AND   #0b10000000     ; Wait until the command phase is finished. That is when
                        ; the drive is ready to send us data.
  JR    Z,  #WAITCMDAV  ; NOTE: That could mean that the sector id was found and
                        ; it is starting with the transfer or that the sector
                        ; was not found and we can start reading the results.

  CALL  GETFDC          ; Read the result and put it in the ST1 variable and ret
  LD    (ST1),  A
  AND   #0b00100000

__endasm;
}


//
// Moves the head to the specified track and try to find a sector with the
// specified Sector ID.
//
// REG a: uSectorID
// REG l: uTrack
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
    CALL  PUTFDC

  ; Send the required parameters.
    LD    A,  (FDCIDDR)   ; HU
    CALL  PUTFDC

  RSPIST: ; Tag to modify the parameter of the LD A,#0 instruction below.
    LD    A,  #0          ; TR
    CALL  PUTFDC

    XOR   A               ; HD  ; TODO - Give support for double headed drives.
    CALL  PUTFDC

    POP   BC              ; SC
    LD    A,  B
    PUSH  AF
    CALL  PUTFDC

    LD    A,  #2          ; SZ
    CALL  PUTFDC

    POP   AF              ; LS
    CALL  PUTFDC

    LD    A,  #0x52       ; GP
    CALL  PUTFDC

    LD    A,  #0xFF       ; SL
    CALL  PUTFDC

  ; Wait until command is processed by the FDC
  ; TODO consider to put this in the _storage function since it is used in more than one place.
    LD    BC, #0xFB7E     ; Master register port
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
    CALL  GETFDC
    CALL  GETFDC
    LD    (ST1), A        ; This is the result we are interested in.
                          ; Missing Addres Mark flag or SectorID not found.
                          ; Set after 2 index pulses if SectorID is not found.
    CALL  GETFDC
    CALL  GETFDC
    CALL  GETFDC
    CALL  GETFDC
    CALL  GETFDC

  ; Store the result in the function parameter.
  ; TODO - Consider changing this to use return registers instead.
    ld    l,  4 (IX)
    ld    h,  5 (IX)

    LD    A,  (ST1)
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
