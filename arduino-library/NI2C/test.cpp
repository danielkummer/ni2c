#define TEST_NON_ARDUINO 1

#include <iostream>



#include "NI2C.h"


using namespace std;

NI2C nix(2);

int main(int argc, char *argv[]) {
	
	nix.begin();
	nix.write("o1  1");
	
	std::cout << "Hello world!" << std::endl;
}

