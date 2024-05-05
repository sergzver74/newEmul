#include "k580vi53.h"

Counter::Counter() {

}

Counter::~Counter() {

}

void Counter::SetRWMode(uint8_t rwMode) {

}

void Counter::SetMode(uint8_t mode, uint8_t rwMode, bool bcd) {

}

void Counter::writeCounter(uint8_t data) {

}

uint8_t Counter::readCounter() {
	
	return 0;
}

uint8_t Counter::step() {
	
	return 0;
}



K580VI53::K580VI53()
{
	controlUnitPort = 0;
	bcd = false;
	mode = 0;
	rwMode = 0;
	channel = 0;
	out0 = 0;
	out1 = 0;
	out2 = 0;
	for (int i = 0; i < 3; i++) {
		counters[i] = new Counter();
	}
}

K580VI53::~K580VI53()
{
	for (int i = 0; i < 3; i++) {
		delete counters[i];
	}
	controlUnitPort = 0;
	bcd = false;
	mode = 0;
	rwMode = 0;
	channel = 0;
	out0 = 0;
	out1 = 0;
	out2 = 0;
}

void K580VI53::setControlUnit(uint8_t data) {
	controlUnitPort = data;
	bcd = data & 0x01;
	mode = (data >> 1) & 0x07;
	rwMode = (data >> 4) & 0x03;
	channel = (data >> 6) & 0x03;
	if (channel == 3) {
		printf("Error channel number to timer\n");
		return;
	}
	if (rwMode == 0) {
		counters[channel]->SetRWMode(rwMode);
	}
	else {
		counters[channel]->SetMode(mode, rwMode, bcd);
	}
}

void K580VI53::setPortData(uint16_t portNum, uint16_t data) {
	if (portNum == 0x08) {
		setControlUnit(data);
	}
	else {
		counters[(portNum - 1) & 0x03]->writeCounter(data);
	}
}

uint16_t K580VI53::getPortData(uint16_t portNum) {
	uint16_t pRes = 0;

	if (portNum == 0x08) {
		pRes = controlUnitPort;
	}
	else {
		pRes = counters[(portNum - 1) & 0x03]->readCounter();
	}

	return pRes;
}

void K580VI53::step() {
	out0 = counters[0]->step();
	out1 = counters[1]->step();
	out2 = counters[2]->step();
}

uint8_t K580VI53::getOutStates() {
	return out0 + out1 + out2;
}

uint8_t K580VI53::getOutState(uint8_t chNumber) {
	switch (chNumber)
	{
	case 0: return out0;
	case 1: return out1;
	case 2: return out2;
	default:
		return 0;
	}
}

tPorts K580VI53::getPorts() {
	ports.count = 4;
	ports.ports[0] = 0x08;
	ports.ports[1] = 0x09;
	ports.ports[2] = 0x0A;
	ports.ports[3] = 0x0B;
	return ports;
}

void K580VI53::reset() {
	for (int i = 0; i < 3; i++) {
		delete counters[i];
	}
	for (int i = 0; i < 3; i++) {
		counters[i] = new Counter();
	}
	controlUnitPort = 0;
	bcd = false;
	mode = 0;
	rwMode = 0;
	channel = 0;
	out0 = 0;
	out1 = 0;
	out2 = 0;
}