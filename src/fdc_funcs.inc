// FDC u765 commands
#define FDC_CMD_SEEK                    0b00001111
#define FDC_CMD_RECALIBRATE             0b00000111
#define FDC_CMD_SENSE_DRIVE_STATUS      0b00000100
#define FDC_CMD_SENSE_INTERRUPT_STATUS  0b00001000
#define FDC_CMD_READ_SECTOR             0b01000110


/* FDC u765 Register flags */

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

// FDC u7665 Status Register 3 - FDC_CMD_DRIVE_STATUS result flags
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

__sfr __banked __at(0xFA7E) fdc_motor_port;
__sfr __banked __at(0xFB7E) fdc_master_register_port;
__sfr __banked __at(0xFB7F) fdc_data_port;

extern U8 g_u8MotorOn;
extern U8 g_u8DriveSelection;
extern U8 g_u8TrackSelection;
extern U8 FDCHEAD;
extern U8 FDCIDDR;

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
  _g_u8MotorOn: .db #0     ; Motor status   (0-OFF 1-ON)
  _g_u8DriveSelection: .db #0     ; Selected drive (0-3)
  _g_u8TrackSelection: .db #0     ; Selected track
  _FDCHEAD:  .db #0     ; Head used (0-1)
  _FDCIDDR:  .db #0     ; 0b00000 Head Drive | b2 - Head number | b0-1 - Drive number |
                        ;           X    XX

  _ST0: .db #0          ; FDC status registers
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
  while(!(FDC_REG_MASTER_READY_TO_SEND_RECEIVE & fdc_master_register_port));
}

//
// Polls FDC's master register until a command execution
// stage ends.
//
void fdc_wait_end_of_command_execution(void) {
  while(FDC_REG_MASTER_EXECUTION_STAGE & fdc_master_register_port);
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
  fdc_data_port = uData;
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
  return fdc_data_port;
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
  do {
    fdc_sendData(FDC_CMD_SENSE_INTERRUPT_STATUS);
    ST0 = fdc_getData();
    fdc_getData(); //Track position
  } while(!(FDC_REG_ST0_END_SEEK & ST0));
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
  fdc_motor_port = g_u8MotorOn = MOTOR_ON;
  fdc_MotorWait();
}


//
// Turn FDC motor off.
//
void fdc_TurnMotorOff(void) {
  fdc_motor_port = g_u8MotorOn = MOTOR_OFF;
}


static void fdc_toggleMotor(void) {
  g_u8MotorOn ? fdc_TurnMotorOff() : fdc_TurnMotorOn();
}

//
// Recalibrate current drive.
// Moves the head to track 0.
//
void fdc_Calibrate(void) /* __naked */ {
  fdc_sendData(FDC_CMD_RECALIBRATE);
  fdc_sendData(FDCIDDR);
  fdc_wait_seek_command_to_finish();
  fdc_MotorWait();
}


//
// Moves the head to the specified track
//
void fdc_GoToTrack(U8 uTrack) {
  g_u8TrackSelection = uTrack;
  fdc_Calibrate();
  fdc_sendData(FDC_CMD_SEEK);
    fdc_sendData(FDCIDDR);
    fdc_sendData(uTrack);
  fdc_wait_seek_command_to_finish();
}


//
// Sets the drive and head to use for the
// FDC commands.
//
void fdc_SelectDrive(U8 uDrive, U8 uHead) {
  g_u8DriveSelection = uDrive;
  FDCHEAD = uHead;
  FDCIDDR = (uHead << 2) | uDrive;  // Drive+Head | b0-1 - Drive number | b2 - Head number
}



//
// Retrieve the ST3 status register.
//
U8 fdc_get_drive_status(void) {
  fdc_sendData(FDC_CMD_SENSE_DRIVE_STATUS);
    fdc_sendData(g_u8DriveSelection);
  
  fdc_wait_ready_to_send_receive_data();
  ST3 = fdc_getData();
  return ST3;
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
  U8 u8MotorOn = g_u8MotorOn;
  fdc_TurnMotorOn();

  do {
    fdc_SelectDrive(!g_u8DriveSelection, 0);
  } while(!fdc_DriveReady());

  if (!u8MotorOn) {
    fdc_TurnMotorOff();
  }
}


//
// Moves the head to the specified track and try
// to find a sector with the specified Sector ID.
//
bool fdc_FindSector(U8 uSectorID) {
  fdc_GoToTrack(g_u8TrackSelection);
  // START - Command
  fdc_sendData(FDC_CMD_READ_SECTOR);
  // Command parameters
    fdc_sendData(FDCIDDR);
    fdc_sendData(g_u8TrackSelection);
    fdc_sendData(FDCHEAD);
    fdc_sendData(uSectorID);
    fdc_sendData(0x02);
    fdc_sendData(uSectorID);
    fdc_sendData(0x52);
    fdc_sendData(0xFF);
  fdc_wait_ready_to_send_receive_data();

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
  // we simply ignore this and wait for the end of the command
  // to extract the result we are interested in.
  fdc_wait_end_of_command_execution();
  ST0 = fdc_getData();
  ST1 = fdc_getData();
  ST2 = fdc_getData();

  // Ignore the rest of the unuseful data for our purposes.
  fdc_getData();
  fdc_getData();
  fdc_getData();
  fdc_getData();

  // Set to true/false if the sector was found.
  return ST1 & (FDC_REG_ST1_SECTOR_NOT_FOUND | FDC_REG_ST1_NO_ADDRESS_MARK);
}
