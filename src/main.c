////////////////////////////////////////////////////////////////////////
// main.c
// DskTest - Small tool for aiding in diagnosing disk drive(s) problems.
// Francisco José Sánchez (pacomix@hotmail.com)
////////////////////////////////////////////////////////////////////////
#include "main.h"
#include "firmware.h"

#include "gate_array.inc"
#include "fdc_funcs.inc"

// Variables defined in crt0_cpc.s to avoid static initialization code.
// TODO - This could also be defined inside a __naked function within an __asm __endasm; block for better clarity...
extern U8 uKeyPressed;
extern U8 g_szBytes[6];  // Temp buffer used to convert from integer/byte to ascii
extern U16 g_sTime;  // Time variable. Contains amount of interruptions happened.
extern U8 g_realTime[5];
extern U8 g_realConstant18000[5];
extern U8 g_realHalf[5];

extern bool g_bSectorIDNotFound;
extern U16 uRPMs;
extern U8 uRPMsDec;
extern U16 uLoops;
extern U8 uPartialSecs;
extern U16 uPartialInts;
extern U8 uReadSectors;
extern U16 uReadBytes;
extern U8 g_realLoops[5];

#define OPTION_COUNT 9
extern const U8 szOptions;

extern const U8 szInfoMsg;
extern U8 uSelectedOption;

#define OPT_DRIVE 0
#define OPT_MOTOR 1
#define OPT_TRACK 2
#define OPT_SECTI 3
#define OPT_RPM   4
#define OPT_UPD   5
#define OPT_READ_SECTORS  6
#define OPT_READ_BYTES  7
#define OPT_TEST_TRACKS  8

#define POS_Y_STAT_DRIVE 1
#define POS_Y_STAT_MOTOR 2
#define POS_Y_STAT_TRACK 3
#define POS_Y_STAT_SECTI 4
#define POS_Y_STAT_RPM   5
#define POS_Y_STAT_UPD   6
#define POS_Y_STAT_READ_SECTORS  7
#define POS_Y_STAT_READ_BYTES  8
#define POS_Y_STAT_TEST_TRACKS  9

#define POS_X_STAT_DRIVE 21
#define POS_X_STAT_MOTOR 21
#define POS_X_STAT_TRACK 21
#define POS_X_STAT_SECTI 21
#define POS_X_STAT_SECTI2 36
#define POS_X_STAT_RPM   5
#define POS_X_STAT_UPD   21
#define POS_X_STAT_READ_SECTORS 21
#define POS_X_STAT_READ_BYTES   21
#define POS_X_STAT_TEST_TRACKS_TRACK_ID   21
#define POS_X_STAT_TEST_TRACKS_SECTOR_ID   27


