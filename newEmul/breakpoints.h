#ifndef __BREAKPOINTS_H__
#define __BREAKPOINTS_H__

#include <stdint.h>
#include <string>

#include "hex.h"

class Breakpoints {
public:
	Breakpoints();
	~Breakpoints();

	void addAddr(uint16_t addr, bool isOnce);
	void addAddr(std::string addr, bool isOnce);
	void deleteAddr(uint16_t addr);
	bool checkAddr(uint16_t addr);

private:
	uint8_t* bpmap;

};


#endif // !__BREAKPOINTS_H__
