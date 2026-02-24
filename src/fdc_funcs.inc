// FDC u765 commands
#define FDC_CMD_SEEK                    0b00001111
#define FDC_CMD_RECALIBRATE             0b00000111
#define FDC_CMD_SENSE_DRIVE_STATUS      0b00000100
#define FDC_CMD_SENSE_INTERRUPT_STATUS  0b00001000
#define FDC_CMD_READ_SECTOR             0b01000110


/*
  FDC u765 Register flags
*/
// FDC u7665 master port flags
#define FDC_REG_MASTER_READY_TO_SEND_RECEIVE           0b10000000
#define FDC_REG_MASTER_READY_TO_SEND                   0b01000000
#define FDC_REG_MASTER_EXECUTION_STAGE                 0b00100000
#define FDC_REG_MASTER_BUSY_ON_ANY_COMMAND             0b00010000
#define FDC_REG_MASTER_BUSY_ON_SEEK_RECALIBRATE_DRIVE3 0b00001000
#define FDC_REG_MASTER_BUSY_ON_SEEK_RECALIBRATE_DRIVE2 0b00000100
#define FDC_REG_MASTER_BUSY_ON_SEEK_RECALIBRATE_DRIVE1 0b00000010
#define FDC_REG_MASTER_BUSY_ON_SEEK_RECALIBRATE_DRIVE0 0b00000001

// FDC u765 Status Register 0 - ST0
#define FDC_REG_ST0_INTERRUPTION_CODE 0b11000000
#define FDC_REG_ST0_END_SEEK          0b00100000
#define FDC_REG_ST0_DRIVE_ERROR       0b00010000
#define FDC_REG_ST0_NOT_READY_ON_RW   0b00001000
#define FDC_REG_ST0_HEAD              0b00000100
#define FDC_REG_ST0_DRIVE_NUMBER      0b00000011

// FDC u765 Status Register 0 - ST0 - Results
#define FDC_REG_ST0_RESULT_INTERRUPTION_CODE_NOT_READY           0b11000000
#define FDC_REG_ST0_RESULT_INTERRUPTION_CODE_INTERRUPTED_COMMAND 0b01000000
#define FDC_REG_ST0_RESULT_INTERRUPTION_CODE_INVALID_COMMAND     0b10000000
#define FDC_REG_ST0_RESULT_INTERRUPTION_CODE_OK                  0b00000000

#define FDC_REG_ST0_RESULT_HEAD_0  0b00000000
#define FDC_REG_ST0_RESULT_HEAD_1  0b00000100

#define FDC_REG_ST0_RESULT_DRIVE_NUMBER_3  0b00000011
#define FDC_REG_ST0_RESULT_DRIVE_NUMBER_2  0b00000010
#define FDC_REG_ST0_RESULT_DRIVE_NUMBER_1  0b00000001
#define FDC_REG_ST0_RESULT_DRIVE_NUMBER_0  0b00000000

// FDC u765 Status Register 1 - ST1
#define FDC_REG_ST1_END_OF_TRACK      0b10000000
#define FDC_REG_ST1_NOT_USED6         0b01000000
#define FDC_REG_ST1_CRC_ERROR         0b00100000
#define FDC_REG_ST1_DATA_OVERRUN      0b00010000
#define FDC_REG_ST1_NOT_USED3         0b00001000
#define FDC_REG_ST1_SECTOR_NOT_FOUND  0b00000100
#define FDC_REG_ST1_WRITE_PROTECTED   0b00000010
#define FDC_REG_ST1_NO_ADDRESS_MARK   0b00000001

// FDC u765 Status Register 2 - ST2
#define FDC_REG_ST2_NOT_USED7               0b10000000
#define FDC_REG_ST2_DELETED_ADDRESS_MARK    0b01000000
#define FDC_REG_ST2_CRC_ERROR_IN_DATA_FIELD 0b00100000
#define FDC_REG_ST2_WRONG_TRACK             0b00010000
#define FDC_REG_ST2_SCAN_EQUAL              0b00001000
#define FDC_REG_ST2_SCAN_SECTOR_NOT_FOUND   0b00000100
#define FDC_REG_ST2_BAD_TRACK               0b00000010
#define FDC_REG_ST2_NO_ADDRESS_MARK         0b00000001

// FDC u7665 Status Register 3 - FDC_CMD_SENSE_DRIVE_STATUS result flags
#define FDC_REG_ST3_FAULT            0b10000000
#define FDC_REG_ST3_WRITE_PROTECTED  0b01000000
#define FDC_REG_ST3_READY            0b00100000
#define FDC_REG_ST3_TRACK0           0b00010000
#define FDC_REG_ST3_DOUBLE_SIDE      0b00001000
#define FDC_REG_ST3_HEAD             0b00000100
#define FDC_REG_ST3_UNIT_SELECT      0b00000011

