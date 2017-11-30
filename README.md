# Nixie i2c Driver

This simple, custom nixie driver is indented to drive  _any_ kind of nixie tubes via i2c interface. It allows to address two  tubes with each i2c slave. The module is intended to be stackable and dynamically addressable using dip switches.

* Supports up to 8 stacked drivers
* Each driver module can power 2 nixie tubes

The driver module has been tested with the following nixie tubes:

* IN-4
* IN-7

## BOM - Bill of Materials

| 74141 | 2x | SN74141 or K155ID1 BCD decoder |
| PCF8574 | 1x | PCF8574 i2c I/O expander | |
| SW1/SW2 | 1x | DIP Switch 4 | |
| R1, R2 | 2x | 10kΩ Resistor | bus pull-up |
| R3, R4, R5, R8 | 4x | 10kΩ Resistor | |
| R6, R7 | 2x | 10kΩ 2W Resistor | |
| C2, C3, C4| 3x | 0.1µF Capacitor | optional |
| STACK PWR | 1x | pin-2 Male/female pin header | use male header with long pins to stack the module. Use a short pinned header for the bottom module |
| HV PWR | 1x | pin-2 Male/female pin header | use male header with long pins to stack the module. Use a short pinned header for the bottom module |
| PWR | 1x screw terminal | optional, power can also be supplied via STACK PWR |
| I2C | 1x | pin-2 Male/female pin header | i2c bus, 1:SDA 2:SCL |
| NX1, NX2 | 2x | pin-8 Male pin header | 90° angle for stackable module |
| ANODE | 1x | pin-6 Male pin header | 90° angle for stackable module |

## Schema

![description](/img/nixie_i2c_driver_stackable_schema.jpg)

## Assembly instructions

* The capacitors are optional as they're used as bypass

## Problem Solving

* If the tubes don't light up correctly you might have to change R6, R7. You can use [this calculator](http://www.csgnetwork.com/anoderescalc.html) to determine the resistor values based on the HV power supply


## I2c Interface Definition

| Byte | 7(MSB) | 6 | 5 | 4 | 3 | 2 | 1 | 0(LSB) |
| ---- | ------ | - | - | - | - | - | - | ------ |
| i2c slave address | L | H | H | H | A2 | A1 | A0 | R/W |
| I/O data bus | P7 | P6 | P5 | P4 | P3 | P2 | P1 | P0 |

### Address Reference

* Only the write addresses are shown as the nixie driver module doesn't support read operations.
* Arduino uses 7-bit addresses, the LSB (R/W) is _not_ set and the address range starts at 0x38

| A2 | A1 | A0 | i2c bus 8-bit slave address |
| -- | -- | -- | --------------------------- |
| L | L | L | 0x70 |
| L | L | H | 0x72 |
| L | H | L | 0x74 |
| L | H | H | 0x76 |
| H | L | L | 0x78 |
| H | L | H | 0x7A |
| H | H | L | 0x7C |
| H | H | H | 0x7E |

## BCD Decoder Reference

The two bcd decoders each use 4-bit of the 8-bit I/O expander.

The 74141 decoder has 4 inputs marked A,B,C,D which trigger corresponding outputs.

### Truth Table

| D | C | B | A | Output |
| - | - | - | - | ------ |
| L | L | L | L | 0 |
| L | L | L | H | 1 |  
| L | L | H | L | 2 |
| L | L | H | H | 3 |
| L | H | L | L | 4 |
| L | H | L | H | 5 |
| L | H | H | L | 6 |
| L | H | H | H | 7 |
| H | L | L | L | 8 |
| H | L | L | H | 9 |

All values above 0x9 are out of range and turn all outputs off.

* "on" range: 0x0..0x9
* "off" range: 0xA..0xF

## Using the Nixie i2c module

Based on the logic tables of the bcd decoders, writing to the i2c bus to output numbers is quite simple.

One byte defines the values for both decoders:

| Decoder | Bits |
| ------- | ---- |
| NIXIE 1 | 0000ABCD |
| NIXIE 2 | ABCD0000 |

### Data Examples

| Example value | Nixie Output |
| ------------- | ------------ |
| 0xFF | all off |
| 0x12 | "21" |
| 0x01 | "10" |
