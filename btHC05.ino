#include "btHC05_I.h"       // internal
#include "btHC05_P.h"       // public

static void btHC05_DebugPrint(char* tFn_Name) {
    // next step: include name of t_fn called
    char strEvName[btHC05_DebugArrLen] = "";
    char strStName[btHC05_DebugArrLen] ="";
    
    //Serial.println();    
    //Serial.print("Current State         --> ");    
    //btHC05_GetStName(btHC05_State, strStName);
    //Serial.println(strStName);                    

    Serial.print("Current Event         --> ");    
    btHC05_GetEventName(btHC05_Event, strEvName);
    Serial.println(strEvName);    

    Serial.print("Translation Fn called --> ");
    Serial.println((char *)tFn_Name);

    Serial.print("Current Input Idx     --> ");    
    Serial.println(HC05_tInputPkt_Idx);

    btHC05_PrintInputsArr();     
}

static void btHC05_GetEventName(uint8_t evID, char *strEvName) {    
    switch(evID) {
        case btHC05_evNone:
            strcpy(strEvName,"btHC05_evNone");
            break;
        case btHC05_evStartMarkRec:
            strcpy(strEvName,"btHC05_evStartMarkRec");            
            break;
        case btHC05_evHasData:
            strcpy(strEvName,"btHC05_evHasData");            
            break;
        case btHC05_evEndMarkRec:
            strcpy(strEvName,"btHC05_evEndMarkRec");
            break;
        case btHC05_evTimeOut:
            strcpy(strEvName,"btHC05_evTimeOut");
            break;
        default:
            break;        
    }    
}

static void btHC05_GetStName(uint8_t st_cur, char *strStName) {
    switch(st_cur) {
        case btHC05_stIdle: 
            strcpy(strStName,"btHC05_stIdle");
            break;
        case btHC05_stRiP:
            strcpy(strStName,"btHC05_stRiP");
            break;
    case btHC05_stRecErr:
            strcpy(strStName,"btHC05_stRecErr");
            break;
        default:
            break;
    }
}

static void btHC05_DebugPrint_PublishedVars() {
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL                        
            
            Serial.println("btHC05 Published vars: ");
            Serial.print("btHC05_PubVars[0]:    --> ");
            Serial.println(btHC05_PubVars[0]);
            
            Serial.print("btHC05_PubVars[1]:    --> ");
            Serial.println(btHC05_PubVars[1]);
            
            Serial.print("btHC05_PubVars[2]:    --> ");
            Serial.println(btHC05_PubVars[2]);
            
            Serial.print("btHC05_PubVars[3]:    --> ");
            Serial.println(btHC05_PubVars[3]);
            
            Serial.print("btHC05_PubVars[4]:    --> ");
            Serial.println(btHC05_PubVars[4]);
            
            Serial.print("btHC05_PubVars[5]:    --> ");
            Serial.println(btHC05_PubVars[5]);
            
            Serial.print("Time of last update:  --> ");
            Serial.println(btHC05_PubVar_TimeOfLastUpdate);
        
            Serial.println();
        #endif
    #endif    
}

static void btHC05_ResetInputsArr() {    
    for (uint8_t i = 0; i < btHC05_Inputs_Len; ++i) {
        btHC05_Inputs[i] = 255;
    }
}

static void btHC05_PrintInputsArr() {
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL             
            Serial.print("btHC05_Input[ ] is    --> ");
            for (uint8_t i = 0; i < btHC05_Inputs_Len; ++i) {
                Serial.print(btHC05_Inputs[i]);
                Serial.print(", ");
            }
            Serial.println();
        #endif
    #endif
}

static void btHC05_ResetDataInputTimer() {
    btHC05_TimeOfLastData = 0;
    btHC05_TimeNow = 0;
}

static void btHC05_ResetPubVars() {
    for (uint_fast8_t i = 0; i < btHC05_Inputs_Len; ++i) {
        btHC05_PubVars[i] = 255;
    }
}

