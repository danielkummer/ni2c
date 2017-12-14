import logging
import re


class Ni2c:
    MAX_MODULES = 8
    NI2C_OK = 0

    CHAR_MAPPING = 'n+mAVMoiK-'

    def __init__(self, i2c, number_of_modules=1, start_address=0x38):
        self.debug = False
        self.number_of_modules = number_of_modules
        self.start_address = start_address
        self.error = self.NI2C_OK
        self.i2c = i2c

    def write_2_module(self, module, tupel):
        address = self.start_address + module

        out_value = 0xff

        for idx, character in enumerate(tupel):
            value = self._get_i2c_value(character)
            out_value = (out_value & 0xF0) | value if idx % 2 == 0 else (out_value & 0x0F) | (value << 4)

        return self.i2c.writeByte(address, 0x00, out_value)

    def write(self, string):
        num_of_chars = len(string)
        is_equal = num_of_chars % 2 == 0

        result = self.NI2C_OK

        if not is_equal:
            string += ' '
            logging.debug('Appended extra character to create string of equal length')

        for module, tupel in enumerate(re.findall('..', string)):
            if module < self.number_of_modules:
                result = self.write_2_module(module, tupel)
                if result > 0:
                    self.error = result
                    break
            else:
                logging.debug('Exceeded max number of allowed characters based on number of modules')
                return result

        return result

    def _get_i2c_value(self, character):
        value = int(character) if character.isdigit() else self.CHAR_MAPPING.find(character)
        return value if value >= 0 else 0xF

    def off(self):
        for i in xrange(0, self.number_of_modules):
            self.write_2_module(i, "  ")

    def last_error(self):
        e = self.error

        if self.debug:
            result = {
                0: "success",
                1: "data too long to fit in transmit buffer",
                2: "received NACK on transmit of address",
                3: "received NACK on transmit of data",
                4: "other error"
            }.get(e, "Unknown error")
            print result

        self.error = self.NI2C_OK
        return e
