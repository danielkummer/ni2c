from unittest import TestCase

import ni2c


class MockOnionI2C:

    def __init__(self):
        pass

    def writeByte(self, address, register, value):
        print "MOCK i2c: address [0x%02x] register [0x%02x] value [0x%02x]" % (address, register, value)
        pass

    def setVerbosity(self, value):
        pass


class TestNi2c(TestCase):

    def setUp(self):
        self.i2c_start_address = 0x38
        self.nixie = ni2c.Ni2c(MockOnionI2C(), 1, self.i2c_start_address)


    def test_get_i2c_address_should_return_correct_address(self):
        for i in range(0, 9):
            self.assertEqual(self.nixie._get_i2c_address(i), self.i2c_start_address + (i * 2))

    def test_write(self):
        result = self.nixie.write("12")
        self.assertTrue(result)

    def test_something(self):
        pass