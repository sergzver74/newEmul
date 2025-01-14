#ifndef __k1801vm1_H__
#define __k1801vm1_H__

#include "cpu.h"
#include "memory.h"
#include "emutypes.h"
#include "ports.h"

class k1801vm1 : public CPU {
public:
	k1801vm1(Memory* mem);
	~k1801vm1();
	void reset();
	uint8_t execute();
	uint8_t execute(uint16_t addr);
	string dasm(uint16_t* addr);
	string disAsm(uint16_t* addr, uint8_t n);
	Registers getRegisters();
	uint8_t getPortData(uint8_t port);
	void setPortData(uint8_t port, uint8_t data);
	void interruptRequest();
	uint16_t getPC();
	void SetDebugValue(uint16_t tp, uint16_t num, std::string data);
	void SetAssemblerValue(std::string addr, std::string value);
	bool isHalted();
	void setPort(uint16_t num, Ports* dev);
	uint8_t getPort(uint8_t num);
	uint16_t getPort(uint16_t num);

private:
	Memory* memory;
	uint16_t R0;
	uint16_t R1;
	uint16_t R2;
	uint16_t R3;
	uint16_t R4;
	uint16_t R5;
	uint16_t SP;
	uint16_t PC;
	uint16_t PSW;
	bool HLT;

};

#endif // !__K1801vm1_H__
