#include "NI2C.h"
#include <Wire.h>

//we're using 4 module to do a simple time display...
NI2C nixie(4);

void setup() {
  Serial.begin(115200);
  
  // put your setup code here, to run once:
  nixie.begin();
}

void loop() {
    int hours = 0;
    int minutes = 0;
    int seconds = 0;
  
    char outBuffer[9];
    sprintf(outBuffer, "%02d+%02d-%02d~", hours, minutes, seconds);
    
    nixie.write(outBuffer);
    
    seconds++;
    if(seconds > 59) {
      minutes++;
      seconds = 0;
    }

    if(minutes > 59) { 
      hours++;
      minutes = 0;
      seconds = 0;
    }	
   
   delay(1000);
}
