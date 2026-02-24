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
extern U8 g_u8KeyPressed;
extern U8 g_szBytes[6];  // Temp buffer used to convert from integer/byte to ascii
extern U16 g_u16Time;  // Time variable. Contains amount of interruptions happened.
extern U8 g_realTime[5];
extern U8 g_realConstant18000[5];
extern U8 g_realHalf[5];

extern bool g_bSectorIDNotFound;
extern U16 g_u16RPMs;
extern U8 g_u8RPMsDec;
extern U16 g_u16Loops;
extern U8 g_u8PartialSecs;
extern U16 g_u16PartialInts;
extern U8 g_realLoops[5];

#define OPTION_COUNT 3
extern const U8 szOptions;

extern const U8 szInfoMsg;
extern U8 g_u8SelectedOption;

#define OPT_TEST_RPM   0
#define OPT_TEST_SECID 1
#define OPT_TEST_TRACKS  2

#define POS_Y_TEST_RPM   3
#define POS_Y_FIND_SECID 4
#define POS_Y_STAT_TEST_TRACKS  5

#define POS_Y_STATS 1

// Main stat bar
#define POS_X_STAT_DRIVE 9
#define POS_X_STAT_MOTOR 21
#define POS_X_STAT_UPD   37
#define POS_X_STAT_TRACK 50
#define POS_X_STAT_SECID 66
#define POS_X_STAT_SECID_FOUND 77


