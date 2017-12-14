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

    def test_write_should_succeed(self):
        result = self.nixie.write(" 1")
        self.assertEqual(result, 0x00)

    def test_get_i2c_value(self):
        for i in range(0, 10):
            self.assertEqual(self.nixie._get_i2c_value("%d" % (i)), i)
        for idx, i in enumerate(['n', '+', 'm', 'A', 'V', 'M', 'o', 'i', 'K', '-']):
            self.assertEqual(self.nixie._get_i2c_value(i), idx)

        self.assertEqual(self.nixie._get_i2c_value(" "), 0xf)
