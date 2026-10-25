#ifndef __IDE_H__
#define __IDE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string>

#include "ports.h"

class IDE : public Ports
{
public:
	IDE(std::string nm);
	~IDE();
	
	void setPortData(uint16_t portNum, uint16_t data);
	uint16_t getPortData(uint16_t portNum);
	tPorts getPorts();

private:

};

#endif // !__IDE_H__

