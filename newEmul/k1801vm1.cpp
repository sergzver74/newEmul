#include "k1801vm1.h"
#include "hex.h"

string oct(uint16_t a)
{
	char* c;
	c = new char[256];

	sprintf_s(c, 256, "%06o", a);

	string s(c);
	delete[] c;

	return s;
}

k1801vm1::k1801vm1(Memory* mem) {
	memory = mem;
	R0 = 0;
	R1 = 0;
	R2 = 0;
	R3 = 0;
	R4 = 0;
	R5 = 0;
	SP = 0;
	PC = 0;
	PSW = 0340;
	HLT = false;
}

k1801vm1::~k1801vm1() {

}

uint8_t k1801vm1::getPortData(uint8_t port) {
	return 0;
}

void k1801vm1::setPortData(uint8_t port, uint8_t data) {
	
}

Registers k1801vm1::getRegisters() {
	Registers reg;
	reg.count = 0;
	reg.registers[reg.count].name = "R0";
	reg.registers[reg.count].data = (uint32_t)R0;
	reg.registers[reg.count++].strData = oct(R0);
	reg.registers[reg.count].name = "R1";
	reg.registers[reg.count].data = (uint32_t)R1;
	reg.registers[reg.count++].strData = oct(R1);
	reg.registers[reg.count].name = "R2";
	reg.registers[reg.count].data = (uint32_t)R2;
	reg.registers[reg.count++].strData = oct(R2);
	reg.registers[reg.count].name = "R3";
	reg.registers[reg.count].data = (uint32_t)R3;
	reg.registers[reg.count++].strData = oct(R3);
	reg.registers[reg.count].name = "R4";
	reg.registers[reg.count].data = (uint32_t)R4;
	reg.registers[reg.count++].strData = oct(R4);
	reg.registers[reg.count].name = "R5";
	reg.registers[reg.count].data = (uint32_t)R5;
	reg.registers[reg.count++].strData = oct(R5);
	reg.registers[reg.count].name = "SP";
	reg.registers[reg.count].data = (uint32_t)SP;
	reg.registers[reg.count++].strData = oct(SP);
	reg.registers[reg.count].name = "PC";
	reg.registers[reg.count].data = (uint32_t)PC;
	reg.registers[reg.count++].strData = oct(PC);
	reg.registers[reg.count].name = "FLAGS";
	reg.registers[reg.count].data = (uint32_t)PSW;
	reg.registers[reg.count++].strData = oct(PSW);
	reg.flags.count = 0;
	reg.flags.flags[reg.flags.count].name = "I";
	reg.flags.flags[reg.flags.count].data = PSW & 0x0080;
	reg.flags.flags[reg.flags.count++].strData = PSW & 0x0080 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "T";
	reg.flags.flags[reg.flags.count].data = PSW & 0x0010;
	reg.flags.flags[reg.flags.count++].strData = PSW & 0x0010 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "N";
	reg.flags.flags[reg.flags.count].data = PSW & 0x0008;
	reg.flags.flags[reg.flags.count++].strData = PSW & 0x0008 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "Z";
	reg.flags.flags[reg.flags.count].data = PSW & 0x0004;
	reg.flags.flags[reg.flags.count++].strData = PSW & 0x0004 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "V";
	reg.flags.flags[reg.flags.count].data = PSW & 0x0002;
	reg.flags.flags[reg.flags.count++].strData = PSW & 0x0002 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "C";
	reg.flags.flags[reg.flags.count].data = PSW & 0x0001;
	reg.flags.flags[reg.flags.count++].strData = PSW & 0x0001 ? "1" : "0";
	return reg;
}

void k1801vm1::reset() {
	R0 = 0;
	R1 = 0;
	R2 = 0;
	R3 = 0;
	R4 = 0;
	R5 = 0;
	SP = 0;
	PC = 0;
	PSW = 0340;
	HLT = false;
}

uint8_t k1801vm1::execute(uint16_t addr) {
	uint16_t  tmpPC = PC;
	PC = addr;
	uint8_t res = execute();
	PC = tmpPC;
	return res;
}

uint8_t k1801vm1::execute() {
	uint16_t addr;
	//uint8_t dopcode, olda, b1;
	uint8_t NumTicks = 0;
	//uint16_t afl;

	//if (HLT && !INTE) return 4;
	//if (HLT) return 4;

	uint8_t code;
	bool mRes = false;
	//if (!INTREQ) {
		mRes = memory->getByte(PC, &code);
		if (mRes) {
			return NumTicks;
		}
		PC += 2;
	//}
	//else {
	//	INTREQ = false;
	//	code = 0xFF;
	//	INTE = false;
	//	HLT = false;
	//}

	switch (code)
	{
	case 0x00:
		break;
	}

	if (mRes) {
		return 0;
	}
	return NumTicks;
}

bool k1801vm1::isHalted() {
	return HLT;
}

string k1801vm1::dasm(uint16_t* addr)
{
	string s = "";
	uint8_t code, b1;
	uint16_t w1;

	bool mRes = memory->getByte(*addr, &code);
	if (mRes) {
		return s;
	}
	(*addr)+=2;

	switch (code)
	{
	case 0x00:
		break;
	}
	return s;
}

string k1801vm1::disAsm(uint16_t* addr, uint8_t n) {
	string res = "";
	for (int i = 0; i < n; i++) {
		res += oct(*addr);
		res += "  ";
		res += dasm(addr);
		res += "\n";
	}

	return res;
}

uint16_t k1801vm1::getPC() {
	return PC;
}

void k1801vm1::SetDebugValue(uint16_t tp, uint16_t num, std::string data) {
	uint16_t dta = OctToDec(data);
	if (tp == 1) {
		switch (num) {
		case 0: R0 = dta;
			break;
		case 1: R1 = dta;
			break;
		case 2: R2 = dta;
			break;
		case 3: R3 = dta;
			break;
		case 4: R4 = dta;
			break;
		case 5: R5 = dta;
			break;
		case 6: SP = dta;
			break;
		case 7: PC = dta;
			break;
		case 8: PSW = dta;
			break;
		}
	}
	if (tp == 2) {
		switch (num) {
		case 0: if (dta == 1) PSW |= (1 << 7); else PSW &= (~(1 << 7));
			break;
		case 1: if (dta == 1) PSW |= (1 << 4); else PSW &= (~(1 << 4));
			break;
		case 2: if (dta == 1) PSW |= (1 << 3); else PSW &= (~(1 << 3));
			break;
		case 3: if (dta == 1) PSW |= (1 << 2); else PSW &= (~(1 << 2));
			break;
		case 4: if (dta == 1) PSW |= (1 << 1); else PSW &= (~(1 << 1));
			break;
		case 5: if (dta == 1) PSW |= 1; else PSW &= 0xFE;
			break;
		}
	}
}