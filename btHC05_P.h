#ifndef BTHC05_P_H
#define BTHC05_P_H

/*
    btHC05_P.h - Published Vars and Public fns for btHC05 module
*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                  PUBLISHED VARS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

uint8_t  btHC05_PubVars[btHC05_Inputs_Len] = {255, 255, 255, 255, 255, 255}; 
uint32_t btHC05_PubVar_TimeOfLastUpdate = 0;    // in ms


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//                  PUBLIC FUNCTIONS
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void    btHC05_Init();
uint8_t btHC05_Iterate();


/*
    Misc notes:
    tested on 3 March 2018, 1455hrs
    ===============================

    Test                                        Results
    ~~~~                                        ~~~~~~~
    (1)  ... normal tx two sets of data         ==> 2 sets of data rec (PASS)
    (2)  ... set 1 oob, set#2 normal data       ==> oob goes stErr, set#2 publishes correctly (PASS)
    (3)  ... '<' followed immediately by '>'    ==> pkt is published with all zeros (InputArr inited as 0). 
                                                (PASS)
    (4)  ... '<', then No data                  ==> timeout / pkt is discarded (PASS)
    (5)  ... '<', data arrives, NO'>'           ==> timeout / pkt is discarded (PASS)
    (6)  ... '<' data arrives, then '<' again   ==> stRiP does not respond discards and st_RecErr where everything is ignored until the next '<' is received. (PASS)
    (7)  ... '>' then data '>'                  ==> every state ignores except for stRiP which closes packet (if open) else it also ignores (PASS)
    (8)  ... '>' then '>'                       ==> stRiP closes pkt, state becomes stIDLE, stIDLE does not respond to '>' (PASS)       

    sequence
    ~~~~~~~~
    (9)  ... '<' then deliberate overflow buffer    ==> OOB, discard packet     (PASS)
        followed by '<' normal package '>'    ==> normal input and publish

*/

#endif // BTHC05_P_H
