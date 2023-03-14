#include <iostream>
#include <stdexcept>

#include "linker.h"

using namespace std;

int main(int argc, char* argv[]) {
	Linker* linker;
	int n = 0;
	try {
		n = Linker::checkArgsLength(argc, argv);
		linker = new Linker(argv, argc, false);
		if(n == -2) linker = new Linker(argv, argc, false);
		if(n == argc) linker = new Linker(argv,argc, true);
	}
	catch (invalid_argument& e) {
		cout << e.what() << endl;
		linker = new Linker(argv, argc, false); // ne koristi se samo da se ne buni
	}
	if (n != -1) {
		linker->start();
	}
}
