

class Ni2c:
    MAX_MODULES = 8
    NI2C_OK = 0
    CHAR_MAPPING = {'n': 0, '+': 1, 'm': 2, 'A': 3, 'V': 4, 'M': 5, 'o': 6, 'i': 7, 'K': 8, '-': 9, ' ': 0xF}

    def __init__(self, i2c, number_of_modules=1, start_address=0x38):
        self.debug = False
        self.number_of_modules = number_of_modules
        self.start_address = start_address
        self.data_out = 0xFF
        self.error = self.NI2C_OK
        self.i2c = i2c

    def write(self, string):
        num_of_chars = len(string)
        if num_of_chars > self.number_of_modules * 2:
            raise ValueError('The string length exceeds the number of configured modules')

        for idx, c in enumerate(string):
            module = self._get_module_for_char_position(idx)
            address = self._get_i2c_address(module)

            if c.isdigit():
                value = int(c)
            else:
                value = self.CHAR_MAPPING[c]

            nixie_index = idx % 2

            if idx == num_of_chars - 1 and nixie_index == 1:
                nixie_index = 2

            result = self._write_i2c(address, value, nixie_index)
            if result > 0:
                self.error = result
                break

    def off(self):
        for i in xrange(0, self.MAX_MODULES + 1):
            self._write_i2c(self._get_i2c_address(i), 0xFF, 2)

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

    def _get_i2c_address(self, module_number):
        return self.start_address + ((module_number - 1) * 2)

    def _get_module_for_char_position(self, char_position):
        in_min = 0.0
        in_max = self.MAX_MODULES * 2.0
        out_min = 1.0
        out_max = self.MAX_MODULES * 1.0
        return int(round((char_position - in_min) * (out_max - out_min) / (in_max - in_min) + out_min))

    def _write_i2c(self, address, value, nixie_index):
        if nixie_index == 0:
            self.data_out = self.data_out & 0x0F | (value << 4)
        elif nixie_index == 1:
            self.data_out = self.data_out & 0xF0 | value
        else:
            self.data_out = value

        result = 0x00
        if nixie_index > 0:
            result = self.i2c.writeByte(address, 0x00, value)

        return result
