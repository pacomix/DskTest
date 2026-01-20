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

//#include "Keyboard.h"
//#include "firmware.h"
//#include "firmware_fdc.h"
#include "firm_keys.h"
#include "firm_screen.h"
#include "firm_math.h"
#include "firm_text.h"
#include "utils.inc"

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
extern U8 g_uX;
extern U8 g_uY;

#define OPTION_COUNT 5
extern const U8 szOptions;
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


void putchar(const char myChar) {
  if('\n' == myChar) {
    g_uX = 1;
    g_uY += 1;
    firm_set_cursor_at(g_uX, g_uY);
  } else if('\t' == myChar) {
    g_uX += 8;
    firm_set_cursor_at_x(g_uX);
  } else {
    if ((g_uX+1) > CPC_SCR_CHARS_WIDTH) {
      g_uX = 1;
      g_uY += 1;
    }
    //firm_put_char_at(myChar, g_uY, g_uX);
    firm_put_char_at_cursor(myChar);
    //firm_put_char(myChar);
    //firm_put_char(myChar);

    if ('\x7e' == myChar || '\xa0' == myChar || '\xa1' == myChar || '\xa2' == myChar) {
      firm_set_cursor_at(g_uX, g_uY); // Move back the cursor.
      firm_set_blend(true);
    } else {
      firm_set_blend(false);
      g_uX += 1;
    }
  }
}


