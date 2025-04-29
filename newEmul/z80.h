#ifndef __Z80_H__
#define __Z80_H__
#include "cpu.h"
#include "memory.h"
#include "emutypes.h"
#include "ports.h"

typedef union {
	uint16_t r16;
	struct {
		uint8_t lo;
		uint8_t hi;
	} r8;
} z80reg16;

typedef struct {
	z80reg16 bc, de, hl;
} z80reg;

class z80 : public CPU
{
public:
	z80(Memory* mem);
	~z80();
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
	uint8_t ports_in[65536];
	uint8_t ports_out[65536];
	uint16_t pc;
	uint16_t sp;
	z80reg16 ir;
	uint16_t ix;
	uint16_t iy;
	z80reg16 af[2];
	z80reg blockRegs[2];
	uint8_t currentBlock;
	uint8_t currentAF;
	bool INTE, HLT, INTREQ;
	Memory* memory;
	uint64_t TickCount;
	Ports* prt[255];

	void setfl();
	void summ(uint8_t sl, uint8_t ppp, uint8_t typ);
	uint8_t inrdcr(uint8_t sl, uint8_t typ);
	std::string getParam(std::string s, int* n);
	uint8_t GetIdRegsToName(std::string n);
	uint8_t GetIdRegsPairToName(std::string n);

	int pTable[256] = {
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };

	int addACtable[8] = { 0, 0, 1, 0, 1, 0, 1, 1 };
	int subACtable[8] = { 0, 1, 1, 1, 0, 0, 0, 1 };

};

#endif // !__Z80_H__

