#ifndef __PORTS_H__
#define __PORTS_H__

#include <stdint.h>

typedef struct {
	uint8_t count;
	uint8_t ports[255];
}tPorts;

class Ports {
protected:
	tPorts ports;
public:

	Ports() {};
	~Ports() {};
	virtual void setPortData(uint16_t portNum, uint16_t data) = 0;
	virtual uint16_t getPortData(uint16_t portNum) = 0;
	virtual tPorts getPorts() = 0;
};

#endif // __PORTS_H__

