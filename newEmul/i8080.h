#ifndef __I8080_H__
#define __I8080_H__
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
} i8080reg16;

class i8080 : public CPU
{
public:
	i8080(Memory* mem);
	~i8080();
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
	uint8_t ports_in[256];
	uint8_t ports_out[256];
	uint16_t pc;
	uint16_t sp;
	i8080reg16 bc, de, hl;
	uint8_t a, fl;
	bool INTE, HLT, INTREQ;
	Memory *memory;
	uint64_t TickCount;
	Ports* prt[255];

	void setfl();
	void summ(uint8_t sl, uint8_t ppp, uint8_t typ);
	uint8_t inrdcr(uint8_t sl, uint8_t typ);
	std::string getParam(std::string s, int* n);
	uint8_t GetIdRegsToName(std::string n);
	uint8_t GetIdRegsPairToName(std::string n);
};


#endif // !__I8080_H__
