#ifndef __Z80_H__
#define __Z80_H__
#include "emutypes.h"
#include "cpu.h"


class z80 : public CPU
{
public:
	z80() {};
	~z80() {};
	void reset();
	Registers getRegisters();
	std::string disAsm(uint16_t* addr, uint8_t n);

private:

};

#endif // !__Z80_H__

