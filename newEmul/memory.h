#ifndef __MEMORY_H__
#define __MEMORY_H__
#include <string>

using namespace std;

class Memory {
public:
	Memory();
	Memory(uint32_t maxMainMemorySize, uint8_t maxBankNumber, uint16_t ROMStartAddr, string ROMFileName);
	~Memory();
	void changeROMStatus(bool status);
	bool getROMStatus();
	bool changeROM(string fileName, uint16_t ROMStartAddr);
	bool setCurrentBank(uint8_t bank);
	uint8_t getCurrentBank();
	bool setMaxBank(uint8_t bank);
	uint8_t getMaxBank();
	bool getByte(uint16_t addr, uint8_t* data);
	bool getByte(uint8_t bank, uint16_t addr, uint8_t* data);
	bool getByteFromCurrentBank(uint16_t addr, uint8_t* data);
	bool getWord(uint16_t addr, uint16_t* data);
	bool getWord(uint8_t bank, uint16_t addr, uint16_t* data);
	bool getWordFromCurrentBank(uint16_t addr, uint16_t* data);
	bool setByte(uint16_t addr, uint8_t data);
	bool setByte(uint8_t bank, uint16_t addr, uint8_t data);
	bool setDebugByte(uint16_t addr, uint8_t data);
	bool setDebugWord(uint16_t addr, uint16_t data);
	bool setByteToCurrentBank(uint16_t addr, uint8_t data);
	bool setWord(uint16_t addr, uint16_t data);
	bool setWord(uint8_t bank, uint16_t addr, uint16_t data);
	bool setWordToCurrentBank(uint16_t addr, uint16_t data);
	uint8_t* getVideoMemoryPointer();
	uint8_t* getMemoryPointer(bool isRom, uint32_t *startAddr, uint32_t *endAddr);
private:
	uint8_t memory[16][65536];
	uint8_t ROMMemory[65536];
	bool ROMEnabled;
	uint16_t ROMStartAddress;
	uint16_t ROMSize;
	uint8_t maxBankCount;
	uint8_t currentBank;
	uint32_t maxMemorySize;
	string ROMMemoryFileName;
};

#endif // !__MEMORY_H__

