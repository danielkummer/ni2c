//
//    FILE: NI2C.h
//  AUTHOR: Daniel Kummer
//    DATE: 09-nov-2017
// VERSION: 0.1.0
// PURPOSE: I2C nixie module library for arduino
//     URL: [TODO]
//
// HISTORY:
// see NI2C.cpp file
//

#ifndef _NI2C_H
#define _NI2C_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include <stdint.h>
#include <string.h>
#include <iostream>
#include <cmath>
#include <math.h>
#endif

#define NI2C_OK         0x00
#define MAX_MODULES     8
#define NIXIE_TUBE_OFF  0xFF

class NI2C {
public:

/**
 * Constructor.
 *
 * @param numberOfModules Define the number of modules in use.
 *                        The maximum number of supported modules is 8
 *                        if you're using the PCF8574 I/O expander. default: 1
 * @param startAddress The first module address, default: 0x38
 * @param debug Enable debug mode, default: false
 *
 */
explicit NI2C(const uint8_t numberOfModules = 1, const uint8_t startAddress = 0x38);

/**
 * Begin wire transmission
 */
void begin();

/**
 * Scan the i2c bus for reachable addresses.
 *
 */
void scan();

/**
 * Get the last i2c error and reset the error code to success.
 */
int lastError();

/**
 * Write a string to the i2c bus.
 * @param value the string to write
 *
 * Note: the string can only be as long as the total number of nixie tubes in use.
 * One module supports two nixie tubes.
 *
 * Apart from the numbers 0-9, the implementation also supports mapping of some characters:
 * ' ' (space) will turn off a tube.
 *
 * The following character mappins are supported if a IN-7 tube is in use:
 * 'n' '+' 'm' 'A' 'V' 'M' 'o' 'i' 'K' '-'
 * The characters are mapped to the numbers 0-9 (in this order)
 *
 * @return i2c bus result code
 */
uint8_t write(const char* value);

/**
 * Write a tupel string to a specific module
 * 
 * @param value the string to write, must be length 2
 * @param module the module to write to, zero based
 * 
 * @return i2c bus result code
 */
uint8_t write(const char* value, uint8_t module);

/**
 * Turn off all nixie tubes
 */
void off();

/**
 * Enable or disable debug mode (console output)
 *
 * @param debug debug enabled or disabled
 */
void debug(const bool debug);


private:

/**
 * Get the corresponding module for a given character position.
 * One module supports two characters.
 *
 * @param
 */
uint8_t getModuleForCharPosition(uint8_t charPosition);

/**
 * Map a given character to a number from 0-9
 * Used by the write function, the method maps non-numeric characters
 * to a usable numeric representation.
 *
 * @param c the character to map
 *
 * @return the mapped value
 */
uint8_t mapCharToValue(char c);

/**
 * Log a formatted message
 * @param messageFormat the message format string
 * @param ... format parameters
 */
void log(const char* messageFormat, ...);

bool _debug;
uint8_t _numberOfModules;
uint8_t _maxAllowedCharacters;
uint8_t _startAddress;

const char* _lookup;

int _error;
};

#endif
