#include "kvaz.h"

kvaz::kvaz()
{
	port10 = 0;
	currentScreenPage = 0;
	currentStackPage = 0;
	quaziDiskStackMode = false;
	quaziDiskScreenModeAD = false;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 65536; j++) {
			mem[i][j] = 0;
		}
	}
}

kvaz::~kvaz()
{
	port10 = 0;
	currentScreenPage = 0;
	currentStackPage = 0;
	quaziDiskStackMode = false;
	quaziDiskScreenModeAD = false;
}

void kvaz::setPortData(uint16_t portNum, uint16_t data) {
	if (portNum == 0x10) {
		port10 = data;
		currentScreenPage = 4 - (data & 0x03);
		if (currentScreenPage == 4) currentScreenPage = 0;
		currentStackPage = 4 - ((data >> 2) & 0x03);
		if (currentStackPage == 4) currentStackPage = 0;
		if ((data >> 4) & 0x01) quaziDiskStackMode = true; else quaziDiskStackMode = false;
		if ((data >> 5) & 0x01) quaziDiskScreenModeAD = true; else quaziDiskScreenModeAD = false;
	}
}

uint16_t kvaz::getPortData(uint16_t portNum) {
	return port10;
}

tPorts kvaz::getPorts() {
	tPorts res;
	res.count = 1;
	res.ports[0] = 0x10;
	return res;
}

bool kvaz::getByte(uint16_t addr, uint8_t* data) {
	if (quaziDiskScreenModeAD) {
		if (addr >= 0xA000 && addr <= 0xDFFF) {
			*data = mem[currentScreenPage][addr];
			return true;
		}
	}
	return false;
}

bool kvaz::getWord(uint16_t addr, uint16_t* data) {
	if (quaziDiskScreenModeAD) {
		if (addr >= 0xA000 && addr <= 0xDFFF) {
			*data = mem[currentScreenPage][addr + 1];
			*data <<= 8;
			*data |= mem[currentScreenPage][addr];
			return true;
		}
	}
	return false;
}

bool kvaz::getWordFromStack(uint16_t addr, uint16_t* data) {
	if (quaziDiskStackMode) {
		*data = mem[currentStackPage][addr + 1];
		*data <<= 8;
		*data |= mem[currentStackPage][addr];
		return true;
	}
	return false;
}

bool kvaz::setByte(uint16_t addr, uint8_t data) {
	if (quaziDiskScreenModeAD) {
		if (addr >= 0xA000 && addr <= 0xDFFF) {
			mem[currentScreenPage][addr] = data;
			return true;
		}
	}
	return false;
}

bool kvaz::setWord(uint16_t addr, uint16_t data) {
	if (quaziDiskScreenModeAD) {
		if (addr >= 0xA000 && addr <= 0xDFFF) {
			mem[currentScreenPage][addr] = (uint8_t)(data & 0xFF);
			mem[currentScreenPage][addr + 1] = (uint8_t)(data >> 8);
			return true;
		}
	}
	return false;
}

bool kvaz::setWordToStack(uint16_t addr, uint16_t data) {
	if (quaziDiskStackMode) {
		mem[currentStackPage][addr] = (uint8_t)(data & 0xFF);
		mem[currentStackPage][addr + 1] = (uint8_t)(data >> 8);
		return true;
	}
	return false;
}

uint8_t* kvaz::getMemoryPointer() {
	uint8_t* m = &mem[0][0];
	return m;
}