void btHC05_Init() {
    // public fn

    btHC05_ResetInputsArr();
    btHC05_ResetPubVars();
    btHC05_PubVar_TimeOfLastUpdate = 0;
    btHC05_ResetDataInputTimer();

    // set initial event and state
    btHC05_Event = btHC05_evNone;
    btHC05_State = btHC05_stIdle;

    Serial1.begin(9600);
    Serial1.println("Robot MCU bluetooth comms ready for your command!");

}

static void btHC05_GetInput() {
    // get input or 'lack of input' from bluetooth ctrl dev and generate events
    // .receive start marker, .receive end marker, .receive data, .receive nothing
    // uses d18,19 serial1       
    
    // Inbyte and Ev needs to be regen again based on new data or the lack of
    btHC05_Inbyte = 0;    
    btHC05_Event_Prev = btHC05_Event;
    btHC05_Event = btHC05_evNone;

    if (Serial1.available()) {
        // there is data
        btHC05_Inbyte = Serial1.read();
        #ifdef DEBUG_BTHC05_ON
            #ifdef SERIAL_MONITOR_AVAIL 
                Serial.println();
                Serial.print("(Dec) Inbyte = ");
                Serial.println(btHC05_Inbyte);
            #endif
        #endif
        switch (btHC05_Inbyte) {
            case '\n': // end of line, do nothing, we don't need it.
                btHC05_Event = btHC05_evNone;    
                break;

            case '\r': // discard carriage return
                btHC05_Event = btHC05_evNone;    
                break;
                        
            case '<':   
                btHC05_Event = btHC05_evStartMarkRec; 
                btHC05_TimeOfLastData = millis();   // mark time when data is received.
                break;

            case '>':   
                btHC05_Event = btHC05_evEndMarkRec;                
                btHC05_TimeOfLastData = millis();   // mark time when data is received.
                break;

            default :
                btHC05_Event = btHC05_evHasData;                
                btHC05_TimeOfLastData = millis();   // mark time when data is received.
                break;
        }
    } else {        // No data received in the hardware serial buffer
        
        // if RiP && timeout, set event to ev_TimeOut, else evNone
        if(btHC05_State == btHC05_stRiP) {        
            btHC05_TimeNow = millis();
            if ((btHC05_TimeNow - btHC05_TimeOfLastData) > btHC05_TimeOut_Treshold) {
                // time since last data rec > treshold            
                btHC05_Event = btHC05_evTimeOut;
                 
                #ifdef DEBUG_BTHC05_ON
                    #ifdef SERIAL_MONITOR_AVAIL
                        Serial.println();
                        Serial.println("*******************************************************************");
                        Serial.println("*******************************************************************");
                        Serial.println("*******************************************************************");
                        Serial.println("stRiP: TimeOut!");                        
                        Serial.println("*******************************************************************");
                        Serial.println("*******************************************************************");
                        Serial.println("*******************************************************************");
                        Serial.println();
                    #endif
                #endif           
            } else {            
                btHC05_Event = btHC05_evNone;
            }
        }        
    }
}

static void HC05_tStartPkt() {    
    // prep for start of incoming packet, nothing to do with Published Vars

    // resets
    btHC05_ResetInputsArr();    
    HC05_tInputPkt_Idx = 0;     
    
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL 
            btHC05_DebugPrint((char*)"tStartPacket()");
        #endif
    #endif   
}

static uint8_t HC05_tInputPkt() {
    // checks for OOB and move inByte to inputBuffer if no OOB, incr idx, and return Success (1)
    // else return failure (0)    
    
    if(HC05_tInputPkt_Idx < btHC05_Inputs_Len) {
        btHC05_Inputs[HC05_tInputPkt_Idx] =  btHC05_Inbyte;
        
        #ifdef DEBUG_BTHC05_ON
            #ifdef SERIAL_MONITOR_AVAIL
                btHC05_DebugPrint((char*)"tInputPacket()");    
            #endif
        #endif
        
        HC05_tInputPkt_Idx++;
        return 1;   // Success
        
    } else {        // OOB - Fail
        HC05_tInputPkt_Idx = 0;        
        #ifdef DEBUG_BTHC05_ON
            #ifdef SERIAL_MONITOR_AVAIL
                Serial.println("tInputPkt() - OOB - Failed!");
            #endif
        #endif
        
        return 0;      // return failure        
    }
}

