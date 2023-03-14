#include <iostream>
#include "emulator.h"

int main(int argc, char* argv[]) {
	Emulator* emulator;
	int n = 0;

	try {
		n = Emulator::checkArgsLength(argc, argv);
		emulator = new Emulator(argv[1]);
	}
	catch (invalid_argument& e) {
		cout << e.what() << endl;
		emulator = new Emulator(argv[1]);
	}
	if (n == argc) emulator->start();

}
