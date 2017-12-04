from setuptools import setup


def readme():
    with open('README.rst') as f:
        return f.read()

setup(name='ni2c',
      version='0.1',
      description='Universal nixie i2c module driver for the Omega2 platform',
      url='https://github.com/danielkummer/ni2c',
      author='Daniel Kummer',
      author_email='daniel.kummer@gmail.com',
      license='CC BY-NC-SA 4.0',
      packages=['ni2c'],
      include_package_data=True,
      test_suite='nose.collector',
      tests_require=['nose'],
      zip_safe=False)
