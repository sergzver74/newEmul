#include "memory.h"

Memory::Memory() {
	for (int i=0;i<16;i++)
		for (int j=0;j<65536;j++) memory[i][j] = 0x00;
	for (int j = 0; j < 65536; j++) ROMMemory[j] = 0x00;
	ROMEnabled = false;
	ROMStartAddress = 0;
	maxBankCount = 1;
	currentBank = 0;
	maxMemorySize = 65536;
	ROMMemoryFileName = "";
	ROMSize = 0;
	kvazidisk = NULL;
}

Memory::Memory(uint32_t maxMainMemorySize, uint8_t maxBankNumber, uint16_t ROMStartAddr, string ROMFileName, kvaz* k) {
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 65536; j++) memory[i][j] = 0x00;
	for (int j = 0; j < 65536; j++) ROMMemory[j] = 0x00;
	ROMStartAddress = ROMStartAddr;
	maxBankCount = maxBankNumber;
	currentBank = 0;
	maxMemorySize = maxMainMemorySize;
	ROMMemoryFileName = ROMFileName;
	FILE* f;
	f = fopen(ROMMemoryFileName.c_str(), "rb");
	if (f)
	{
		ROMSize = fread(ROMMemory + ROMStartAddress, 1, 65536, f);
		fclose(f);
	}
	ROMEnabled = true;
	kvazidisk = k;
}

Memory::~Memory() {
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 65536; j++) memory[i][j] = 0x00;
	for (int j = 0; j < 65536; j++) ROMMemory[j] = 0x00;
	ROMEnabled = false;
	ROMStartAddress = 0;
	maxBankCount = 0;
	currentBank = 0;
	maxMemorySize = 0;
	ROMMemoryFileName = "";
	ROMSize = 0;
	kvazidisk = NULL;
}

void Memory::changeROMStatus(bool status) {
	ROMEnabled = status;
}

bool Memory::getROMStatus() {
	return ROMEnabled;
}

bool Memory::changeROM(string fileName, uint16_t ROMStartAddr) {
	ROMStartAddress = ROMStartAddr;
	ROMMemoryFileName = fileName;
	for (int j = 0; j < 65536; j++) ROMMemory[j] = 0xFF;
	FILE* f;
	f = fopen(ROMMemoryFileName.c_str(), "rb");
	if (f)
	{
		int ROMSize = fread(ROMMemory + ROMStartAddress, 1, 65536, f);
		fclose(f);
		ROMEnabled = true;
		return false;
	}
	return true;
}

bool Memory::setCurrentBank(uint8_t bank) {
	if (bank < maxBankCount) {
		currentBank = bank;
		return false;
	}
	else return true;
}

uint8_t Memory::getCurrentBank() {
	return currentBank;
}

bool Memory::setMaxBank(uint8_t bank) {
	if (bank < 16) {
		maxBankCount = bank;
		return false;
	}
	else return true;
}

uint8_t Memory::getMaxBank() {
	return maxBankCount;
}

bool Memory::getByte(uint16_t addr, uint8_t* data) {
	if (ROMEnabled && addr >= ROMStartAddress && addr < ROMStartAddress + ROMSize) {
		*data = ROMMemory[addr];
		return false;
	}
	else {
		if (kvazidisk && kvazidisk->getByte(addr, data)) {
			return false;
		}
		if (addr < maxMemorySize) {
			*data = memory[0][addr];
			return false;
		}
	}
	return true;
}

bool Memory::getByte(uint8_t bank, uint16_t addr, uint8_t* data) {
	if (bank == 0) return getByte(addr, data);
	if (bank < maxBankCount) {
		*data = memory[bank][addr];
		return false;
	}
	return true;
}

bool Memory::getByteFromCurrentBank(uint16_t addr, uint8_t* data) {
	if (currentBank == 0) return getByte(addr, data);
	*data = memory[currentBank][addr];
	return false;
}

bool Memory::getWord(uint16_t addr, uint16_t* data) {
	if (ROMEnabled && addr >= ROMStartAddress && addr + 1 < ROMStartAddress + ROMSize) {
		*data = ROMMemory[addr + 1];
		*data <<= 8;
		*data |= ROMMemory[addr];
		return false;
	}
	else {
		if (addr + 1 < maxMemorySize) {
			*data = memory[0][addr + 1];
			*data <<= 8;
			*data |= memory[0][addr];
			return false;
		}
	}
	return true;
}

