#ifndef __CPU_H__
#define __CPU_H__
#include <string>
#include "emutypes.h"
#include "ports.h"


class CPU
{
public:
	CPU() {};
	~CPU() {};
	virtual void reset() = 0;
	virtual Registers getRegisters() = 0;
	virtual std::string disAsm(uint16_t* addr, uint8_t n) = 0;
	virtual uint16_t getPC() = 0;
	virtual uint8_t execute(uint16_t addr) = 0;
	virtual uint8_t execute() = 0;
	virtual void SetDebugValue(uint16_t tp, uint16_t num, std::string data) = 0;
	virtual void SetAssemblerValue(std::string addr, std::string value) = 0;
	virtual bool isHalted() = 0;
	virtual void setPort(uint16_t num, Ports* dev) = 0;
};


#endif // !__CPU_H__