#define FDC_REG_ST3_RESULT_UNIT_SELECT3     0b00000011
#define FDC_REG_ST3_RESULT_UNIT_SELECT2     0b00000010
#define FDC_REG_ST3_RESULT_UNIT_SELECT1     0b00000001
#define FDC_REG_ST3_RESULT_UNIT_SELECT0     0b00000000

__sfr __banked __at(0xFA7E) fdc_port_motor;
__sfr __banked __at(0xFB7E) fdc_port_master_reg;
__sfr __banked __at(0xFB7F) fdc_port_data;

extern U8 g_fdc_u8MotorOn;
extern U8 g_fdc_u8TrackSelection;
extern U8 g_fdc_u8SectorSelection;

extern U8 g_fdc_u8HeadDriveSelection;

extern U8 ST0;
extern U8 ST1;
extern U8 ST2;
extern U8 ST3;

//
//  Function used as a simple initialized variables
//  container.
//
void fdc_variables(void) __naked {
__asm
  _g_fdc_u8MotorOn:             .db #0    ; Motor status    (0-OFF 1-ON)

  _g_fdc_u8TrackSelection:      .db #0    ; Selected track  (0-79)
  _g_fdc_u8SectorSelection:     .db #0xC1 ; Selected track  (0-79)

  _g_fdc_u8HeadDriveSelection:  .db #0    ; 0b00000 Head Drive | b2 - Head number | b0-1 - Drive number |
                                          ;   XXXXX   X    XX
  
  ; FDC status registers
  _ST0: .db #0
  _ST1: .db #0
  _ST2: .db #0
  _ST3: .db #0
__endasm;
}

//
// Polls FDC's master register until it is ready to send
// or receive data.
//
void fdc_wait_ready_to_send_receive_data(void) {
  while(!(FDC_REG_MASTER_READY_TO_SEND_RECEIVE & fdc_port_master_reg));
}

//
// Polls FDC's master register until a command execution
// stage ends.
//
void fdc_wait_end_of_command_execution(void) {
  while(FDC_REG_MASTER_EXECUTION_STAGE & fdc_port_master_reg);
}

//
// Sends data to the FDC controller
//
// NOTE - This function simply waits for the FDC controller
// until it is ready to send OR receive data. It's up to the 
// programmer to use this function at correct places.
// It's redundant to check for the specific FDC_REG_MASTER_READY_TO_SEND
// flag since the programmer must know what she/he is doing.
//
void fdc_sendData(U8 uData) {
  fdc_wait_ready_to_send_receive_data();
  fdc_port_data = uData;
}

//
// Read data from the FDC controller
//
// NOTE - This function simply waits for the FDC controller
// until it is ready to send OR receive data. It's up to the 
// programmer to use this function at correct places.
// It's redundant to check for the specific FDC_REG_MASTER_READY_TO_SEND
// flag since the programmer must know what she/he is doing.
//
U8 fdc_getData(void) {
  fdc_wait_ready_to_send_receive_data();
  return fdc_port_data;
}

U8 fdc_cmd_sense_interrupt_status(void) {
  fdc_sendData(FDC_CMD_SENSE_INTERRUPT_STATUS);
  ST0 = fdc_getData();
  fdc_getData(); //Track position
  return ST0;
}

//
// Executes the sense interrupt status command
// to get the ST0 register status.
// It's mainly intended to be used during the
// execution stage of specific commands like seek
// OR recalibrate.
// Here it is used to wait until the end of the 
// seek command.
//
// TODO - refactor this function to be targetted
// for different commands. i.e.: Simply store the
// result in ST0 and leave it up to the programmer
// what to do with the result.
//
void fdc_wait_seek_command_to_finish(void) /* __naked */ {
  while(!(FDC_REG_ST0_END_SEEK & fdc_cmd_sense_interrupt_status()));
}


//
// Retrieve the ST3 status register.
//
U8 fdc_get_drive_status(void) {
  fdc_sendData(FDC_CMD_SENSE_DRIVE_STATUS);
    // fdc_sendData(g_fdc_u8DriveSelection); // NOTE - It looks like sending head drive selection byte works fine as well but keep an eye in case something does not work fine at the end. In theory and according to the FDC documentation here we should only send the drive number bits (b1-0) BUT since we are not supporting (still) double headed drives the value is the same.
    fdc_sendData(g_fdc_u8HeadDriveSelection);
  fdc_wait_ready_to_send_receive_data();

  ST3 = fdc_getData();
  return ST3;
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
  fdc_port_motor = g_fdc_u8MotorOn = MOTOR_ON;
  fdc_MotorWait();
}


