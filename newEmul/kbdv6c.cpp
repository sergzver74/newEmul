#include "kbdv6c.h"

Keyboard::Keyboard() {
	
	uint8_t k = 0;
	uint8_t pr = 0;
	
	for (int i = 0; i < 256; i++)
	{
		kp02[i] = 0xFF;
		kp03[i] = 0xFF;
	}
	for (uint8_t i = 0; i < 8; i++)
		for (uint8_t j = 0; j < 8; j++)
		{
			k = VectorKeyboard[i][j];
			pr = 1;
			pr = pr << (7 - i);
			pr ^= 0xFF;
			kp03[k] = pr;
			pr = 1;
			pr = pr << (7 - j);
			pr ^= 0xFF;
			kp02[k] = pr;
		}
	blk = false;
	vvod = false;
	sbr = false;
	p02 = 0xFF;
	p03 = 0xFF;
}

tPorts Keyboard::getPorts() {
	ports.count = 1;
	ports.ports[0] = 0x01;
	return ports;
}

void Keyboard::setPortData(uint16_t portNum, uint16_t data) {
	if (portNum == 1) port01 = data;
	if (portNum == 2) port02 = data;
	if (portNum == 3) {
		port03 = data;
		if ((data == p03) || (data == 0)) port02 = p02; else port02 = 0xFF;
	}
}

uint16_t Keyboard::getPortData(uint16_t portNum) {
	if (portNum == 1) return port01;
	if (portNum == 2) return port02;
	if (portNum == 3) return port03;
	return 0x00;
}

uint8_t Keyboard::keyDown(uint8_t key) {
	if (key == 0x3F) sbr = true;
	if (key == 0x40) blk = true;
	if (key == 0x41) vvod = true;

	if (key == 0xE1) port01 &= 0xDF;
	if (key == 0xE0) port01 &= 0xBF;
	if (key == 0xE2) port01 &= 0x7F;
	
	p03 = kp03[key];
	p02 = kp02[key];
	
	if (vvod && blk) return 1;
	if (sbr && blk) return 2;
	return 0;
}

uint8_t Keyboard::keyUp(uint8_t key) {
	if (key == 0x3F) vvod = false;
	if (key == 0x40) blk = false;
	if (key == 0x41) sbr = false;

	if (key == 0xE1) port01 |= 0x20;
	if (key == 0xE0) port01 |= 0x40;
	if (key == 0xE2) port01 |= 0x80;

	port02 = 0xFF;
	port03 = 0xFF;
	p02 = 0xFF;
	p03 = 0xFF;

	return 0;

}