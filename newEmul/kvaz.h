#ifndef __KVAZ_H__
#define __KVAZ_H__

#include "ports.h"

class kvaz : public Ports {
public:
	kvaz();
	~kvaz();

	void setPortData(uint16_t portNum, uint16_t data);
	uint16_t getPortData(uint16_t portNum);
	tPorts getPorts();
	bool getByte(uint16_t addr, uint8_t* data);
	bool getWord(uint16_t addr, uint16_t* data);
	bool getWordFromStack(uint16_t addr, uint16_t* data);
	bool setByte(uint16_t addr, uint8_t data);
	bool setWord(uint16_t addr, uint16_t data);
	bool setWordToStack(uint16_t addr, uint16_t data);
	uint8_t* getMemoryPointer();

private:
	uint8_t port10;

	uint32_t currentScreenPage;
	uint32_t currentStackPage;
	bool quaziDiskStackMode;
	bool quaziDiskScreenModeAD;

	//uint8_t mem[4][65536];
	uint8_t mem[262144];

};



#endif // !__KVAZ_H__
