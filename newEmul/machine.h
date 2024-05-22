#ifndef __MACHINE_H__
#define __MACHINE_H__
#include "cpu.h"
#include "memory.h"
#include "vkeyboard.h"

#define STOPADDR	0
#define CHANGEADDR  1
#define EQUALVAL	2
#define NONEQUALVAL 3

typedef struct {
	uint16_t addr;
	uint8_t type;
	uint16_t value;
}Breakpoint;

typedef struct {
	uint8_t count;
	Breakpoint bp[64];
}Breakpoints;

class Machine
{
public:
	Machine() {};
	~Machine() {};
	virtual void timer() = 0;
	virtual CPU *getCPU() = 0;
	virtual Memory* getMemory() = 0;
	//virtual void trace() = 0;
	//virtual void step() = 0;
	virtual void stepTo(std::string addr) = 0;
	virtual void start(uint8_t mode) = 0;
	virtual void stop() = 0;
	virtual void keyDown(uint32_t keyCode) = 0;
	virtual void keyUp(uint32_t keyCode) = 0;
	virtual void loadProgramToMemory(std::string url, bool addr0) = 0;
	virtual void loadDataToMemory(std::string url, int tp) = 0;
	virtual vKeybParams getVirtualKeyboardParams() = 0;
	virtual bool getStatus() = 0;
	virtual bool udateNeeded() = 0;
	virtual void updated() = 0;
};


#endif // !__MACHINE_H__

