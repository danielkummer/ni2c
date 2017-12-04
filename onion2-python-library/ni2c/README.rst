 NI2C - Universal nixie driver
--------

For use with the Omega2, you need to supply the i2c class
in the constructor in order for it to work...

To use , simply do::

    >>> import ni2c
    >>> nixie ni2c.Ni2c(onionI2C.OnionI2C())
    >>> nixie.write("1234")