//
// Turn FDC motor off.
//
void fdc_TurnMotorOff(void) {
  fdc_port_motor = g_fdc_u8MotorOn = MOTOR_OFF;
}


static void fdc_toggleMotor(void) {
  g_fdc_u8MotorOn ? fdc_TurnMotorOff() : fdc_TurnMotorOn();
}

//
// Recalibrate current drive.
// Moves the head to track 0.
//
// Return true if head was moved to Track0.
// NOTE - The command finishes when the head was
// moved to Track0 OR 77 impulses were sent to
// to the head. For drives with 80 tracks it is
// possible that the head wasn't fully moved to
// the track 0 so it's up to the programmer to
// retry at least the command once if it fails
// at first try.
//
bool fdc_Recalibrate(void) /* __naked */ {
  fdc_sendData(FDC_CMD_RECALIBRATE);
    fdc_sendData(g_fdc_u8HeadDriveSelection);
  fdc_MotorWait();
  
  return FDC_REG_ST3_TRACK0 & fdc_get_drive_status();
}


//
// Moves the head to the specified track
//
void fdc_GoToTrack(void) {
  fdc_sendData(FDC_CMD_SEEK);
    fdc_sendData(g_fdc_u8HeadDriveSelection);
    fdc_sendData(g_fdc_u8TrackSelection);
  fdc_wait_seek_command_to_finish();

  // TODO - perform checks to see if the seek command was successful and if not perform a calibrate command and retry
}


//
// Ask if the specified drive is available
// and stores the result in uResult.
// Return true if the drive is ready,
// that means, the drive is on and with an
// inserted disc on it ready to operate.
//
bool fdc_DriveReady(void) {
  return FDC_REG_ST3_READY & fdc_get_drive_status();
}


void fdc_toggleDrives(void) {
  U8 u8MotorOn = g_fdc_u8MotorOn;
  fdc_TurnMotorOn();

  do {
    g_fdc_u8HeadDriveSelection = !g_fdc_u8HeadDriveSelection;  // Drive+Head | b0-1 - Drive number | b2 - Head number
  } while(!fdc_DriveReady());

  if (!u8MotorOn) {
    fdc_TurnMotorOff();
  }
}

//
// Sends the read sector command to the FDC
//
void fdc_cmd_read_sector_send(void) {
  // START - Command
  fdc_sendData(FDC_CMD_READ_SECTOR);
  // Command parameters
    fdc_sendData(g_fdc_u8HeadDriveSelection);
    fdc_sendData(g_fdc_u8TrackSelection);
    fdc_sendData(0);  // HEAD selection is hardcoded to 0. Not supported in the classic CPCs anyways. Sorry for the other (very uncommon) CPC 3" double headed drives.
    fdc_sendData(g_fdc_u8SectorSelection);
    fdc_sendData(0x02);
    fdc_sendData(g_fdc_u8SectorSelection);
    fdc_sendData(0x52);
    fdc_sendData(0xFF);
}

// void fdc_cmd_read_sector_get_data_skip(U8 skipBytes, U8* p_u8Addr) {
//   while(FDC_REG_MASTER_EXECUTION_STAGE & fdc_port_master_reg) {
//     // *p_u8Addr++ = fdc_getData();
//     // fdc_wait_ready_to_send_receive_data();

//     while(!(FDC_REG_MASTER_READY_TO_SEND_RECEIVE & fdc_port_master_reg));
//     if (skipBytes) {
//       skipBytes--;
//       *p_u8Addr = fdc_port_data;
//     } else {
//       *p_u8Addr = fdc_port_data;
//       p_u8Addr++;
//     }
//   }
// }

// void fdc_cmd_read_sector_get_data(U8* p_u8Addr) {
//   do {
//     // NOTE - The below commented out line should be fine as well but introduces a long execution time and the byte coming from the FDC controller is sometimes ovewritten by a new one before we can retrieve it.
//     // We have thus to inline the functionality.
//     //fdc_wait_ready_to_send_receive_data();

//     while(!(FDC_REG_MASTER_READY_TO_SEND_RECEIVE & fdc_port_master_reg));
//     *p_u8Addr++ = fdc_port_data;
//   } while(FDC_REG_MASTER_EXECUTION_STAGE & fdc_port_master_reg);
// }


// // hl - p_u8Addr
// // Achieved changing the border with different colours for EACH byte read.
// void fdc_cmd_read_sector_get_data_fast(U8* p_u8Addr) __sdcccall(1) {