bool Memory::getWord(uint8_t bank, uint16_t addr, uint16_t* data) {
	if (currentBank == 0) return getWord(addr, data);
	if (bank < maxBankCount) {
		*data = memory[bank][addr + 1];
		*data <<= 8;
		*data |= memory[bank][addr];
		return false;
	}
	return true;
}

bool Memory::getWordFromCurrentBank(uint16_t addr, uint16_t* data) {
	if (currentBank == 0) {
		if (kvazidisk && kvazidisk->getWord(addr, data)) {
			return false;
		}
		return getWord(addr, data);
	}
	*data = memory[currentBank][addr + 1];
	*data <<= 8;
	*data |= memory[currentBank][addr];
	return false;
}

bool Memory::setByte(uint16_t addr, uint8_t data) {
	if (kvazidisk && kvazidisk->setByte(addr, data)) {
		return false;
	}
	if (addr < maxMemorySize) {
		memory[0][addr] = data;
		return false;
	}
	else return true;
}

bool Memory::setDebugByte(uint16_t addr, uint8_t data) {
	
	if (ROMEnabled && addr >= ROMStartAddress && addr < ROMStartAddress + ROMSize) {
		ROMMemory[addr] = data;
		return false;
	}
	else {
		if (addr < maxMemorySize) {
			memory[0][addr] = data;
			return false;
		}
	}
	return true;
}

bool Memory::setDebugWord(uint16_t addr, uint16_t data) {
	if (ROMEnabled && addr >= ROMStartAddress && addr + 1 < ROMStartAddress + ROMSize) {
		ROMMemory[addr] = (uint8_t)(data & 0xFF);
		ROMMemory[addr + 1] = (uint8_t)(data >> 8);
		return false;
	}
	else {
		if (addr + 1 < maxMemorySize) {
			memory[0][addr] = (uint8_t)(data & 0xFF);
			memory[0][addr + 1] = (uint8_t)(data >> 8);
			return false;
		}
	}
	return true;
}

bool Memory::setByte(uint8_t bank, uint16_t addr, uint8_t data) {
	if (kvazidisk && kvazidisk->setByte(addr, data)) {
		return false;
	}
	if (addr < maxMemorySize && bank < maxBankCount) {
		memory[bank][addr] = data;
		return false;
	}
	else return true;
}

bool Memory::setByteToCurrentBank(uint16_t addr, uint8_t data) {
	if (kvazidisk && kvazidisk->setByte(addr, data)) {
		return false;
	}
	if (addr < maxMemorySize) {
		memory[currentBank][addr] = data;
		return false;
	}
	else return true;
}

bool Memory::setWord(uint16_t addr, uint16_t data) {
	if (addr + 1 < maxMemorySize) {
		memory[0][addr] = (uint8_t)(data & 0xFF);
		memory[0][addr+1] = (uint8_t)(data >> 8);
		return false;
	}
	else return true;
}

bool Memory::setWord(uint8_t bank, uint16_t addr, uint16_t data) {
	if (addr + 1 < maxMemorySize && bank < maxBankCount) {
		memory[bank][addr] = (uint8_t)(data & 0xFF);
		memory[bank][addr + 1] = (uint8_t)(data >> 8);
		return false;
	}
	else return true;
}

bool Memory::setWordToCurrentBank(uint16_t addr, uint16_t data) {
	if (kvazidisk && kvazidisk->setWord(addr, data)) {
		return false;
	}
	if (addr + 1 < maxMemorySize) {
		memory[currentBank][addr] = (uint8_t)(data & 0xFF);
		memory[currentBank][addr + 1] = (uint8_t)(data >> 8);
		return false;
	}
	else return true;
}

uint8_t* Memory::getVideoMemoryPointer() {
	uint8_t* mem = &memory[0][0];
	return mem;
}

uint8_t* Memory::getMemoryPointer(bool isRom, uint32_t* startAddr, uint32_t* endAddr) {
	uint8_t *mem;
	if (isRom) {
		mem = &ROMMemory[ROMStartAddress];
		*startAddr = ROMStartAddress;
		*endAddr = ROMStartAddress + ROMSize;
	}
	else {
		mem = &memory[0][0];
		*startAddr = 0;
		*endAddr = maxMemorySize;
	}
	return mem;
}

void Memory::resetMainMemory() {
	for (int j = 0; j < 65536; j++) memory[0][j] = 0x00;
}

void Memory::setMemory(uint8_t* buf, uint16_t addr, uint16_t cnt) {
	for (int i = 0; i < 0xFFFF; i++) memory[0][i] = 0;
	for (int i = 0; i < cnt; i++) memory[0][addr + i] = buf[i];
}