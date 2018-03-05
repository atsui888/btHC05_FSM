#ifndef BTHC05_I_H
#define BTHC05_I_H

/*
    btHC05_I.h - Private vars and Private fns for btHC05 module
                 Public vars and fns are in btHC05_P.h
*/

/*
    Installation and Other useful Info
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Bluetooth module used: HC05
  
    for Mega
    ~~~~~~~~
    Ard Pin --- Level Shifter ---   HC05 Pin
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Gnd         HV_Gnd,             HC05_Gnd and LED13_Gnd(if LED13 used)
    5v          HV                  HC05_VCC
    3v          HV
    d19         HV2---LV2           HC05_TX   // Serial1.Rx
    d18         HV1---LV1           HC05_RX   // Serial1.Tx

    for Uno (use software serial to keep serial monitor available)
    ~~~~~~~~
    Ard Pin --- Level Shifter ---   HC05 Pin
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    Gnd         HV_Gnd,             HC05_Gnd, LED13_Gnd
    5v          HV                  HC05_VCC
    3v          HV
    d10         HV2---LV2           HC05_TX
    d11         HV1---LV1           HC05_RX
    
    for Uno:
        // #include <SoftwareSerial.h>
        // SoftwareSerial Bluetooth(10, 11); // RX, TX
        // and change all "Serial1.abc" to "Bluetooth.abc"

    Communications
    ~~~~~~~~~~~~~~
    Sending Ctrl Dev must send HEX with no CR, no LF and no other 
    terminators except:
        '<' == start of packet
        '>' == end of packet    

    valid packet is:
    <n0,n1,n2,n3,n4,n5,n6>
      where
        '<' is start marker and '>' is end marker
        CRC to be implemented in future

    example of valid packet:
        <n0,n1,n2,n3,n4,n5>
        <52, 1, 5, 0, 2, 2>
        where
            deviceID = 52 (id)
            CtrlDev  = 1  (is on)
            dir      = 5  (at rest)
            mPwr     = 0  (motor pwr is 0)
            camPole  = 2  (camPole stop)
            ca       = 2  (CA is On)

*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                  PRIVATE VARS - only for use by btHC05.ino
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
typedef enum {
    btHC05_stIdle     = 1,
    btHC05_stRiP      = 2,    
    btHC05_stRecErr   = 3
} btHC05_STATE;

typedef enum {          
    btHC05_evNone             = 0,
    btHC05_evStartMarkRec     = 1,
    btHC05_evHasData          = 2,
    btHC05_evEndMarkRec       = 3,
    btHC05_evTimeOut          = 4
} btHC05_EVENTS;

btHC05_STATE btHC05_State;
btHC05_STATE btHC05_State_Prev;     // if short of processor cycles, can consider removing this
btHC05_EVENTS btHC05_Event;         
btHC05_EVENTS btHC05_Event_Prev;    // if short of processor cycles, can consider removing this

const uint8_t btHC05_Inputs_Len = 6;
uint8_t  btHC05_Inputs[btHC05_Inputs_Len] = {255, 255, 255, 255, 255, 255}; 
// 255 is unused, 0 is used by mPwr

uint32_t btHC05_TimeOfLastData = 0;
uint32_t btHC05_TimeNow = 0;
uint32_t btHC05_TimeOut_Treshold = 8000;    // how many ms before timeout is called (if state = RiP)?

uint8_t btHC05_Inbyte = 0;
uint8_t HC05_tInputPkt_Idx = 0;
uint8_t btHC05_DebugArrLen = 30;


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                  PRIVATE FUNCTIONS - only for use by btHC05.ino
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static void     btHC05_GetInput();
static void     btHC05_RunFSM();

static void     btHC05_ResetInputsArr();
static void     btHC05_ResetPubVars();  // if time-out, reset published var
static void     btHC05_ResetDataInputTimer();
static uint8_t  btHC05_IsPacketValid(); // only chks for correct # of bytes, not the logic, not the ranges

static void     HC05_tStartPkt();
static uint8_t  HC05_tInputPkt();
static void     HC05_tDiscardPkt();
static void     HC05_tChkPktNPub();

static void     btHC05_DebugPrint(char* tFn_Name);
static void     btHC05_GetEventName(uint8_t evID, char *strEvName);
static void     btHC05_GetStName(uint8_t st_cur, char *strStName);
static void     btHC05_PrintInputsArr();
static void     btHC05_DebugPrint_PublishedVars();


#endif // BTHC05_I_H