//   (void) p_u8Addr;
// __asm

// ld bc, #_ga_border_and_fdc_port ; Port I/O trickery. Load BC with Gate Array
//                                 ; border pen selection and FDC master port
//                                 ; registers (plus some bitmask to avoid port
//                                 ; conflicts) so we can READ data from the FDC
//                                 ; and WRITE data to the Gate Array without the
//                                 ; need of changing BC each time. Without this,
//                                 ; border color change for each of the read bytes
//                                 ; would be impossible due we have just some CPU
//                                 ; available cycles before the next byte is made
//                                 ; available in the FDC data port.
// out (c), c                      ; Select Gate Array border pen so when
//                                 ; out (c), a is called we change the colour of
//                                 ; the border.
// LOOP_READ_BYTES:
//   LOOP_WAIT_FOR_DATA:       ; // Wait for the FDC READY TO SEND *OR* RECEIVE DATA.
//     in a, (c)               ; Read the master register of the fdc
//   jp p, #LOOP_WAIT_FOR_DATA ; and repeat the operation until the FDC says it is ready to send/receive data.
//                             ; jp p (jump if positive sign) is here used instead of AND #FDC_REG_MASTER_READY_TO_SEND_RECEIVE + JR Z
//                             ; because FDC_REG_MASTER_READY_TO_SEND_RECEIVE byte mask is 0b10000000 and that matches the sign bit.

//   // Exit if read sector command is finished
//   and #FDC_REG_MASTER_EXECUTION_STAGE ; Check if the fdc is still reading bytes
//   ret z                               ; and exit if we are done.

//   // Read the byte from the FDC port and leave it in A register
//   inc c     ; Optimization trick to avoid <ld bc, #_fdc_port_data> since fdc data port is just _fdc_port_master_reg + 1
//   in a, (c) ; Read the byte from the fdc data port
//   dec c     ; Leave the port again pointing to _fdc_port_master_reg for the next loop iteration
  
//   // Store the byte in our target memory address.
//   ld (hl), a                    ; ...store it in our target memory address
//   inc hl                        ; and point to the next memory address for the next byte read.

//   // Change the colour of the border based on the read data... BECAUSE WE CAN!!! :)
//   or #GA_REG_SET_COLOR        ; Activate the set ink/color command mixed with the read byte...
//   and #GA_REG_SET_COLOR_MASK  ; ...and ensure we send the right color info to the Gate Array
//   out (c), a                  ; and not something else.

// jp LOOP_READ_BYTES
  
// LOOP_END_READ:
// __endasm;
// }




// U16 fdc_cmd_read_sector_get_data_count(U8* p_u8Addr, U16 uByteCount) __sdcccall(1) {

//   (void) p_u8Addr;
//   (void) uByteCount;
// __asm

// ld bc, #_fdc_port_master_reg

// LOOP_READ_BYTES_COUNT:
//   LOOP_WAIT_FOR_DATA_COUNT:       ; // Wait for the FDC READY TO SEND *OR* RECEIVE DATA 
//     in a, (c)                     ; Read the master register of the fdc
//   jp p, #LOOP_WAIT_FOR_DATA_COUNT ; and repeat the operation until the FDC says it is ready to send/receive data.
//                                   ; jp p (jump if positive sign) is here used instead of AND #FDC_REG_MASTER_READY_TO_SEND_RECEIVE + JR Z
//                                   ; because FDC_REG_MASTER_READY_TO_SEND_RECEIVE byte mask is 0b10000000 and that matches the sign bit.

//   // exit if read sector command is finished
//   and #FDC_REG_MASTER_EXECUTION_STAGE ; Check if the fdc is still reading bytes
//   ret z                               ; and exit if we are done.

//   // Read the byte from the FDC port and leave it in A register
//   inc c     ; Optimization trick to avoid <ld bc, #_fdc_port_data> since fdc data port is just _fdc_port_master_reg + 1
//   in a, (c) ; Read the byte from the fdc data port
//   dec c     ; Leave the port again pointing to _fdc_port_master_reg for the next loop iteration

//   // Skip the remaining sector bytes if we read all the required bytes (uByteCount stored in DE register)
//   ex af, af'                    ;' Backup A register because we need it
//   ld a, d                       ; If uByteCount (stored in DE)
//   or e                          ; reached 0, it means we do not need to read more bytes,
//   jr z, #LOOP_READ_BYTES_COUNT  ; hence we keep reading bytes until the command finishes.
  
//   // else decrement bytes to read and...
//   dec de                        ; Otherwise, decrement the uByteCount (stored in DE)
//   ex af, af'                    ;' recover the read byte from the FDC...

