////////////////////////////////////////////////////////////////////////
// main.c
// Dsk Test - Small tool for aiding in diagnosing disk drive(s) problems.
// Francisco José Sánchez (pacomix@hotmail.com)
////////////////////////////////////////////////////////////////////////

typedef unsigned char U8;
typedef unsigned short int U16;
typedef signed char S8;
typedef signed short int S16;
typedef unsigned long int U32;
typedef signed long int I32;

#define false 0
#define true 1
#define OFF false
#define ON true
#define FALSE false
#define TRUE true
#define bool U8
#define BOOL bool
#define MOTOR_OFF OFF
#define MOTOR_ON ON

// Set the graphic mode
// TODO - Implement and TEST functionality when loaded from a CPC464. Distribute TWO .dsks
#define CPC_MODEL_6128
#define CPC_MODE2

# if defined (CPC_MODE0)
#   define CPC_SCR_MODE 0
#   define CPC_SCR_CHARS_WIDTH 40

# elif defined (CPC_MODE1)
#   define CPC_SCR_MODE 1
#   define CPC_SCR_CHARS_WIDTH 40

# elif defined (CPC_MODE2)
#   define CPC_SCR_MODE 2
#   define CPC_SCR_CHARS_WIDTH 80

# else
#   error Graphic mode not defined!!!
#endif

//#include "Keyboard.h"
//#include "firmware.h"
//#include "firmware_fdc.h"
#include "firm_keys.h"
#include "firm_screen.h"
#include "firm_math.h"
#include "firm_text.h"
#include "utils.inc"



extern U8 uKeyPressed;
extern U8 g_szBytes[6];  // Temp buffer used to convert from integer/byte to ascii
extern U16 g_sTime;  // Time variable. Contains amount of interruptions happened.
extern U8 g_realTime[5];
extern U8 g_realConstant18000[5];
extern U8 g_realHalf[5];

extern U8 uTrack;
extern U8 uSectorID;
extern U8 uFoundErrorSectorID;
extern U16 uRPMs;
extern U8 uRPMsDec;
extern U16 uLoops;
extern U16 uElapsedSeconds;
extern U8 g_realLoops[5];
extern U8 uMotor;
extern U8 uDrive;

#define MAX_DRIVES 2
extern U8 uDrives[MAX_DRIVES];

#define OPTION_COUNT 5
extern const U8 szOptions;

extern const U8 szInfoMsg;
extern U8 uSelectedOption;

#define OPT_DRIVE 0
#define OPT_MOTOR 1
#define OPT_TRACK 2
#define OPT_SECTI 3
#define OPT_RPM   4

#define POS_Y_STAT_DRIVE 1
#define POS_Y_STAT_MOTOR 2
#define POS_Y_STAT_TRACK 3
#define POS_Y_STAT_SECTI 4
#define POS_Y_STAT_RPM   5

#define POS_X_STAT_DRIVE 21
#define POS_X_STAT_MOTOR 21
#define POS_X_STAT_TRACK 21
#define POS_X_STAT_SECTI 21
#define POS_X_STAT_SECTI2 36
#define POS_X_STAT_RPM   5


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
  { // Detected available drives
    U8 uCounter=0;
    firm_set_cursor_at(POS_X_STAT_DRIVE, POS_Y_STAT_DRIVE);
    do {
      if(uDrives[uCounter]) {
        firm_put_char(uCounter + 65);
      }
    } while(++uCounter != MAX_DRIVES);
      
  }

  { // Current selected drive
    firm_set_cursor_at(POS_X_STAT_DRIVE+uDrive, POS_Y_STAT_DRIVE);
    firm_set_inverse();
    firm_put_char(65 + uDrive);
    firm_set_inverse();
  }
}

static void printStatusMotor(void) {
  printText(uMotor ? "\x1F\x15\x2ON!" : "\x1F\x15\x2OFF");
}

static void printStatusTrack(void) {
  // Current selected track
  firm_set_cursor_at(POS_X_STAT_TRACK, POS_Y_STAT_TRACK);
  printInt((U16)uTrack);
}