static void HC05_tDiscardPkt() {    
    btHC05_ResetInputsArr();    
    HC05_tInputPkt_Idx = 0;     
       
    btHC05_ResetDataInputTimer();
        
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL
            btHC05_DebugPrint((char*)"HC05_tDiscardPkt()");
        #endif
    #endif    
}

static void HC05_tChkPktNPub() {
    // chks if sent bytes are correct (CRC)
    // nb: this is NOT implemented yet
    
    // does not chk logic of cmds sent
    // does not chk valid ranges of each var, since cmd resolver already does that no need to duplicate work
    // if Ok, publish the vars and sets the time of last pub update    

    for(uint_fast8_t i = 0; i<btHC05_Inputs_Len; ++i) {
        btHC05_PubVars[i] = btHC05_Inputs[i];        
    }
    
    btHC05_PubVar_TimeOfLastUpdate = millis();
    
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL
            btHC05_DebugPrint((char*)"tChkPktNPub()");
        #endif
    #endif
}

static void btHC05_RunFSM() {
    char strStName[btHC05_DebugArrLen];
    
    btHC05_State_Prev = btHC05_State;    
    
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL
            //Serial.println();    
            Serial.print("Current State         --> ");    
            btHC05_GetStName(btHC05_State_Prev, strStName); 
            Serial.println(strStName);                                            
        #endif
    #endif
    
    switch (btHC05_Event) {
        case btHC05_evNone:     // no Event
                                // currently no states will respond to this event.
            
            #ifdef DEBUG_BTHC05_ON
                #ifdef SERIAL_MONITOR_AVAIL
                    btHC05_DebugPrint((char*)"No Translation Fn called");
                #endif
            #endif            
            break;

        case btHC05_evStartMarkRec:             // Start Marker Received
            if (btHC05_State == btHC05_stIdle) {
                HC05_tStartPkt();      btHC05_State = btHC05_stRiP;                
                
            } else if (btHC05_State == btHC05_stRiP) {
                HC05_tDiscardPkt();    btHC05_State = btHC05_stRecErr;                
                
            } else if (btHC05_State == btHC05_stRecErr) {
                HC05_tStartPkt();    btHC05_State = btHC05_stRiP;                
                
            }
            
            break;

        case btHC05_evHasData:              // data received that is NOT the '<' or '>'
            if (btHC05_State == btHC05_stRiP) {
                
                if(HC05_tInputPkt()) { btHC05_State = btHC05_stRiP;} 
                else {
                    // Usr Input to Input Array failed - OOB
                    HC05_tDiscardPkt(); btHC05_State = btHC05_stRecErr;    
                }                
            }                
            break;

        case btHC05_evEndMarkRec:           // End marker received
            if (btHC05_State == btHC05_stRiP) {
                HC05_tChkPktNPub(); btHC05_State = btHC05_stIdle;
            } 
            break;

        case btHC05_evTimeOut:
            if (btHC05_State == btHC05_stRiP) {
                // Receive-in-Progress but it is too long since last data received, hence, we assume error
                HC05_tDiscardPkt(); btHC05_State = btHC05_stIdle;
            }            
            break;
            
        default:            
            break;
    }

    // after all translation fns are run
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL
            Serial.print("Next State            --> ");    
            btHC05_GetStName(btHC05_State, strStName); 
            Serial.println(strStName);                                
            Serial.println();
        #endif
    #endif
}

uint8_t btHC05_Iterate() {
    // public fn
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL
            Serial.println();
            Serial.println("Start Cycle ==========");
            Serial.println("======================");
        #endif    
    #endif
    btHC05_GetInput();
    btHC05_RunFSM();
    #ifdef DEBUG_BTHC05_ON
        #ifdef SERIAL_MONITOR_AVAIL
            btHC05_DebugPrint_PublishedVars();
            Serial.println("End Cycle ..........");
            Serial.println("....................");
        #endif
    #endif             
}




