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

#include <Wire.h>

NI2C::NI2C(const uint8_t numberOfModules = 1, const uint8_t startAddress = 0x38, bool debug = false)
{
        _numberOfModules = numberOfModules;
        _debug = debug;
        _startAddress = startAddress;
        _error = NI2C_OK;
        _dataOut = 0xFF;
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
        //2 chars per module
        uint8_t numOfChars = strlen(value);
        if(numOfChars > _numberOfModules * 2) {
                _error = NI2C_MAX_MODULE_ERROR;
                return;
        }

        for(uint8_t i = 0; i < numOfChars; i++) {
                uint8_t module = getModuleForCharPosition(i);
                byte address = getI2cAddress(module);

                char c = value[i];

                int value = 0;
                if(isdigit(c)) {
                        //Trick to map ASCII characters to integer value
                        value = c - '0';
                } else {
                        value = mapCharToValue(c);
                }


                /*
                hack - we only write "full bytes" - e.g if the 3rd param is >= 1
                that's why for the last character if it's uneven we write directly
                */
                uint8_t nixieIndex = i % 2;
                //is last character and uneven
                if (i == numOfChars - 1 && nixieIndex == 1) {
                  nixieIndex = 2;
                }

                byte result = write8(address, value, nixieIndex);
                if(result > 0) {
                        _error = result;
                        break; //break for loop - don't write more!
                }
        }
}

void NI2C::off() {
        for(uint8_t i = 1; i < _numberOfModules; i++) {
                write8(getI2cAddress(i), 0xff, 2);
        }
}

uint8_t NI2C::write8(const uint8_t address, const uint8_t value, const uint8_t nixieIndex)
{
        switch(nixieIndex) {
        case 0:
                _dataOut = _dataOut & 0x0F | (value << 4);
                break;
        case 1:
                _dataOut = _dataOut & 0xF0 | value;
                break;
        default:
                _dataOut = value;
                break;
        }

        if(_debug) {
                Serial.print("i2c address: ");
                Serial.print(address);
                Serial.print(" value: ");
                Serial.println(_dataOut);
        }


        /*
        note we only want to write "full" bytes to not make the tubes "flicker"
        because one part of the byte has "old" values from the previous write
        */
        uint8_t result = NI2C_OK;
        if(nixieIndex > 0) {
          Wire.beginTransmission(address);
          Wire.write(_dataOut);
          result =  Wire.endTransmission();
          if(_debug) {
                  Serial.print("Transmission status: ");
                  Serial.println(result);
          }
        }
        return result;

}

/**
 * Returns addresses 0x70, 0x72, 0x74, ... 0x7E
 */
uint8_t NI2C::getI2cAddress(uint8_t moduleNumber)
{
        uint8_t result = _startAddress + (moduleNumber * 2);
        if(_debug) {
                char outBuffer[4];
                sprintf(outBuffer, "%02X", result);
                Serial.print("I2C address [");
                Serial.print(outBuffer);
                Serial.print("] -> module (");
                Serial.print(moduleNumber);
                Serial.println(")");
        }
        return result;
}

uint8_t NI2C::getModuleForCharPosition(uint8_t charPosition)
{
        long in_min = 0.0;
        long in_max = MAX_MODULES * 2.0;
        long out_min = 1.0;
        long out_max = MAX_MODULES * 1.0;

        uint8_t result = round((charPosition - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);

        if(_debug) {
                Serial.print("Character position [");
                Serial.print(charPosition);
                Serial.print("] -> module (");
                Serial.print(result);
                Serial.println(")");
        }
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