// Our interruption function. It simply increments our timer.
void myIntFunc(void) __naked {
  __asm
  _myInt:

    di
    push af
    push hl
    ld hl, (#_g_u16Time)
    inc hl
    ld (#_g_u16Time), hl
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

	ld hl, (#ADDR_FIRMWARE_JP_ADDR)
	ld (#_old_int),  hl

	ld hl, #_myIntFunc
	ld (#ADDR_FIRMWARE_JP_ADDR),    hl

	ei
	ret
__endasm;
}

void disable_my_int(void) __naked __sdcccall(1) {
__asm
  di

	ld hl, (#_old_int)
	ld (#ADDR_FIRMWARE_JP_ADDR),  hl

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
  g_szBytes[uStrIdx] = '\0';

  printText(g_szBytes);
}

void printByte(U8 u8Num, U16 u16Base) {
  printNum((U16) u8Num, u16Base);
}

void printInt(U16 u16Num, U16 u16Base) {
  printNum(u16Num, u16Base);
}


static void printStatusDrives(void) {
  // Current selected drive
  firm_set_cursor_at(POS_X_STAT_DRIVE, POS_Y_STATS);
  firm_put_char(65 + g_fdc_u8HeadDriveSelection);
}

static void printStatusMotor(void) {
  printText(g_fdc_u8MotorOn ? "\x1F\x15\x1ON!" : "\x1F\x15\x1OFF");
}

static void printStatusTrack(void) {
  // Current selected track
  firm_set_cursor_at(POS_X_STAT_TRACK, POS_Y_STATS);
  printByte(g_fdc_u8TrackSelection, 100);
}

static void printStatusSectorID(void) {
  // Current selected Sector ID and result
  firm_set_cursor_at(POS_X_STAT_SECID, POS_Y_STATS);
  printByte(g_fdc_u8SectorSelection, 100);
  printText(g_bSectorIDNotFound ? "\x1F\x4D\x01NO!" : "\x1F\x4D\x01YES");
}

static void printStatusUpdSecs(void) {
  firm_set_cursor_at(POS_X_STAT_UPD, POS_Y_STATS);
  printByte(g_u8PartialSecs, 100);
}

void printStatusRPMs(void) {
  /*
  float fRPMs;
  fRPMs = (g_u16Loops * 36000.0f) / (g_u16Time * 1.0f);
  g_u16RPMs = fRPMs; // integer part of the division
  g_u8RPMsDec = (fRPMs - g_u16RPMs) * 100.0f;
  */

  ////fRPMs = ((g_u16Loops * 300) * 60.0f) / (g_u16Time * 1.0f);
  // Calc the 0.5f and the 300 * 60  real
  firm_integer_to_real(1, g_realHalf);
  firm_integer_to_real(2, g_realLoops);
  firm_real_division(g_realHalf, g_realLoops);

  firm_integer_to_real(36000, g_realConstant18000);

  firm_integer_to_real(g_u16Time, g_realTime);
  firm_integer_to_real(g_u16Loops, g_realLoops);

  firm_real_multiplication(g_realLoops, g_realConstant18000);
  firm_real_division(g_realLoops, g_realTime);

  firm_real_sub(g_realHalf, g_realLoops);  // subtract 0.5 since real_to_integer rounds up when decs. >= 0.5 or down when decs. < 0.5
  g_u16RPMs = firm_real_to_integer(g_realHalf);

  firm_integer_to_real(g_u16RPMs, g_realTime);
  firm_real_sub(g_realTime, g_realLoops);
  firm_integer_to_real((U16)100, g_realLoops);
  firm_real_multiplication(g_realTime, g_realLoops);
  g_u8RPMsDec = (U8) firm_real_to_integer(g_realTime);

  // Print the current calculated RPMs
  firm_set_cursor_at(23, POS_Y_TEST_RPM);
  printByte(g_u8RPMsDec, 10);
  printText("\x1F\x16\x03.");
  firm_set_cursor_at(19, POS_Y_TEST_RPM);
  printInt(g_u16RPMs, 100);
  
}


void printLabels(void) {
  printStatusDrives();
  printStatusMotor();
  printStatusTrack();
  printStatusSectorID();
  printStatusUpdSecs();
  
  printText(&szOptions);
  firm_set_cursor_at(1, g_u8SelectedOption+3);
  printText(">");
}

static void prepareHeadToTrack(void) {
  fdc_TurnMotorOn();
  fdc_Recalibrate();
  fdc_Recalibrate();
  fdc_GoToTrack();
}

static void startRPMs(void) {
  U8 counter;
  
  prepareHeadToTrack();

  do { // Look for a missing address mark error track and sector
    counter = 0;
    g_fdc_u8SectorSelection++;
    while(counter++ < 8) {
      g_bSectorIDNotFound = fdc_FindSector();
    }
    printStatusSectorID();
  } while(!g_bSectorIDNotFound);

  g_u16Loops = 0;
  g_u16Time = 1;  // Start measuring
}

static void measureRPMs(void) {
  if (g_u16Time) {
    // FindSector with a wrong sector ID will finish after 2 full rotations
    // of the disc, so g_u16Loops will end up having the number of rotations / 2.
    // Start with syncing the hole...
    fdc_FindSector();

    // ...and start the measurement.
    enable_my_int();
    fdc_FindSector();    
    disable_my_int();
    g_u16Loops++;

    // Print stats every g_u8PartialSecs
    if (g_u16Time > (g_u8PartialSecs * 150)) {
      g_u16Time--;
      printStatusRPMs();
      g_u16Time = 1;
      g_u16Loops = 0;
    }
  }
}

void testTracks(void) {
  U8 u8TargetTrack = g_fdc_u8TrackSelection+1;
  U8 u8TargetSector = g_fdc_u8SectorSelection + 9;
  U8 u8InitialSector = g_fdc_u8SectorSelection;

  prepareHeadToTrack();
  for (g_fdc_u8TrackSelection = 0; g_fdc_u8TrackSelection < u8TargetTrack; g_fdc_u8TrackSelection++) {
    fdc_GoToTrack();
    printStatusTrack();
    for (g_fdc_u8SectorSelection = u8InitialSector; g_fdc_u8SectorSelection < u8TargetSector; g_fdc_u8SectorSelection++) {
      do {
        g_bSectorIDNotFound = fdc_FindSector();
        printStatusSectorID();
      } while(g_bSectorIDNotFound);
    }
  }
  fdc_TurnMotorOff();
  g_fdc_u8SectorSelection = u8InitialSector;
}

void main(void) {

  fdc_toggleDrives();
  
  firm_set_palette_color(0, (U16) 0);
  firm_set_palette_color(1, (U16) 0x1B1B);

  printText(&szInfoMsg);
  printLabels();
  
  // Main app loop
  do {
    g_u8KeyPressed = firm_read_key();
    if (g_u8KeyPressed) {
      if (FIRM_KEY_CURSOR_UP == g_u8KeyPressed && g_u8SelectedOption) {
        g_u8SelectedOption--;
      } else if (FIRM_KEY_CURSOR_DOWN == g_u8KeyPressed && g_u8SelectedOption < (OPTION_COUNT-1)) {
        g_u8SelectedOption++;
      } else if ('1' == g_u8KeyPressed) {
          fdc_toggleDrives();
      } else if ('2' == g_u8KeyPressed) {
        fdc_toggleMotor();
      } else if ('q' == g_u8KeyPressed) {
        g_u8PartialSecs -= 1;
        g_u16PartialInts -= 300;
      } else if ('w' == g_u8KeyPressed) {
        g_u8PartialSecs += 1;
        g_u16PartialInts += 300;
      } else if ('a' == g_u8KeyPressed) {
        g_fdc_u8TrackSelection--;
      } else if ('s' == g_u8KeyPressed) {
        g_fdc_u8TrackSelection++;
      } else if ('z' == g_u8KeyPressed) {
        g_fdc_u8SectorSelection--;
      } else if ('x' == g_u8KeyPressed) {
        g_fdc_u8SectorSelection++;
      } else if (g_u8KeyPressed == FIRM_KEY_ENTER_BIG || g_u8KeyPressed == FIRM_KEY_ENTER_SMALL || g_u8KeyPressed == FIRM_KEY_COPY) {
        if (OPT_TEST_SECID == g_u8SelectedOption) {
          prepareHeadToTrack();
          g_bSectorIDNotFound = fdc_FindSector();
          fdc_TurnMotorOff();
        } else if (OPT_TEST_RPM == g_u8SelectedOption) {
          if (!g_u16Time) {
            startRPMs();
          } else {
            fdc_toggleMotor();
          }
        } else if (OPT_TEST_TRACKS == g_u8SelectedOption) {
          testTracks();
        }
      }

      // Update measure rpm status based on motor status
      if (!g_fdc_u8MotorOn) {
        g_u16Time = 0;
      } else if (g_u16Time) {
        g_u16Time = 1;
        g_u16Loops = 0;
      }
      printLabels();
    }

    measureRPMs();
  } while(true);
}
