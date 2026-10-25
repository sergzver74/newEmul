#include "ide.h"

using namespace std;

IDE::IDE(string fn) {

}

IDE::~IDE() {

}


void IDE::setPortData(uint16_t portNum, uint16_t data) {

}

uint16_t IDE::getPortData(uint16_t portNum) {

	return 0;
}

tPorts IDE::getPorts() {
	ports.count = 10;
	ports.ports[0] = 0x50;
	ports.ports[1] = 0x51;
	ports.ports[2] = 0x52;
	ports.ports[3] = 0x53;
	ports.ports[4] = 0x54;
	ports.ports[5] = 0x55;
	ports.ports[6] = 0x56;
	ports.ports[7] = 0x57;
	ports.ports[8] = 0x58;
	ports.ports[9] = 0x5F;
	return ports;
}