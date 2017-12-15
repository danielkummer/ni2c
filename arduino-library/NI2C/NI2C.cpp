//
//    FILE: NI2C.cpp
//  AUTHOR: Daniel Kummer
// PURPOSE: I2C nixie module library for arduino
//     URL: https://danielkummer.github.io/ni2c/

#include "NI2C.h"

#include <stdarg.h>
#include <stdio.h>

#if defined(ARDUINO) && ARDUINO >= 100
#include <Wire.h>
#endif

NI2C::NI2C(const uint8_t numberOfModules, const uint8_t startAddress)
{
        _numberOfModules = numberOfModules;
        _maxAllowedCharacters = _numberOfModules * 2;
        _debug = false;
        _startAddress = startAddress;
        _error = NI2C_OK;
        _lookup = "n+mAVMoiK-";
}

void NI2C::debug(bool debug = true) {
        _debug = debug;
}

void NI2C::log(const char* format, ...) {
  char outBuffer[60];
  va_list(args);
  va_start(args, format);
  vsprintf(outBuffer, format, args);
  va_end(args); 
  
  #if defined(ARDUINO)
    Serial.println(outBuffer);
  #else
    std::cout << outBuffer << std::endl;
  #endif 
}


void NI2C::begin()
{
#if defined(ARDUINO)
        Wire.begin();
#endif
}

void NI2C::scan() {
#if defined(ARDUINO)
  uint8_t foundDevicesCount = 0;
  uint8_t error = 0;
  for(uint8_t address; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      log("DEBUG - I2c found at: 0x%02x", address);
      foundDevicesCount++;
    }
  }
  if (foundDevicesCount == 0) {
    log("No I2C devices found");
  } else {
    log("done");    
  }
#else
  log("Not implemented in non-arduino");
#endif
}

uint8_t NI2C::write(const char* value, uint8_t module) {
  uint8_t address = _startAddress + module;
  uint8_t outValue = 0;
  uint8_t result = NI2C_OK;

  for(uint8_t tupelIdx = 0; tupelIdx < 2; tupelIdx++) {
          char c = value[tupelIdx];
          uint8_t intValue = 0;

          intValue = isdigit(c) ? c - '0' : mapCharToValue(c);
          //switch outvalue byte order
          outValue = tupelIdx % 2 == 0 ? (outValue & 0xF0) | intValue : (outValue & 0x0F) | (intValue << 4);
  }
  
  if(_debug) {
    log("DEBUG - I2c: 0x%02x - value: 0x%02x", address, outValue);
  }

#if defined(ARDUINO)
  Wire.beginTransmission(address);
  Wire.write(value);
  result =  Wire.endTransmission();

  if(_debug) {
    log("DEBUG - I2c result: 0x%02X", result);
  }
#endif
  return result;
}

uint8_t NI2C::write(const char* value)
{
        uint8_t numOfChars = strlen(value);
        bool isEqual = numOfChars % 2 == 0;
        char stringValue[isEqual ? numOfChars : numOfChars + 1];

        strcat(stringValue, value);

        if(!isEqual) {
                strcat(stringValue, " ");
                if(_debug) {
                        log("INFO - Appended extra character for even output string");
                }

        }

        uint8_t result = NI2C_OK;

        uint8_t lastCharacterIndex = (numOfChars > _maxAllowedCharacters) ? _maxAllowedCharacters : numOfChars;
        
        for(uint8_t i = 0; i < lastCharacterIndex; i = i + 2) {          
          uint8_t module = getModuleForCharPosition(i);
          uint8_t outValue = 0;
          
          char tupel[2];
          strncpy(tupel, value + i, 2);
          
          result = write(tupel, module);
          if(result > 0) {
            _error = result;
            break; //break for loop - don't write more!
          }          
        }
        return result;
}

void NI2C::off() {
  for(uint8_t i = 0; i < _numberOfModules ; i++) {
      write("  ", i);
  }
}

uint8_t NI2C::getModuleForCharPosition(uint8_t charPosition)
{
        long in_min = 0.0;
        long in_max = MAX_MODULES * 2.0;
        long out_min = 0.0;
        long out_max = MAX_MODULES * 1.0;

        return round((charPosition - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

uint8_t NI2C::mapCharToValue(char c)
{
        //map all non numeric characters to values which can be represented (or set display to off)
        uint8_t result = NIXIE_TUBE_OFF;

        for(uint8_t i = 0; i < strlen(_lookup); i++) {
          if(_lookup[i] == c) {
            result = i;
            break;
          }
        }

        if(_debug) {
          log("DEBUG - Character mapping [%c]->[%d]", c,result);          
        }

        return result;
}

int NI2C::lastError()
{
        const char* returnMessages[] = { "SUCCESS", "data too long to fit transmit buffer", "received NACK on transmit of address", "received NACK on transmit of data", "other error" };
        int e = _error;
        if(_debug) {
                log("DEBUG - I2c error: %s", returnMessages[e]);
        }
        _error = NI2C_OK;
        return e;
}
