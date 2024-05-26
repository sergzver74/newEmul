#include "breakpoints.h"


Breakpoints::Breakpoints() {
	bpmap = new uint8_t[65536];
	for (int i = 0; i < 65536; i++) bpmap[i] = 0;
}

Breakpoints::~Breakpoints() {
	delete[] bpmap;
}

void Breakpoints::addAddr(uint16_t addr, bool isOnce) {
	if (isOnce) bpmap[addr] = 0x81; else bpmap[addr] = 0x01;
}

void Breakpoints::addAddr(std::string addr, bool isOnce) {
	uint16_t stopAddr = hexToDec(addr);
	if (isOnce) bpmap[stopAddr] = 0x81; else bpmap[stopAddr] = 0x01;
}

void Breakpoints::deleteAddr(uint16_t addr) {
	bpmap[addr] = 0;
}

bool Breakpoints::checkAddr(uint16_t addr) {
	if (bpmap[addr]) {
		if (bpmap[addr] > 0x80) bpmap[addr] = 0;
		return true;
	}
	else return false;
}