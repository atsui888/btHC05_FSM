#include <stdint.h>
#include "btHC05_I.h"
#include "btHC05_P.h"

// Debug switches, comment out if not in use
#define SERIAL_MONITOR_AVAIL
#define DEBUG_BTHC05_ON


void setup() {
    #ifdef SERIAL_MONITOR_AVAIL
        Serial.begin(9600);
   #endif

    btHC05_Init();

}


void loop() {
    btHC05_Iterate();
    #ifdef DEBUG_BTHC05_ON
        delay(1000);
    #endif
}
