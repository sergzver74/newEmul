#ifndef __MACHINE_H__
#define __MACHINE_H__
#include "cpu.h"
#include "memory.h"

class Machine
{
public:
	Machine() {};
	~Machine() {};
	virtual void timer() = 0;
	virtual CPU *getCPU() = 0;
	virtual Memory* getMemory() = 0;
	virtual void trace() = 0;
	virtual void step() = 0;
	virtual void stepTo(std::string addr) = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void keyDown(uint32_t keyCode) = 0;
	virtual void keyUp(uint32_t keyCode) = 0;
	virtual void loadProgramToMemory(std::string url, bool addr0) = 0;
	virtual void loadDataToMemory(std::string url, int tp) = 0;
};


#endif // !__MACHINE_H__