void printText(const U8* text, U8 x, U8 y) {
  g_uX = x;
  g_uY = y;
  firm_set_cursor_at(g_uX, g_uY);
  do {
    putchar(*text++);
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

  printText(g_szBytes, g_uX, g_uY);
}


void printInt(U16 uByte, U8 x, U8 y) {
  g_uX = x;
  g_uY = y;
  printNum(uByte, 10000);
}


void printByte(U8 uByte, U8 x, U8 y) {
  g_uX = x;
  g_uY = y;
  printNum((U16) uByte, 10000);
}

static void printLabel(const U8* pszLabel, U8 yPos) {
  printText(pszLabel, 1, yPos);
}


static void printStatusDrives(void) {
  { // Detected available drives
    U8 uCounter=0;
    g_uX = 21;
    firm_set_cursor_at(g_uX, POS_Y_STAT_DRIVE);
    do {
      if(uDrives[uCounter]) {
        putchar(uCounter + 65);
      }
    } while(++uCounter != MAX_DRIVES);
      
  }

  { // Current selected drive
    g_uX = 21;
    firm_set_cursor_at(g_uX+uDrive, POS_Y_STAT_DRIVE);
    firm_set_inverse();
    putchar(65 + uDrive);
    firm_set_inverse();
  }
}

static void printStatusMotor(void) {
  printText(uMotor ? " ON" : "OFF", 21, POS_Y_STAT_MOTOR);
}

static void printStatusTrack(void) {
  // Current selected track
  printByte(uTrack, 21, POS_Y_STAT_TRACK);
}

static void printStatusSectorID(void) {
  // Current selected Sector ID and result
  printByte(uSectorID, 21, POS_Y_STAT_SECTI);
  printText(uFoundErrorSectorID ? "NO!" : "YES", 37, POS_Y_STAT_SECTI);
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
  firm_integer_to_real(18000, g_realConstant18000);

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
  printByte(uRPMsDec, 24, POS_Y_STAT_RPM);
  printText(".", 26, POS_Y_STAT_RPM);
  printInt(uRPMs, 21, POS_Y_STAT_RPM);
  
}


void printLabels(void) {
  printStatusDrives();
  printStatusMotor();
  printStatusTrack();
  printStatusSectorID();
  
  printLabel(&szOptions, 1);
  printLabel(">", uSelectedOption+1);
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

  firm_set_screen_mode(CPC_SCR_MODE);
  //firm_txt_vdu_enable();
  //firm_txt_reset();
  //firm_txt_init();

  checkAvailableDrives();
  fdc_SelectDrive(0, 0);

  // printWarning();
  //printText("::: WARNING :::", 32, 16);
  //printText("USE IT AT YOUR OWN RISK", 28, 18);
  //printText("DskTest v1.0-RC1\nFrancisco Jos""\xA1""e <PACOMIX> S""\xA1""anchez - https://linkedin.com/in/pacomix", 1, 24);
  
  // firm_set_palette_color(0, 0b0000001100000011);
  // firm_set_palette_color(1, 0b0001100000011000);

  do {
    //printLabels use only this one and avoid the bottom ones. investigate why the screen scrolls...
    printLabels();
    uKeyPressed = firm_get_key_wait();

    __asm HALT __endasm;
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
        printText("STARTING", 9, POS_Y_STAT_RPM);
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
        printText("RUNNING!", 9, POS_Y_STAT_RPM);
        fdc_FindSector(uSectorID, uTrack, &uFoundErrorSectorID);

        uLoops = 0;
        g_sTime = 0;
        uElapsedSeconds = 600;
        enable_my_int();
        do {
          // FindSector with a wrong sector ID will finish after 2 full rotations
          // of the disc, so uLoops will end up having the number of rotations / 2.
          fdc_FindSector(uSectorID, uTrack, &uFoundErrorSectorID);
          uLoops += 2;
          
          if (g_sTime > uElapsedSeconds) {
            uElapsedSeconds += 600;
            printStatusRPMs();
          }

        } while(g_sTime <= 18000);
        disable_my_int();

        myTurnMotorOff();

        // Test 292 rpms
        //uLoops = 39;
        printStatusRPMs();
      }
    }

    // TODO - funny - If we put the printStatus at the bottom the code increases +140 bytes
    //printStatus();

    // In/de crease track
    /*

    // Drive
    if(!cpc_TestKeyF(6)) {
      g_Keys |= FLAG_KEY_6;
    } else if (FLAG_KEY_6 & g_Keys) {
      g_Keys &= ~FLAG_KEY_6;

      checkAvailableDrives();
      do {
        uDrive++;
        if(MAX_DRIVES == uDrive) {
          uDrive = 0;
        }
      } while(0 == uDrives[uDrive]);

      ConfigureFDC(uDrive, 0);
    }

    // Measure RPMs
    if(!cpc_TestKeyF(5)) {
      g_Keys |= FLAG_KEY_5;
    } else if (FLAG_KEY_5 & g_Keys) {
      U8 bFound = 0;
      g_Keys &= ~FLAG_KEY_5;
      bMeasuring = 1;

      myTurnMotorOn();
      fdc_Calibrate();

      // Look for a missing address mark error track and sector
      do {
        GoToTrack(uTrack);
        {
          U8 counter;
          bSearchingSector = 1;
          for(counter = 0, uFoundErrorSectorID = 1; 15 != counter && uFoundErrorSectorID; counter++) {
            FindSector(uSectorID, uTrack, &uFoundErrorSectorID);
          }
          bFound = 15 == counter ? 1 : 0;
          bSearchingSector = 0;
        }

        uSectorID++;
        printStatus();
      } while(!bFound && 255 != uSectorID);
      uSectorID--;
      printText("@@@@@@@@", 62, 40);
      printText("RUNNING???", 62, 48);

      // Start measuring
      FindSector(uSectorID, uTrack, &uFoundErrorSectorID);

      g_sTime = -6;
      uLoops = 0;
      do {
        // FindSector with a wrong sector ID will finish after 2 full rotations
        // of the disc, so uLoops will end up having the number of rotations / 2.
        FindSector(uSectorID, uTrack, &uFoundErrorSectorID);
        uLoops++;
      } while(3000 >= g_sTime);

      // We have now enough rotations accrued over time so let's calc the RPMs
      __asm di __endasm; // Disable interrupts so we don't update anymore g_sTime
      uLoops <<= 1;
      {
        float fRPMs = ((uLoops * 300) * 60.0f) / (g_sTime * 1.0f);
        uRPMs = fRPMs; // integer part of the division
        uRPMsDec = (fRPMs - uRPMs) * 100.0f;
      }
      __asm ei __endasm;

      bMeasuring = 0;
      printText(":FINISHED:", 62, 48);
      myTurnMotorOff();

    }*/
  } while(true);
}


