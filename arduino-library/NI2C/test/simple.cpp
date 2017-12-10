#include <iostream>
#include "../NI2C.h"

using namespace std;

NI2C nix(8);

int main(int argc, char *argv[]) {
	nix.debug(true);
	nix.begin();
	nix.write("12345678A +-oiMV");
	nix.write("12", 3);
	nix.write("AA", 3);
}