// Our interruption function. It simply increments our timer.
void myIntFunc(void) __naked {
  __asm
  _myInt:

    di
    push af
    push hl
    ld hl, (#_g_sTime)
    inc hl
    ld (#_g_sTime), hl
    pop hl
    pop af
    ei

    ret
  __endasm;
}

#define ADDR_FIRMWARE_INT 0x0038
#define ADDR_FIRMWARE_JP_ADDR 0x0039
void enable_my_int(void) __naked __sdcccall(1) {
__asm
  di

	ld   hl,           (#ADDR_FIRMWARE_JP_ADDR)
	ld   (#_old_int),  hl

	ld   hl,           #_myIntFunc
	ld   (#ADDR_FIRMWARE_JP_ADDR),    hl

	ei
	ret
__endasm;
}

void disable_my_int(void) __naked __sdcccall(1) {
__asm
  di

	ld   hl,           (#_old_int)
	ld   (#ADDR_FIRMWARE_JP_ADDR),  hl

	ei
	ret
__endasm;
}


void printText(const U8* text) {
  do {
    firm_put_char(*text++);
  } while (*text != '\0');
}


void printNum(U16 uNum, U16 uBase) {
  U8 uStrIdx = 0;
  do {
    U8 uTemp = uNum / uBase;
    g_szBytes[uStrIdx++] = uTemp + 0x30; // 0x30 - ASCII shift to make 0-9 a valid char
    uNum -= uBase * uTemp;
    uBase /= 10;
  } while(uBase != 0);
  g_szBytes[5] = '\0';

  printText(g_szBytes);
}


void printInt(U16 uByte) {
  printNum(uByte, 10000);
}


static void printStatusDrives(void) {
  // Current selected drive
  firm_set_cursor_at(POS_X_STAT_DRIVE, POS_Y_STAT_DRIVE);
  firm_put_char(65 + g_fdc_u8HeadDriveSelection);
}

static void printStatusMotor(void) {
  printText(g_fdc_u8MotorOn ? "\x1F\x15\x2ON!" : "\x1F\x15\x2OFF");
}

static void printStatusTrack(void) {
  // Current selected track
  firm_set_cursor_at(POS_X_STAT_TRACK, POS_Y_STAT_TRACK);
  printInt((U16)g_fdc_u8TrackSelection);
}

static void printStatusSectorID(void) {
  // Current selected Sector ID and result
  firm_set_cursor_at(POS_X_STAT_SECTI, POS_Y_STAT_SECTI);
  printInt((U16)g_fdc_u8SectorSelection);
  printText(g_bSectorIDNotFound ? "\x1F\x24\x04NO!" : "\x1F\x24\x04YES");
}

static void printStatusUpdSecs(void) {
  firm_set_cursor_at(POS_X_STAT_UPD, POS_Y_STAT_UPD);
  printInt((U16) uPartialSecs);
}

static void printStatusReadSectors(void) {
  firm_set_cursor_at(POS_X_STAT_READ_SECTORS, POS_Y_STAT_READ_SECTORS);
  printInt((U16) uReadSectors);
}

static void printStatusReadBytes(void) {
  firm_set_cursor_at(POS_X_STAT_READ_BYTES, POS_Y_STAT_READ_BYTES);
  printInt(uReadBytes);
}

void printStatusRPMs(void) {
  /*
  float fRPMs;
  fRPMs = (uLoops * 36000.0f) / (g_sTime * 1.0f);
  uRPMs = fRPMs; // integer part of the division
  uRPMsDec = (fRPMs - uRPMs) * 100.0f;
  */

  ////fRPMs = ((uLoops * 300) * 60.0f) / (g_sTime * 1.0f);
  // Calc the 0.5f and the 300 * 60  real
  firm_integer_to_real(1, g_realHalf);
  firm_integer_to_real(2, g_realLoops);
  firm_real_division(g_realHalf, g_realLoops);

  firm_integer_to_real(36000, g_realConstant18000);

  firm_integer_to_real(g_sTime, g_realTime);
  firm_integer_to_real(uLoops, g_realLoops);

  firm_real_multiplication(g_realLoops, g_realConstant18000);
  firm_real_division(g_realLoops, g_realTime);

  firm_real_sub(g_realHalf, g_realLoops);  // subtract 0.5 since real_to_integer rounds up when decs. >= 0.5 or down when decs. < 0.5
  uRPMs = firm_real_to_integer(g_realHalf);

  firm_integer_to_real((U16)uRPMs, g_realTime);
  firm_real_sub(g_realTime, g_realLoops);
  firm_integer_to_real((U16)100, g_realLoops);
  firm_real_multiplication(g_realTime, g_realLoops);
  uRPMsDec = (U8) firm_real_to_integer(g_realTime);

  // Print the current calculated RPMs
  firm_set_cursor_at(24, POS_Y_STAT_RPM);
  printInt((U16)uRPMsDec);
  printText("\x1F\x1A\x05.");
  firm_set_cursor_at(21, POS_Y_STAT_RPM);
  printInt(uRPMs);
  
}

TODO - reestructurar opciones
motor on/off - asignar una tecla
drive - asignar tecla

read bytes - enter y meter numero


void printLabels(void) {
  printStatusDrives();
  printStatusMotor();
  printStatusTrack();
  printStatusSectorID();
  printStatusUpdSecs();
  printStatusReadSectors();
  printStatusReadBytes();
  
  // printLabel(&szOptions, 1);
  printText(&szOptions);
  firm_set_cursor_at(1, uSelectedOption+1);  printText(">");
}


static void startRPMs(void) {
  U8 counter;
  printText("\x1F\x0A\x05STARTING");
  fdc_TurnMotorOn();
  fdc_GoToTrack();

  do { // Look for a missing address mark error track and sector
    counter = 0;
    g_fdc_u8SectorSelection++;
    while(counter++ < 8) {
      g_bSectorIDNotFound = fdc_FindSector();
    }
    printStatusSectorID();
  } while(!g_bSectorIDNotFound);

  // Start measuring
  printText("\x1F\x0A\x05RUNNING!");

  uLoops = 0;
  g_sTime = 1;  // Start measuring
}

static void measureRPMs(void) {
  if (g_sTime) {
    // FindSector with a wrong sector ID will finish after 2 full rotations
    // of the disc, so uLoops will end up having the number of rotations / 2.
    // Start with syncing the hole...
    fdc_FindSector();

    // ...and start the measurement.
    enable_my_int();
    fdc_FindSector();    
    disable_my_int();
    uLoops++;

    // Print stats every uPartialSecs
    if (g_sTime > (uPartialSecs * 150)) {
      g_sTime--;
      printStatusRPMs();
      g_sTime = 1;
      uLoops = 0;
    }
  }
}

void myReadBytes(void) {

  U8 uTrack = g_fdc_u8TrackSelection;
  U8 uSector = g_fdc_u8SectorSelection;
  U8* pTargetAddr = (U8*) 0xC000;

  fdc_GoToTrack();
  fdc_read_bytes(pTargetAddr, uReadBytes);

  g_fdc_u8TrackSelection = uTrack;
  g_fdc_u8SectorSelection = uSector;
}

void testTracks(void) {

  g_fdc_u8TrackSelection = 41;

  fdc_TurnMotorOn();
  fdc_GoToTrack();
  for (g_fdc_u8TrackSelection = 0; g_fdc_u8TrackSelection < 42; g_fdc_u8TrackSelection++) {
    fdc_GoToTrack();
    printStatusTrack();
    for (g_fdc_u8SectorSelection = 193; g_fdc_u8SectorSelection < 202; g_fdc_u8SectorSelection++) {
      do {
        g_bSectorIDNotFound = fdc_FindSector();
        printStatusSectorID();
      } while(g_bSectorIDNotFound);
    }
  }
  fdc_TurnMotorOff();
}

void main(void) {

  fdc_toggleDrives();
  
  firm_set_palette_color(0, (U16) 0x1B1B);
  firm_set_palette_color(1, (U16) 0);

  printText(&szInfoMsg);
  printLabels();
  
  do {  // app loop
    uKeyPressed = firm_read_key();
    if (uKeyPressed) {

      if (uKeyPressed == CHAR_CURSOR_UP && uSelectedOption != 0) {
        uSelectedOption -= 1;

      } else if (uKeyPressed == CHAR_CURSOR_DOWN && uSelectedOption < (OPTION_COUNT-1)) {
        uSelectedOption += 1;

      } else if (uKeyPressed == CHAR_CURSOR_LEFT) {
        if (OPT_TRACK == uSelectedOption) {
          g_fdc_u8TrackSelection--;

        } else if (OPT_SECTI == uSelectedOption) {
          g_fdc_u8SectorSelection--;

        } else if (OPT_UPD == uSelectedOption) {
          uPartialSecs -= 1;
          uPartialInts -= 300;

        } else if (OPT_READ_SECTORS == uSelectedOption) {
          uReadSectors--;
          uReadBytes = uReadSectors << 9;

        } else if (OPT_READ_BYTES == uSelectedOption) {
          uReadBytes--;

        }

      } else if (uKeyPressed == CHAR_CURSOR_RIGHT) {
        if (OPT_TRACK == uSelectedOption) {
          g_fdc_u8TrackSelection++;

        } else if (OPT_SECTI == uSelectedOption) {
          g_fdc_u8SectorSelection++;

        } else if (OPT_UPD == uSelectedOption) {
          uPartialSecs += 1;
          uPartialInts += 300;

        } else if (OPT_READ_SECTORS == uSelectedOption) {
          uReadSectors++;
          uReadBytes = uReadSectors << 9;

        } else if (OPT_READ_BYTES == uSelectedOption) {
          uReadBytes++;

        }

      } else if (uKeyPressed == CHAR_ENTER_BIG || uKeyPressed == CHAR_ENTER_SMALL || uKeyPressed == CHAR_COPY) {
        if (OPT_DRIVE == uSelectedOption) {
          fdc_toggleDrives();

        } else if (OPT_MOTOR == uSelectedOption) {
          fdc_toggleMotor();

        } else if (OPT_TRACK == uSelectedOption) {
          fdc_GoToTrack();

        } else if (OPT_SECTI == uSelectedOption) {
          fdc_GoToTrack();
          g_bSectorIDNotFound = fdc_FindSector();

        } else if (OPT_RPM == uSelectedOption) {
          if (!g_sTime) {
            startRPMs();
          } else {
            fdc_toggleMotor();
          }
        
        } else if ((OPT_READ_SECTORS == uSelectedOption) || (OPT_READ_BYTES == uSelectedOption)) {
          myReadBytes();
        } else if (OPT_TEST_TRACKS == uSelectedOption) {
          testTracks();
        }
      }

      // Update measure rpm status based on motor status
      if (!g_fdc_u8MotorOn) {
        printText("\x1F\x0A\x05        ");
        g_sTime = 0;
      } else if (g_sTime) {
        g_sTime = 1;
        uLoops = 0;
        printText("\x1F\x0A\x05RUNNING!");
      }
      printLabels();
    }

    measureRPMs();
  } while(true);
}


