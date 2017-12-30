#include <iostream>
#include "../NI2C.h"

using namespace std;

NI2C nix(8);



void mockClock() {
// 12A
// 34-
// 56+
// 12-34-56-

	int hours = 0;
	int minutes = 0;
	int seconds = 0;
	
	char outBuffer[9];
	
	while (hours <= 12) {
		sprintf(outBuffer, "%02d+%02d-%02d~", hours, minutes, seconds);	
    std::cout << outBuffer << std::endl;
		nix.write(outBuffer);	
		seconds++;
		if(seconds > 59) {
			minutes++;
			seconds = 0;
		}

		if(minutes > 59) { 
			hours++;
			minutes = 0;
			seconds = 0;
		}	
	}
}


int main(int argc, char *argv[]) {
	nix.debug(false);
	nix.begin();
	nix.scan();
	nix.write("12345678A +-o~MV");
	nix.write("12", 3);
	nix.write("AA", 3);
	mockClock();
	nix.off();
}

