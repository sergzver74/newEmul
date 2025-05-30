#ifndef __K580VI53_H__
#define __K580VI53_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ports.h"

class Counter {
public:
	Counter();
	~Counter();

	void SetRWMode();
	void SetMode(uint8_t mode, uint8_t rwMode, bool bcdMode);
	void writeCounter(uint8_t data);
	uint8_t readCounter();
	uint8_t step();

private:
	int latchValue;
	int writeState;
	int latchMode;
	int out;
	int value;
	int modeInt;

	uint8_t writeLsb;
	uint8_t writeMsb;
	uint16_t loadValue;

	bool armed;
	bool load;
	bool enabled;
	bool bcd;

	int delay;

};

class K580VI53 : public Ports
{
public:
	K580VI53();
	~K580VI53();

	void setPortData(uint16_t portNum, uint16_t data);
	uint16_t getPortData(uint16_t portNum);
	tPorts getPorts();
	void reset();
	void step();
	uint8_t getOutStates();
	uint8_t getOutState(uint8_t chNumber);

private:
	void setControlUnit(uint8_t data);

	
	Counter *counters[3];
	uint8_t controlUnitPort;
	bool bcd;
	uint8_t mode;
	uint8_t rwMode;
	uint8_t channel;
	uint8_t out0;
	uint8_t out1;
	uint8_t out2;

};

#endif
