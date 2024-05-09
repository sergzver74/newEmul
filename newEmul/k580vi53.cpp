#include "k580vi53.h"

static uint16_t tobcd(uint16_t x) {
	int result = 0;
	for (int i = 0; i < 4; ++i) {
		result |= (x % 10) << (i * 4);
		x /= 10;
	}
	return result;
}

static uint16_t frombcd(uint16_t x) {
	int result = 0;
	for (int i = 0; i < 4; ++i) {
		int digit = (x & 0xf000) >> 12;
		if (digit > 9) digit = 9;
		result = result * 10 + digit;
		x <<= 4;
	}
	return result;
}

Counter::Counter() {
	latchValue = 0;
	writeState = 0;
	latchMode = 0;
	out = 0;
	value = 0;
	modeInt = 0;

	writeLsb = 0;
	writeMsb = 0;
	loadValue = 0;
	armed = false;
	load = false;
	enabled = false;
	bcd = false;
	delay = 0;
}

Counter::~Counter() {
	latchValue = 0;
	writeState = 0;
	latchMode = 0;
	out = 0;
	value = 0;
	modeInt = 0;

	writeLsb = 0;
	writeMsb = 0;
	loadValue = 0;
	armed = false;
	load = false;
	enabled = false;
	bcd = false;
	delay = 0;
}

void Counter::SetRWMode() {
	step();
	delay = 1;
	latchValue = value;
}

void Counter::SetMode(uint8_t mode, uint8_t rwMode, bool bcdMode) {
	step();
	delay = 1;

	bcd = bcdMode;
	if ((mode & 0x03) == 2) {
		modeInt = 2;
	}
	else if ((mode & 0x03) == 3) {
		modeInt = 3;
	}
	else {
		modeInt = mode;
	}

	switch (modeInt) {
	case 0:
		out = 0;
		armed = true;
		enabled = false;
		break;
	case 1:
		out = 1;
		armed = true;
		enabled = false;
		break;
	case 2:
		out = 1;
		enabled = false;
		break;
	default:
		out = 1;
		enabled = false;
		break;
	}
	load = false;
	latchMode = rwMode;
	writeState = 0;
}

void Counter::writeCounter(uint8_t data) {
	if (latchMode == 3) {
		// lsb, msb             
		switch (writeState) {
		case 0:
			writeLsb = data;
			writeState = 1;
			break;
		case 1:
			writeMsb = data;
			writeState = 0;
			loadValue = ((writeMsb << 8) & 0xffff) |
				(writeLsb & 0xff);
			this->load = true;
			break;
		default:
			break;
		}
	}
	else if (latchMode == 1) {
		loadValue = data;
		load = true;
	}
	else if (latchMode == 2) {
		value = data << 8;
		value &= 0xffff;
		loadValue = value;
		load = true;
	}
	if (load) {
		if (bcd) {
			loadValue = frombcd(loadValue);
		}
		switch (modeInt) {
		case 0:
			delay = 3; break;
		case 1:
			if (!enabled) {
				delay = 3;
			}
			break;
		case 2:
			if (!enabled) {
				delay = 3;
			}
			break;
		case 3:
			if (!enabled) {
				delay = 3;
			}
			break;
		default:
			delay = 4;
			break;
		}
	}
}

uint8_t Counter::readCounter() {
	

	int value = 0;
	switch (latchMode) {
	case 0:
		// impossible
		break;
	case 1:
		value = latchValue != -1 ? latchValue : value;
		latchValue = -1;
		value = bcd ? tobcd(value) : value;
		value &= 0xff;
		break;
	case 2:
		value = latchValue != -1 ? latchValue : value;
		latchValue = -1;
		value = bcd ? tobcd(value) : value;
		value = (value >> 8) & 0xff;
		break;
	case 3:
		value = latchValue != -1 ? latchValue : value;
		value = bcd ? tobcd(value) : value;
		switch (writeState) {
		case 0:
			writeState = 1;
			value = value & 0xff;
			break;
		case 1:
			latchValue = -1;
			writeState = 0;
			value = (value >> 8) & 0xff;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return value;


	return 0;
}

uint8_t Counter::step() {
	
	int cycles = 1;
	if (delay) {
		delay--;
		cycles = 0;
	}
	if (!cycles) return out;
	if (!enabled && !load) return out;
	int result = out;

	switch (modeInt) {
	case 0: // Interrupt on terminal count
		if (load) {
			value = loadValue;
			enabled = true;
			armed = true;
			out = 0;
			result = 0;
		}
		if (enabled) {
			int previous = value;
			value -= cycles;
			if (value <= 0) {
				if (armed) {
					if (previous != 0) out = 1;
					result = -value + 1;
					armed = false;
				}
				value += bcd ? 10000 : 65536;
			}
		}
		break;
	case 1: // Programmable one-shot
		if (!enabled && load) {
			enabled = true;
		}
		if (enabled) {
			value -= cycles;
			if (value <= 0) {
				int reload = loadValue == 0 ?
					(bcd ? 10000 : 0x10000) : (loadValue + 1);
				value += reload;
			}
		}
		break;
	case 2: // Rate generator
		if (!enabled && load) {
			value = loadValue;
			enabled = true;
		}
		if (enabled) {
			value -= cycles;
			if (value <= 0) {
				int reload = loadValue == 0 ?
					(bcd ? 10000 : 0x10000) : loadValue;
				value += reload;
			}
		}
		break;
	case 3: // Square wave generator
		if (!enabled && load) {
			value = loadValue;
			enabled = true;
		}
		if (enabled) {
			value -=
				((value == loadValue) && ((value & 1) == 1)) ?
				out == 0 ? 3 : 1 : 2;
			if (value <= 0) {
				out ^= 1;

				int reload = (loadValue == 0) ?
					(bcd ? 10000 : 0x10000) : loadValue;
				value += reload;
			}
		}
		result = out;
		break;
	case 4: // Software triggered strobe
		break;
	case 5: // Hardware triggered strobe
		break;
	default:
		break;
	}

	this->load = false;
	return result;
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
		counters[channel]->SetRWMode();
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
		uint8_t ch = 2 - (portNum - 1) & 0x03;
		counters[ch]->writeCounter(data);
	}
}

uint16_t K580VI53::getPortData(uint16_t portNum) {
	uint16_t pRes = 0;

	if (portNum == 0x08) {
		pRes = controlUnitPort;
	}
	else {
		uint8_t ch = 2 - (portNum - 1) & 0x03;
		pRes = counters[ch]->readCounter();
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