static void printStatusSectorID(void) {
  // Current selected Sector ID and result
  firm_set_cursor_at(POS_X_STAT_SECTI, POS_Y_STAT_SECTI);
  printInt((U16)uSectorID);
  printText(uFoundErrorSectorID ? "\x1F\x24\x04NO!" : "\x1F\x24\x04YES");
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


void printLabels(void) {
  printStatusDrives();
  printStatusMotor();
  printStatusTrack();
  printStatusSectorID();
  
  // printLabel(&szOptions, 1);
  printText(&szOptions);
  firm_set_cursor_at(1, uSelectedOption+1);  printText(">");
}


void myTurnMotorOn(void) {
  fdc_TurnMotorOn();
  uMotor = MOTOR_ON;
}


void myTurnMotorOff(void) {
  fdc_TurnMotorOff();
  uMotor = MOTOR_OFF;
}


void checkAvailableDrives(void) {
  myTurnMotorOn();
  uDrives[0] = fdc_DriveReady(0);
  uDrives[1] = fdc_DriveReady(1);
  myTurnMotorOff();
}


static void ToggleMotor(void) {
  if(uMotor == MOTOR_ON) {
    myTurnMotorOff();
  } else {
    myTurnMotorOn();
  }
}



void main(void) {

  checkAvailableDrives();
  fdc_SelectDrive(0, 0);
  
  // firm_set_palette_color(0, 0b0000001100000011);
  // firm_set_palette_color(1, 0b0001100000011000);

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
          uTrack -= uTrack > 0 ? 1 : 0;
        } else if (OPT_SECTI == uSelectedOption) {
          uSectorID -= uSectorID > 0 ? 1 : 0;
        }

      } else if (uKeyPressed == CHAR_CURSOR_RIGHT) {

        if (OPT_TRACK == uSelectedOption) {
          uTrack += uTrack < 41 ? 1 : 0;

        } else if (OPT_SECTI == uSelectedOption) {
          uSectorID += uSectorID < 255 ? 1 : 0;

        }

      } else if (uKeyPressed == CHAR_ENTER_BIG || uKeyPressed == CHAR_ENTER_SMALL || uKeyPressed == CHAR_COPY) {

        if (OPT_MOTOR == uSelectedOption) {
          ToggleMotor();

        } else if (OPT_TRACK == uSelectedOption) {
          fdc_GoToTrack(uTrack);

        } else if (OPT_SECTI == uSelectedOption) {
          fdc_FindSector(uSectorID, uTrack, &uFoundErrorSectorID);

        } else if (OPT_RPM == uSelectedOption) {
          printText("\x1F\x09\x05STARTING");
          U8 counter;
          myTurnMotorOn();
          do { // Look for a missing address mark error track and sector
            uSectorID++;
            printStatusSectorID();
            for(counter = 0, uFoundErrorSectorID = true; counter != 15 && uFoundErrorSectorID; counter++) {
              fdc_FindSector(uSectorID, uTrack, &uFoundErrorSectorID);
            }
          } while(counter != 15);
          printStatusSectorID();

          // Start measuring
          printText("\x1F\x09\x05RUNNING!");

          uLoops = 0;
          g_sTime = 1;
          uElapsedSeconds = 300;
          do {
            // FindSector with a wrong sector ID will finish after 2 full rotations
            // of the disc, so uLoops will end up having the number of rotations / 2.
            // Start with syncing the hole...
            fdc_FindSector(uSectorID, uTrack, &uFoundErrorSectorID);
            // ...and start the measurement.
            enable_my_int();
            fdc_FindSector(uSectorID, uTrack, &uFoundErrorSectorID);
            disable_my_int();
            uLoops++;

            // Print stats every second. The longer it runs the more accurate the measurement will be.
            if (g_sTime > uElapsedSeconds) {
              uElapsedSeconds += 300;
              printStatusRPMs();
            }

          } while(g_sTime <= 18000);

          myTurnMotorOff();

          // Test 292 rpms
          //uLoops = 39;
          printStatusRPMs();
        }
      }
      printLabels();
    }
  } while(true);
}


