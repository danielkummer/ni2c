//
//    FILE: NI2C.cpp
//  AUTHOR: Daniel Kummer
//    DATE: 09-nov-2017
// VERSION: 0.1.0
// PURPOSE: I2C nixie module library for arduino
//     URL: [TODO]
//
// HISTORY:
// 0.1.0 initial version
//

#include "NI2C.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include <Wire.h>
#else

#include <iostream>
#include <cmath>
#include <math.h>

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

void NI2C::log(const char * message) {
#if defined(ARDUINO)
        Serial.println(message);
#else
        std::cout << message << std::endl;
#endif
}

void NI2C::begin()
{
#if defined(ARDUINO)
        Wire.begin();
#endif
}

uint8_t NI2C::write(const char* value, uint8_t module) {
  uint8_t address = _startAddress + module;

  uint8_t outValue = 0;

  for(uint8_t tupelIdx = 0; tupelIdx < 2; tupelIdx++) {
          char c = value[tupelIdx];
          uint8_t intValue = 0;

          intValue = isdigit(c) ? c - '0' : mapCharToValue(c);
          outValue = tupelIdx % 2 == 0 ? (outValue & 0x0F) | (intValue << 4) : (outValue & 0xF0) | intValue;
  }

  return write8(address, outValue);
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
        for(uint8_t i = 0; i < lastCharacterIndex; i = i+2 ) {
                uint8_t module = getModuleForCharPosition(i);
                uint8_t outValue = 0;

                char tupel[] = {value[i], value[i+1]};

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

uint8_t NI2C::write8(const uint8_t address, const uint8_t value)
{
        if(_debug) {
                char outBuffer[32];
                sprintf(outBuffer, "DEBUG - I2c: 0x%02x - value: 0x%02x", address, value);
                log(outBuffer);
        }

        uint8_t result = NI2C_OK;
#if defined(ARDUINO)
        Wire.beginTransmission(address);
        Wire.write(value);
        result =  Wire.endTransmission();

        if(_debug) {
                char outBuffer[24];
                sprintf(outBuffer, "DEBUG - I2c result: 0x%02X", result);
                log(result);
        }
#endif
        return result;

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
                char outBuffer[36];
                sprintf(outBuffer, "DEBUG - Character mapping [%c]->[%d]", c,result);
                log(outBuffer);
        }

        return result;
}

int NI2C::lastError()
{
        const char* returnMessages[] = { "SUCCESS", "data too long to fit transmit buffer", "received NACK on transmit of address", "received NACK on transmit of data", "other error" };
        int e = _error;
        if(_debug) {
                char outBuffer[58];
                sprintf(outBuffer, "DEBUG - I2c error: %s", returnMessages[e]);
                log(outBuffer);
        }
        _error = NI2C_OK;
        return e;
}
