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
#endif

#include <iostream>
#include <cmath>
#include <math.h>

class Wire {
  public:
    static void begin();
    void beginTransmission(uint8_t);
    void write(uint8_t);
    uint8_t endTransmission(void);
};

void Wire::begin() {
  
}

void Wire::write(uint8_t value) {
  
}

void Wire::beginTransmission(uint8_t val) {
  
}

uint8_t Wire::endTransmission() {
  return 0;
}

Wire Wire;

class MockSerial {
  public:
    void print(const char* value);
    void print(uint8_t value);
    void println(uint8_t value);
    void println(const char* value);
};



void MockSerial::print(const char* value) {
  std::cout << value;
}

void MockSerial::println(const char* value) {
  std::cout << value << std::endl;
}

void MockSerial::print(uint8_t value) {
  std::cout << std::to_string(value);
}

void MockSerial::println(uint8_t value) {
  std::cout << std::to_string(value) << std::endl;
}



MockSerial Serial;

NI2C::NI2C(const uint8_t numberOfModules, const uint8_t startAddress, bool debug)
{
  _numberOfModules = numberOfModules;
  _debug = debug;
  _startAddress = startAddress;
  _error = NI2C_OK;
}

void NI2C::debug(const bool debug = true) {
  _debug = debug;
}

void NI2C::begin()
{
  Wire.begin();
}

void NI2C::write(const char* value)
{
  //TODO check if value length is uneven and append " " to make it even if it is...
  char* stringValue;
  strcpy(stringValue, value);
  
        //2 chars per module
  uint8_t numOfChars = strlen(stringValue);
  
  if(numOfChars % 2 != 0) {
    stringValue[numOfChars] = " ";
    Serial.println("Appended one extra character to make string even");
  }
  
  if(numOfChars > _numberOfModules * 2) {
    _error = NI2C_MAX_MODULE_ERROR;
    Serial.println("ERROR - String longer than max allowed number of characters based on module number");
    return;
  }

        for(uint8_t i = 0; i < numOfChars; i = i+2 ) {          
          uint8_t module = getModuleForCharPosition(i);
          uint8_t address = _startAddress + module;
          
          uint8_t outValue = 0;
          
          for(uint8_t j = i; j < i + 2; j++) {
              char c = value[j];
              uint8_t intValue = 0;
            
              if(isdigit(c)) {
                //Trick to map ASCII characters to integer value
                intValue = c - '0';
              } else {
                intValue = mapCharToValue(c);
              }              

              //even = first nixie, uneven = second nixie
              if(j % 2 == 0) {
                  outValue = (outValue & 0x0F) | (intValue << 4);
              } else {
                  outValue = (outValue & 0xF0) | intValue;       
              }
          }
                    
          Serial.print("Module");
          Serial.println(module);
          
          Serial.print("Out Value");
          
          char outBuffer[4];
          sprintf(outBuffer, "0x%02X", outValue);
          Serial.println(outBuffer);
          
          uint8_t result = write8(address, outValue);
          if(result > 0) {
            _error = result;
            break; //break for loop - don't write more!
          }
          

          
        }
}

void NI2C::off() {
       
}

uint8_t NI2C::write8(const uint8_t address, const uint8_t value)
{        
        if(_debug) {
          char outBuffer[4];
          sprintf(outBuffer, "0x%02X", address);
          Serial.print("Writing to i2c [");
          Serial.print(outBuffer);
          Serial.print("] -> value ");        
          sprintf(outBuffer, "0x%02X", value);
          Serial.println(outBuffer);          
        }


        /*
        note we only want to write "full" bytes to not make the tubes "flicker"
        because one part of the byte has "old" values from the previous write
        */
        uint8_t result = NI2C_OK;
        Wire.beginTransmission(address);
          Wire.write(value);
          result =  Wire.endTransmission();
          if(_debug) {
                  Serial.print("Transmission status: ");
                  Serial.println(result);
          }
        
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

        switch(c) {
        case 'n':
                result = 0;
                break;
        case '+':
                result = 1;
                break;
        case 'm':
                result = 2;
                break;
        case 'A':
                result = 3;
                break;
        case 'V':
                result = 4;
                break;
        case 'M':
                result = 5;
                break;
        case 'o':
                result = 6;
                break;
        case 'i':
                result = 7;
                break;
        case 'K':
                result = 8;
                break;
        case '-':
                result = 9;
                break;
        case ' ':
                result = NIXIE_TUBE_OFF;
                break;
        default:
                if(_debug) {
                        Serial.print("Unknown character: ");
                        Serial.println(c);
                }
                break;
        }

        if(_debug) {
                Serial.print("Mapped character [");
                Serial.print(c);
                Serial.print("] -> (");
                Serial.print(result);
                Serial.println(")");
        }

        return result;
}

int NI2C::lastError()
{
        int e = _error;
        if(_debug) {
                Serial.print("i2c error: ");
                switch(e) {
                case 0:
                        Serial.println("success");
                        break;
                case 1:
                        Serial.println("data too long to fit in transmit buffer");
                        break;
                case 2:
                        Serial.println("received NACK on transmit of address");
                        break;
                case 3:
                        Serial.println("received NACK on transmit of data");
                        break;
                case 4:
                        Serial.println("other error");
                        break;
                case NI2C_MAX_MODULE_ERROR:
                        Serial.println("maximum number of modules exeeded");
                        break;
                }
        }
        _error = NI2C_OK;
        return e;
}
