#include "NI2C.h"
#include <Wire.h>

//we're using 1 module to display a simple second counter
NI2C nixie(1);

void setup() {
  Serial.begin(115200);
  
  // put your setup code here, to run once:
  nixie.begin();
}

void loop() {    
    int seconds = 0;
  
    char outBuffer[9];
    sprintf(outBuffer, "%02d", seconds);
    
    nixie.write(outBuffer);
    
    seconds++;

  
    if(seconds == 99) {
      seconds = 0;
    }

   delay(1000);
}