//   // ...store the byte in our target memory address.
//   ld (hl), a                    ; ...store it in our target memory address
//   inc hl                        ; and point to the next memory address for the next byte read.

// jp LOOP_READ_BYTES_COUNT

// __endasm;
// }

// // TODO make version that stores the last byte in last target memory address + 1 to reduce bytesize


void fdc_cmd_read_sector_get_results(void) {
  fdc_wait_end_of_command_execution();
  ST0 = fdc_getData();
  ST1 = fdc_getData();
  ST2 = fdc_getData();

  // Ignore the rest of the unuseful data for our purposes.
  fdc_getData();
  fdc_getData();
  fdc_getData();
  fdc_getData();
}

//
// Moves the head to the specified track and try
// to find a sector with the specified Sector ID.
//
bool fdc_FindSector(void) {
  fdc_cmd_read_sector_send();

  // At this point we have data ready to be read coming from the 
  // execution stage of the command. This can be sector data
  // OR an error indicating the sector was not found.

  // Since we are not interested in the content of the sector we
  // wait until the command finishes. If we would like to retrieve
  // the data stored in the sector, we would here implement the logic
  // to copy the data to memory positions. i.e.:
  //
  //    - while(fdc_wait_ready_to_send_receive_data())
  //        wait for data to be read
  //        read the data from the data port
  //        store data in memory position
  //
  // We simply ignore this and wait for the end of the command
  // to extract the result we are interested in.
  fdc_wait_ready_to_send_receive_data();
  fdc_cmd_read_sector_get_results();

  // Set to true/false if the sector was found.
  return ST1 & (FDC_REG_ST1_SECTOR_NOT_FOUND | FDC_REG_ST1_NO_ADDRESS_MARK);
}

// bool fdc_read_sector_skip(U8 skipBytes, U8* p_u8Addr) {
//   __asm di __endasm;
//   do {
//     fdc_cmd_read_sector_send();
//     fdc_cmd_read_sector_get_data_skip(skipBytes, p_u8Addr);
//     fdc_cmd_read_sector_get_results();
//   } while (ST1 & (FDC_REG_ST1_CRC_ERROR | FDC_REG_ST1_DATA_OVERRUN | FDC_REG_ST1_SECTOR_NOT_FOUND | FDC_REG_ST1_NO_ADDRESS_MARK));

//   __asm ei __endasm;
//   return true;
// }


// bool fdc_read_sector(U8* p_u8Addr) {
//   __asm di __endasm;
//   do {
//     fdc_cmd_read_sector_send();
//     fdc_cmd_read_sector_get_data(p_u8Addr);
//     fdc_cmd_read_sector_get_results();
//   } while (ST1 & (FDC_REG_ST1_CRC_ERROR | FDC_REG_ST1_DATA_OVERRUN | FDC_REG_ST1_SECTOR_NOT_FOUND | FDC_REG_ST1_NO_ADDRESS_MARK));

//   __asm ei __endasm;
//   return true;
// }

// //
// // Read bytes starting from the current selected track/sector onwards.
// // 
// // Params:
// //    p_u8Addr    - Target address where to start storing the bytes.
// //    uByteCount  - Amount of bytes to read.
// //
// // Entry conditions
// // // bytes starting from g_fdc_u8TrackSelection and g_fdc_u8SectorSelection
// bool fdc_read_bytes(U8* p_u8Addr, U16 uByteCount) {
// __asm di __endasm;

//   do {
//     fdc_cmd_read_sector_send();
//     if (uByteCount > 512) {
//       fdc_cmd_read_sector_get_data_fast(p_u8Addr);
//       uByteCount -= 512;
//     } else {
//       uByteCount = fdc_cmd_read_sector_get_data_count(p_u8Addr, uByteCount);
//     }
//     fdc_cmd_read_sector_get_results();
//     p_u8Addr += 512;
    
//     // Exit if error (caller is responsible to retry the read)
//     if (ST1 & (FDC_REG_ST1_CRC_ERROR | FDC_REG_ST1_DATA_OVERRUN | FDC_REG_ST1_SECTOR_NOT_FOUND | FDC_REG_ST1_NO_ADDRESS_MARK)) {
//       return false;
//     } else if (++g_fdc_u8SectorSelection > 201) { // If no errors then adjust next sector/track to read.
//       g_fdc_u8SectorSelection = 193;
//       g_fdc_u8TrackSelection++;
//       fdc_GoToTrack();
//     }
//   } while (uByteCount);

// __asm ei __endasm;
//   return true;
// }
