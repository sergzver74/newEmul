#include <iostream>
#include "z80.h"


using namespace std;

void z80::reset() {
	cout << "THIS Z80 processor" << endl;
}

Registers z80::getRegisters() {
	Registers reg;



	return reg;
}

std::string z80::disAsm(uint16_t* addr, uint8_t n) {
	std::string res = "";

	return res;
}