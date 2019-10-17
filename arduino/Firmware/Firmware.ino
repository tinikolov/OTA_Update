#include "UpdateFirmware.h"

#define BAUD_RATE 19200

void setup(){
   
  Serial.begin(BAUD_RATE);
  SetupFirmwareUpdate(); 
}

void loop() { // run over and over
     FirmwareUpdate();
     delay(100);  
}


