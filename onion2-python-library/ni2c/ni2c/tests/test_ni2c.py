from unittest import TestCase

import ni2c


class MockOnionI2C:

    def __init__(self):
        pass

    def writeByte(self, address, register, value):
        print "MOCK i2c: address [0x%02x] register [0x%02x] value [0x%02x]" % (address, register, value)
        return 0x00

    def setVerbosity(self, value):
        pass


class TestNi2c(TestCase):

    def setUp(self):
        self.i2c_start_address = 0x38
        self.nixie = ni2c.Ni2c(MockOnionI2C(), 1, self.i2c_start_address)

    def test_write_should_throw_exception_when_string_exeeds_modules(self):
        with self.assertRaises(ValueError):
            self.nixie.write("1234")

    def test_get_i2c_address_should_return_correct_address(self):
        for i in range(1, 9):
            self.assertEqual(self.nixie._get_i2c_address(i), self.i2c_start_address + ((i-1) * 2))

    def test_get_module_for_char_position_should_return_module_number_in_pairs_of_two(self):
        self.assertEqual(self.nixie._get_module_for_char_position(0), 1)
        self.assertEqual(self.nixie._get_module_for_char_position(1), 1)
        self.assertEqual(self.nixie._get_module_for_char_position(2), 2)
        self.assertEqual(self.nixie._get_module_for_char_position(3), 2)

    def test_write(self):
        result = self.nixie.write("12")
        self.assertEqual(result, 0x00)

    def test_get_i2c_value(self):
        for i in range(0, 10):
            self.assertEqual(self.nixie._get_i2c_value("%d" % (i)), i)
        for idx, i in enumerate(['n', '+','m','A','V','M','o','i','K','-']):
            self.assertEqual(self.nixie._get_i2c_value(i), idx)

        self.assertEqual(self.nixie._get_i2c_value(" "), 0xf)

    def test_get_nixie_index(self):
        self.assertEqual(self.nixie._get_nixie_index(0, 2), 0)
        self.assertEqual(self.nixie._get_nixie_index(1, 2), 0xff)
        self.assertEqual(self.nixie._get_nixie_index(2, 4), 0)
        self.assertEqual(self.nixie._get_nixie_index(3, 4), 0xff)
        self.assertEqual(self.nixie._get_nixie_index(3, 5), 1)
        self.assertEqual(self.nixie._get_nixie_index(4, 5), 0)