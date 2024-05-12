#include <iostream>
#include "i8080.h"
#include "hex.h"

using namespace std;

int pTable[] = {
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };

int addACtable[] = { 0, 0, 1, 0, 1, 0, 1, 1 };
int subACtable[] = { 0, 1, 1, 1, 0, 0, 0, 1 };

string hex(uint16_t a)
{
	char* c;
	c = new char[256];

	sprintf_s(c, 256, "%02X", a);

	string s(c);
	delete[] c;

	return s;
}

string hexaddr(uint16_t a)
{
	char* c;
	c = new char[256];

	sprintf_s(c, 256, "%04X", a);

	string s(c);
	delete[] c;

	return s;
}

string bin(uint8_t a) {
	string s = "";
	for (int i = 0; i < 8; i++) {
		a % 2 == 0 ? s = '0' + s : s = '1' + s;
		a /= 2;
	}
	return s;
}

i8080::i8080(Memory *mem) {
	memory = mem;
	for (int i = 0; i < 256; i++)
	{
		ports_in[i] = 0;
		ports_out[i] = 0;
	}
	HLT = false;
	TickCount = 0;
	INTE = false;
	INTREQ = false;
	pc = 0;
	fl = 2;
	sp = 0;
	a = 0;
	bc.r16 = 0;
	de.r16 = 0;
	hl.r16 = 0;
	for (int i = 0; i < 255; i++) prt[i] = NULL;
}

i8080::~i8080() {

}

uint8_t i8080::getPortData(uint8_t port) {
	return ports_out[port];
}

void i8080::setPortData(uint8_t port, uint8_t data) {
	ports_in[port] = data;
}

Registers i8080::getRegisters() {
	Registers reg;
	reg.count = 0;
	reg.registers[reg.count].name = "A";
	reg.registers[reg.count].data = (uint32_t)a;
	reg.registers[reg.count++].strData = hex(a);
	reg.registers[reg.count].name = "B";
	reg.registers[reg.count].data = (uint32_t)bc.r8.hi;
	reg.registers[reg.count++].strData = hex(bc.r8.hi);
	reg.registers[reg.count].name = "C";
	reg.registers[reg.count].data = (uint32_t)bc.r8.lo;
	reg.registers[reg.count++].strData = hex(bc.r8.lo);
	reg.registers[reg.count].name = "D";
	reg.registers[reg.count].data = (uint32_t)de.r8.hi;
	reg.registers[reg.count++].strData = hex(de.r8.hi);
	reg.registers[reg.count].name = "E";
	reg.registers[reg.count].data = (uint32_t)de.r8.lo;
	reg.registers[reg.count++].strData = hex(de.r8.lo);
	reg.registers[reg.count].name = "H";
	reg.registers[reg.count].data = (uint32_t)hl.r8.hi;
	reg.registers[reg.count++].strData = hex(hl.r8.hi);
	reg.registers[reg.count].name = "L";
	reg.registers[reg.count].data = (uint32_t)hl.r8.lo;
	reg.registers[reg.count++].strData = hex(hl.r8.lo);
	reg.registers[reg.count].name = "BC";
	reg.registers[reg.count].data = (uint32_t)bc.r16;
	reg.registers[reg.count++].strData = hexaddr(bc.r16);
	reg.registers[reg.count].name = "DE";
	reg.registers[reg.count].data = (uint32_t)de.r16;
	reg.registers[reg.count++].strData = hexaddr(de.r16);
	reg.registers[reg.count].name = "HL";
	reg.registers[reg.count].data = (uint32_t)hl.r16;
	reg.registers[reg.count++].strData = hexaddr(hl.r16);
	reg.registers[reg.count].name = "SP";
	reg.registers[reg.count].data = (uint32_t)sp;
	reg.registers[reg.count++].strData = hexaddr(sp);
	reg.registers[reg.count].name = "PC";
	reg.registers[reg.count].data = (uint32_t)pc;
	reg.registers[reg.count++].strData = hexaddr(pc);
	reg.registers[reg.count].name = "FLAGS";
	reg.registers[reg.count].data = (uint32_t)fl;
	reg.registers[reg.count++].strData = hex(fl);
	reg.flags.count = 0;
	reg.flags.flags[reg.flags.count].name = "S";
	reg.flags.flags[reg.flags.count].data = fl & 0x80;
	reg.flags.flags[reg.flags.count++].strData = fl & 0x80 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "Z";
	reg.flags.flags[reg.flags.count].data = fl & 0x40;
	reg.flags.flags[reg.flags.count++].strData = fl & 0x40 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "AC";
	reg.flags.flags[reg.flags.count].data = fl & 0x10;
	reg.flags.flags[reg.flags.count++].strData = fl & 0x10 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "P";
	reg.flags.flags[reg.flags.count].data = fl & 0x4;
	reg.flags.flags[reg.flags.count++].strData = fl & 0x4 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "CY";
	reg.flags.flags[reg.flags.count].data = fl & 0x1;
	reg.flags.flags[reg.flags.count++].strData = fl & 0x1 ? "1" : "0";
	return reg;
}

void i8080::reset() {
	for (int i = 0; i < 256; i++)
	{
		ports_in[i] = 0;
		ports_out[i] = 0;
	}
	HLT = false;
	TickCount = 0;
	INTE = false;
	INTREQ = false;
	pc = 0;
	fl = 2;
	sp = 0;
	a = 0;
	bc.r16 = 0;
	de.r16 = 0;
	hl.r16 = 0;
}


void i8080::setfl()
{
	uint8_t k, b;

	b = a;
	k = 0;
	do
	{
		if (b % 2 == 1) k++;
		b /= 2;
	} while (b != 0);

	if (a == 0) fl |= 0x40; else fl &= 0xBF;
	if (a > 127) fl |= 0x80; else fl &= 0x7F;
	if ((k & 0x01) == 0) fl |= 0x04; else fl &= 0xFB;
}


void i8080::summ(uint8_t sl, uint8_t ppp, uint8_t typ)
{
	uint16_t temp = 0;
	uint8_t iAC = 0;
	if (typ)
	{
		temp = a - sl - ppp;
		iAC = (((a & 0x08) >> 1) | ((sl & 0x08) >> 2) | ((temp & 0x08) >> 3)) & 0x07;
		if (subACtable[iAC] == 1) fl &= 0xEF; else fl |= 0x10;
	}
	else
	{
		temp = a + sl + ppp;
		iAC = (((a & 0x08) >> 1) | ((sl & 0x08) >> 2) | ((temp & 0x08) >> 3)) & 0x07;
		if (addACtable[iAC] == 1) fl |= 0x10; else fl &= 0xEF;
	}

	a = (uint8_t)temp;

	if (a == 0) fl |= 0x40; else fl &= 0xBF;
	if ((a & 0x80) != 0) fl |= 0x80; else fl &= 0x7F;
	if (pTable[a] == 1) fl |= 0x04; else fl &= 0xFB;
	if ((temp & 0x0100) != 0) fl |= 0x01; else fl &= 0xFE;
}


uint8_t i8080::inrdcr(uint8_t sl, uint8_t typ)
{
	if (typ)
	{
		sl--;
		if ((sl & 0x0F) == 0x0F) fl &= 0xEF; else fl |= 0x10;
	}
	else
	{
		sl++;
		if ((sl & 0x0F) == 0) fl |= 0x10; else fl &= 0xEF;
	}
	if (sl == 0) fl |= 0x40; else fl &= 0xBF;
	if ((sl & 0x80) != 0) fl |= 0x80; else fl &= 0x7F;
	if (pTable[sl] == 1) fl |= 0x04; else fl &= 0xFB;
	return sl;
}

uint8_t i8080::execute(uint16_t addr) {
	uint16_t  tmpPC = pc;
	pc = addr;
	uint8_t res = execute();
	pc = tmpPC;
	return res;
}

uint8_t i8080::execute() {
	uint16_t addr;
	uint8_t dopcode, olda, b1;
	uint8_t NumTicks = 0;
	uint16_t afl;

	if (HLT && !INTE) return 4;
	if (HLT) return 4;

	uint8_t code;
	bool mRes = false;
	if (!INTREQ) {
		mRes = memory->getByte(pc++, &code);
		if (mRes) {
			return NumTicks;
		}
	}
	else {
		INTREQ = false;
		code = 0xFF;
		INTE = false;
		HLT = false;
	}

	switch (code)
	{
	case 0x00:
	case 0x08:
	case 0x10:
	case 0x18:
	case 0x20:
	case 0x28:
	case 0x30:
	case 0x38: //{nop}
		NumTicks = 4;
		break;
	case 0x01: //{lxi b,d16}
		mRes = memory->getWord(pc, &bc.r16);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x02: //{stax b}
		mRes = memory->setByte(bc.r16, a);
		NumTicks = 7;
		break;
	case 0x03: //{inx b}
		bc.r16++;
		NumTicks = 5;
		break;
	case 0x04: //{inr b}
		bc.r8.hi = inrdcr(bc.r8.hi, 0);
		NumTicks = 5;
		break;
	case 0x05: //{dcr b}
		bc.r8.hi = inrdcr(bc.r8.hi, 1);
		NumTicks = 5;
		break;
	case 0x06: //{mvi b, d8}
		mRes = memory->getByte(pc++, &bc.r8.hi);
		NumTicks = 7;
		break;
	case 0x07: // {rlc}
		olda = a & 0x80;
		a <<= 1;
		if (olda == 0x80)
		{
			a |= 0x01;
			fl |= 0x01;
		}
		else fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0x09: //{dad b}
		if ((0xFFFF - hl.r16) < bc.r16) fl |= 0x01; else fl &= 0xFE;
		hl.r16 += bc.r16;
		NumTicks = 10;
		break;
	case 0x0A: // {ldax b}
		mRes = memory->getByte(bc.r16, &a);
		NumTicks = 7;
		break;
	case 0x0B: // {dcx b}
		bc.r16--;
		NumTicks = 5;
		break;
	case 0x0C: // {inr c}
		bc.r8.lo = inrdcr(bc.r8.lo, 0);
		NumTicks = 5;
		break;
	case 0x0D: //{dcr c}
		bc.r8.lo = inrdcr(bc.r8.lo, 1);
		NumTicks = 5;
		break;
	case 0x0E: // {mvi c, d8}
		mRes = memory->getByte(pc++, &bc.r8.lo);
		NumTicks = 7;
		break;
	case 0x0F: // {rrc}
		olda = a & 0x01;
		a = a >> 1;
		if (olda == 0x01)
		{
			a |= 0x80;
			fl |= 0x01;
		}
		else fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0x11: // {lxi d,d16}
		mRes = memory->getWord(pc, &de.r16);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x12: // {stax d}
		mRes = memory->setByte(de.r16, a);
		NumTicks = 7;
		break;
	case 0x13: // {inx d}
		de.r16++;
		NumTicks = 5;
		break;
	case 0x14: // {inr d}
		de.r8.hi = inrdcr(de.r8.hi, 0);
		NumTicks = 5;
		break;
	case 0x15: // {dcr d}
		de.r8.hi = inrdcr(de.r8.hi, 1);
		NumTicks = 5;
		break;
	case 0x16: // {mvi d}
		mRes = memory->getByte(pc++, &de.r8.hi);
		NumTicks = 7;
		break;
	case 0x17: // {ral}
		olda = a & 0x80;
		dopcode = fl & 0x01;
		a = (a << 1) | dopcode;
		if (olda == 0x80) fl |= 0x01; else fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0x19: // {dad d}
		if ((0xFFFF - hl.r16) < de.r16) fl |= 0x01; else fl &= 0xFE;
		hl.r16 += de.r16;
		NumTicks = 10;
		break;
	case 0x1A: // {ldax d}
		mRes = memory->getByte(de.r16, &a);
		NumTicks = 7;
		break;
	case 0x1B: // {dcx d}
		de.r16--;
		NumTicks = 5;
		break;
	case 0x1C: // {inr e}
		de.r8.lo = inrdcr(de.r8.lo, 0);
		NumTicks = 5;
		break;
	case 0x1D: // {dcr e}
		de.r8.lo = inrdcr(de.r8.lo, 1);
		NumTicks = 5;
		break;
	case 0x1E: // {mvi e}
		mRes = memory->getByte(pc++, &de.r8.lo);
		NumTicks = 7;
		break;
	case 0x1F: // {rar}
		olda = a & 0x01;
		dopcode = fl & 0x01;
		dopcode <<= 7;
		a = dopcode | (a >> 1);
		if (olda == 0x01) fl |= 0x01; else fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0x21: // {lxi h,d16}
		mRes = memory->getWord(pc, &hl.r16);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x22: // {shld addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setWord(addr, hl.r16);
		NumTicks = 16;
		break;
	case 0x23: // {inx h}
		hl.r16++;
		NumTicks = 5;
		break;
	case 0x24: // {inr h}
		hl.r8.hi = inrdcr(hl.r8.hi, 0);
		NumTicks = 5;
		break;
	case 0x25: /// {dcr h}
		hl.r8.hi = inrdcr(hl.r8.hi, 1);
		NumTicks = 5;
		break;
	case 0x26: // {mvi h}
		mRes = memory->getByte(pc++, &hl.r8.hi);
		NumTicks = 7;
		break;
	case 0x27: // {daa}
	{
		uint8_t temp = 0;
		uint8_t tmpC = (fl & 0x01);
		if (((a & 0x0F) > 9) || ((fl & 0x10) == 0x10))
		{
			temp = 0x06;
		}
		if (((a >> 4) > 9) || ((fl & 0x01) == 0x01) || (((a >> 4) >= 9) && ((a & 0x0F) > 9)))
		{
			temp |= 0x60;
			tmpC = 1;
		}
		summ(temp, 0, 0);
		if (tmpC) fl |= 0x01; else fl &= 0xFE;
		if (pTable[a] == 1) fl |= 0x04; else fl &= 0xFB;
		NumTicks = 4;
	}
	break;
	case 0x29: // {dad h}
		if ((0xFFFF - hl.r16) < hl.r16) fl |= 0x01; else fl &= 0xFE;
		hl.r16 <<= 1;
		NumTicks = 10;
		break;
	case 0x2A: // {lhld addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getWord(addr, &hl.r16);
		NumTicks = 16;
		break;
	case 0x2B: // {dcx h}
		hl.r16--;
		NumTicks = 5;
		break;
	case 0x2C: // {inr l}
		hl.r8.lo = inrdcr(hl.r8.lo, 0);
		NumTicks = 5;
		break;
	case 0x2D: // {dcr l}
		hl.r8.lo = inrdcr(hl.r8.lo, 1);
		NumTicks = 5;
		break;
	case 0x2E: // {mvi l}
		mRes = memory->getByte(pc++, &hl.r8.lo);
		NumTicks = 7;
		break;
	case 0x2F: // {cma}
		a ^= 0xff;
		NumTicks = 4;
		break;
	case 0x31: // {lxi sp,d16}
		mRes = memory->getWord(pc, &sp);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x32: // {sta addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setByte(addr, a);
		NumTicks = 13;
		break;
	case 0x33: // {inx sp}
		sp++;
		NumTicks = 5;
		break;
	case 0x34: // {inr m}
		mRes = memory->getByte(hl.r16, &dopcode);
		dopcode = inrdcr(dopcode, 0);
		mRes = memory->setByte(hl.r16, dopcode);
		NumTicks = 10;
		break;
	case 0x35: // {dcr m}
		mRes = memory->getByte(hl.r16, &dopcode);
		dopcode = inrdcr(dopcode, 1);
		mRes = memory->setByte(hl.r16, dopcode);
		NumTicks = 10;
		break;
	case 0x36: // {mvi m}
		mRes = memory->getByte(pc++, &dopcode);
		mRes = memory->setByte(hl.r16, dopcode);
		NumTicks = 10;
		break;
	case 0x37: // {stc}
		fl |= 0x01;
		NumTicks = 4;
		break;
	case 0x39: // {dad sp}
		if ((0xFFFF - hl.r16) < sp) fl |= 0x01; else fl &= 0xFE;
		hl.r16 += sp;
		NumTicks = 10;
		break;
	case 0x3A: // {lda addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getByte(addr, &a);
		NumTicks = 13;
		break;
	case 0x3B: // {dcx sp}
		sp--;
		NumTicks = 5;
		break;
	case 0x3C: // {inr a}
		a = inrdcr(a, 0);
		NumTicks = 5;
		break;
	case 0x3D: // {dcr a}
		a = inrdcr(a, 1);
		NumTicks = 5;
		break;
	case 0x3E: // {mvi a,b1}
		mRes = memory->getByte(pc++, &a);
		NumTicks = 7;
		break;
	case 0x3F: // {cmc}
		fl ^= 0x01;
		NumTicks = 4;
		break;
	case 0x40: // {mov b,b}
		NumTicks = 5;
		break;
	case 0x41: // {mov b,c}
		NumTicks = 5;
		break;
	case 0x42: // {mov b,d}
		bc.r8.hi = de.r8.hi;
		NumTicks = 5;
		break;
	case 0x43: // {mov b,e}
		bc.r8.hi = de.r8.lo;
		NumTicks = 5;
		break;
	case 0x44: // {mov b,h}
		bc.r8.hi = hl.r8.hi;
		NumTicks = 5;
		break;
	case 0x45: // {mov b,l}
		bc.r8.hi = hl.r8.lo;
		NumTicks = 5;
		break;
	case 0x46: // {mov b,m}
		mRes = memory->getByte(hl.r16, &bc.r8.hi);
		NumTicks = 7;
		break;
	case 0x47: // {mov b,a}
		bc.r8.hi = a;
		NumTicks = 5;
		break;
	case 0x48: // {mov c,b}
		bc.r8.lo = bc.r8.hi;
		NumTicks = 5;
		break;
	case 0x49: // {mov c,c}
		NumTicks = 5;
		break;
	case 0x4A: // {mov c,d}
		bc.r8.lo = de.r8.hi;
		NumTicks = 5;
		break;
	case 0x4B: // {mov c,e}
		bc.r8.lo = de.r8.lo;
		NumTicks = 5;
		break;
	case 0x4C: // {mov c,h}
		bc.r8.lo = hl.r8.hi;
		NumTicks = 5;
		break;
	case 0x4D: // {mov c,l}
		bc.r8.lo = hl.r8.lo;
		NumTicks = 5;
		break;
	case 0x4E: // {mov c,m}
		mRes = memory->getByte(hl.r16, &bc.r8.lo);
		NumTicks = 7;
		break;
	case 0x4F: // {mov c,a}
		bc.r8.lo = a;
		NumTicks = 5;
		break;
	case 0x50: // {mov d,b}
		de.r8.hi = bc.r8.hi;
		NumTicks = 5;
		break;
	case 0x51: // {mov d,c}
		de.r8.hi = bc.r8.lo;
		NumTicks = 5;
		break;
	case 0x52: // {mov d,d}
		NumTicks = 5;
		break;
	case 0x53: // {mov d,e}
		de.r8.hi = de.r8.lo;
		NumTicks = 5;
		break;
	case 0x54: // {mov d,h}
		de.r8.hi = hl.r8.hi;
		NumTicks = 5;
		break;
	case 0x55: // {mov d,l}
		de.r8.hi = hl.r8.lo;
		NumTicks = 5;
		break;
	case 0x56: // {mov d,m}
		mRes = memory->getByte(hl.r16, &de.r8.hi);
		NumTicks = 7;
		break;
	case 0x57: // {mov d,a}
		de.r8.hi = a;
		NumTicks = 5;
		break;
	case 0x58: // {mov e,b}
		de.r8.lo = bc.r8.hi;
		NumTicks = 5;
		break;
	case 0x59: // {mov e,c}
		de.r8.lo = bc.r8.lo;
		NumTicks = 5;
		break;
	case 0x5A: // {mov e,d}
		de.r8.lo = de.r8.hi;
		NumTicks = 5;
		break;
	case 0x5B: // {mov e,e}
		NumTicks = 5;
		break;
	case 0x5C: // {mov e,h}
		de.r8.lo = hl.r8.hi;
		NumTicks = 5;
		break;
	case 0x5D: // {mov e,l}
		de.r8.lo = hl.r8.lo;
		NumTicks = 5;
		break;
	case 0x5E: // {mov e,m}
		mRes = memory->getByte(hl.r16, &de.r8.lo);
		NumTicks = 7;
		break;
	case 0x5F: // {mov e,a}
		de.r8.lo = a;
		NumTicks = 5;
		break;
	case 0x60: // {mov h,b}
		hl.r8.hi = bc.r8.hi;
		NumTicks = 5;
		break;
	case 0x61: // {mov h,c}
		hl.r8.hi = bc.r8.lo;
		NumTicks = 5;
		break;
	case 0x62: // {mov h,d}
		hl.r8.hi = de.r8.hi;
		NumTicks = 5;
		break;
	case 0x63: // {mov h,e}
		hl.r8.hi = de.r8.lo;
		NumTicks = 5;
		break;
	case 0x64: // {mov h,h}
		NumTicks = 5;
		break;
	case 0x65: // {mov h,l}
		hl.r8.hi = hl.r8.lo;
		NumTicks = 5;
		break;
	case 0x66: // {mov h,m}
		mRes = memory->getByte(hl.r16, &hl.r8.hi);
		NumTicks = 7;
		break;
	case 0x67: // {mov h,a}
		hl.r8.hi = a;
		NumTicks = 5;
		break;
	case 0x68: // {mov l,b}
		hl.r8.lo = bc.r8.hi;
		NumTicks = 5;
		break;
	case 0x69: // {mov l,c}
		hl.r8.lo = bc.r8.lo;
		NumTicks = 5;
		break;
	case 0x6A: // {mov l,d}
		hl.r8.lo = de.r8.hi;
		NumTicks = 5;
		break;
	case 0x6B: // {mov l,e}
		hl.r8.lo = de.r8.lo;
		NumTicks = 5;
		break;
	case 0x6C: // {mov l,h}
		hl.r8.lo = hl.r8.hi;
		NumTicks = 5;
		break;
	case 0x6D: // {mov l,l}
		NumTicks = 5;
		break;
	case 0x6E: // {mov l,m}
		mRes = memory->getByte(hl.r16, &hl.r8.lo);
		NumTicks = 7;
		break;
	case 0x6F: // {mov l,a}
		hl.r8.lo = a;
		NumTicks = 5;
		break;
	case 0x70: // {mov m,b}
		mRes = memory->setByte(hl.r16, bc.r8.hi);
		NumTicks = 7;
		break;
	case 0x71: // {mov m,c}
		mRes = memory->setByte(hl.r16, bc.r8.lo);
		NumTicks = 7;
		break;
	case 0x72: // {mov m,d}
		mRes = memory->setByte(hl.r16, de.r8.hi);
		NumTicks = 7;
		break;
	case 0x73: // {mov m,e}
		mRes = memory->setByte(hl.r16, de.r8.lo);
		NumTicks = 7;
		break;
	case 0x74: // {mov m,h}
		mRes = memory->setByte(hl.r16, hl.r8.hi);
		NumTicks = 7;
		break;
	case 0x75: // {mov m,l}
		mRes = memory->setByte(hl.r16, hl.r8.lo);
		NumTicks = 7;
		break;
	case 0x76: // {hlt}
		HLT = true;
		NumTicks = 7;
		break;
	case 0x77: // {mov m,a}
		mRes = memory->setByte(hl.r16, a);
		NumTicks = 7;
		break;
	case 0x78: // {mov a,b}
		a = bc.r8.hi;
		NumTicks = 5;
		break;
	case 0x79: // {mov a,c}
		a = bc.r8.lo;
		NumTicks = 5;
		break;
	case 0x7A: // {mov a,d}
		a = de.r8.hi;
		NumTicks = 5;
		break;
	case 0x7B: // {mov a,e}
		a = de.r8.lo;
		NumTicks = 5;
		break;
	case 0x7C: // {mov a,h}
		a = hl.r8.hi;
		NumTicks = 5;
		break;
	case 0x7D: // {mov a,l}
		a = hl.r8.lo;
		NumTicks = 5;
		break;
	case 0x7E: // {mov a,m}
		mRes = memory->getByte(hl.r16, &a);
		NumTicks = 7;
		break;
	case 0x7F: // {mov a,a}
		NumTicks = 5;
		break;
	case 0x80: // {add b}
		summ(bc.r8.hi, 0, 0);
		NumTicks = 4;
		break;
	case 0x81: // {add c}
		summ(bc.r8.lo, 0, 0);
		NumTicks = 4;
		break;
	case 0x82: // {add d}
		summ(de.r8.hi, 0, 0);
		NumTicks = 4;
		break;
	case 0x83: // {add e}
		summ(de.r8.lo, 0, 0);
		NumTicks = 4;
		break;
	case 0x84: // {add h}
		summ(hl.r8.hi, 0, 0);
		NumTicks = 4;
		break;
	case 0x85: // {add l}
		summ(hl.r8.lo, 0, 0);
		NumTicks = 4;
		break;
	case 0x86: // {add m}
		uint8_t tmp;
		mRes = memory->getByte(hl.r16, &tmp);
		summ(tmp, 0, 0);
		NumTicks = 7;
		break;
	case 0x87: // {add a}
		summ(a, 0, 0);
		NumTicks = 4;
		break;
	case 0x88: // {adc b}
		summ(bc.r8.hi, fl & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x89: // {adc c}
		summ(bc.r8.lo, fl & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8A: // {adc d}
		summ(de.r8.hi, fl & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8B: // {adc e}
		summ(de.r8.lo, fl & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8C: // {adc h}
		summ(hl.r8.hi, fl & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8D: // {adc l}
		summ(hl.r8.lo, fl & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8E: // {adc m}
		mRes = memory->getByte(hl.r16, &b1);
		summ(b1, fl & 0x01, 0);
		NumTicks = 7;
		break;
	case 0x8F: // {adc a}
		summ(a, fl & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x90: // {sub b}
		summ(bc.r8.hi, 0, 1);
		NumTicks = 4;
		break;
	case 0x91: // {sub c}
		summ(bc.r8.lo, 0, 1);
		NumTicks = 4;
		break;
	case 0x92: // {sub d}
		summ(de.r8.hi, 0, 1);
		NumTicks = 4;
		break;
	case 0x93: // {sub e}
		summ(de.r8.lo, 0, 1);
		NumTicks = 4;
		break;
	case 0x94: // {sub h}
		summ(hl.r8.hi, 0, 1);
		NumTicks = 4;
		break;
	case 0x95: // {sub l}
		summ(hl.r8.lo, 0, 1);
		NumTicks = 4;
		break;
	case 0x96: // {sub m}
		mRes = memory->getByte(hl.r16, &b1);
		summ(b1, 0, 1);
		NumTicks = 7;
		break;
	case 0x97: // {sub a}
		summ(a, 0, 1);
		NumTicks = 4;
		break;
	case 0x98: // {sbb b}
		summ(bc.r8.hi, fl & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x99: // {sbb c}
		summ(bc.r8.lo, fl & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9A: // {sbb d}
		summ(de.r8.hi, fl & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9B: // {sbb e}
		summ(de.r8.lo, fl & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9C: // {sbb h}
		summ(hl.r8.hi, fl & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9D: // {sbb l}
		summ(hl.r8.lo, fl & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9E: // {sbb m}
		mRes = memory->getByte(hl.r16, &b1);
		summ(b1, fl & 0x01, 1);
		NumTicks = 7;
		break;
	case 0x9F: // {sbb a}
		summ(a, fl & 0x01, 1);
		NumTicks = 4;
		break;
	case 0xA0: // {ana b}
		olda = a | bc.r8.hi;
		olda >>= 3;
		a &= bc.r8.hi;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0xA1: // {ana c}
		olda = a | bc.r8.lo;
		olda >>= 3;
		a &= bc.r8.lo;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0xA2: // {ana d}
		olda = a | de.r8.hi;
		olda >>= 3;
		a &= de.r8.hi;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0xA3: // {ana e}
		olda = a | de.r8.lo;
		olda >>= 3;
		a &= de.r8.lo;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0xA4: // {ana h}
		olda = a | hl.r8.hi;
		olda >>= 3;
		a &= hl.r8.hi;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0xA5: // {ana l}
		olda = a | hl.r8.lo;
		olda >>= 3;
		a &= hl.r8.lo;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0xA6: // {ana m}
		mRes = memory->getByte(hl.r16, &b1);
		olda = a | b1;
		olda >>= 3;
		a &= b1;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 7;
		break;
	case 0xA7: // {ana a}
		olda = a | a;
		olda >>= 3;
		a &= a;
		setfl();
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		fl &= 0xFE;
		NumTicks = 4;
		break;
	case 0xA8: // {xra b}
		a ^= bc.r8.hi;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xA9: // {xra c}
		a ^= bc.r8.lo;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xAA: // {xra d}
		a ^= de.r8.hi;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xAB: // {xra e}
		a ^= de.r8.lo;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xAC: // {xra h}
		a ^= hl.r8.hi;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xAD: // {xra l}
		a ^= hl.r8.lo;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xAE: // {xra m}
		mRes = memory->getByte(hl.r16, &b1);
		a ^= b1;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 7;
		break;
	case 0xAF: // {xra a}
		a ^= a;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB0: // {ora b}
		a |= bc.r8.hi;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB1: // {ora c}
		a |= bc.r8.lo;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB2: // {ora d}
		a |= de.r8.hi;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB3: // {ora e}
		a |= de.r8.lo;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB4: // {ora h}
		a |= hl.r8.hi;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB5: // {ora l}
		a |= hl.r8.lo;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB6: // {ora m}
		mRes = memory->getByte(hl.r16, &b1);
		a |= b1;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 7;
		break;
	case 0xB7: // {ora a}
		a |= a;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 4;
		break;
	case 0xB8: // {cmp b}
		olda = a;
		summ(bc.r8.hi, 0, 1);
		a = olda;
		NumTicks = 4;
		break;
	case 0xB9: // {cmp c}
		olda = a;
		summ(bc.r8.lo, 0, 1);
		a = olda;
		NumTicks = 4;
		break;
	case 0xBA: // {cmp d}
		olda = a;
		summ(de.r8.hi, 0, 1);
		a = olda;
		NumTicks = 4;
		break;
	case 0xBB: // {cmp e}
		olda = a;
		summ(de.r8.lo, 0, 1);
		a = olda;
		NumTicks = 4;
		break;
	case 0xBC: // {cmp h}
		olda = a;
		summ(hl.r8.hi, 0, 1);
		a = olda;
		NumTicks = 4;
		break;
	case 0xBD: // {cmp l}
		olda = a;
		summ(hl.r8.lo, 0, 1);
		a = olda;
		NumTicks = 4;
		break;
	case 0xBE: // {cmp m}
		olda = a;
		mRes = memory->getByte(hl.r16, &dopcode);
		summ(dopcode, 0, 1);
		a = olda;
		NumTicks = 7;
		break;
	case 0xBF: // {cmp a}
		olda = a;
		summ(a, 0, 1);
		a = olda;
		NumTicks = 4;
		break;
	case 0xC0: // {rnz}
		if (((fl & 0x40) >> 6) == 0)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xC1: // {pop b}
		mRes = memory->getWordFromCurrentBank(sp, &bc.r16);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xC2: // {jnz addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x40) >> 6) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xC3:
	case 0xCB: // {jmp addr}
		mRes = memory->getWord(pc, &pc);
		NumTicks = 10;
		break;
	case 0xC4: // {cnz addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x40) >> 6) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xC5: // {push b}
		sp -= 2;
		mRes = memory->setWordToCurrentBank(sp, bc.r16);
		NumTicks = 11;
		break;
	case 0xC6: // {adi}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, 0, 0);
		NumTicks = 7;
		break;
	case 0xC7: // {rst 0}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 0;
		NumTicks = 11;
		break;
	case 0xC8: // {rz}
		if (((fl & 0x40) >> 6) == 1)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xC9:
	case 0xD9: // {ret}
		mRes = memory->getWordFromCurrentBank(sp, &pc);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xCA: // {jz addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x40) >> 6) == 1) pc = addr;
		NumTicks = 10;
		break;
	case 0xCC: // {cz addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x40) >> 6) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xCD:
	case 0xDD:
	case 0xED:
	case 0xFD: // {call addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		sp -= 2;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = addr;
		NumTicks = 17;
		break;
	case 0xCE: // {aci}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, fl & 0x01, 0);
		NumTicks = 7;
		break;
	case 0xCF: // {rst 1}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 8;
		NumTicks = 11;
		break;
	case 0xD0: // {rnc}
		if ((fl & 0x01) == 0)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xD1: // {pop d}
		mRes = memory->getWordFromCurrentBank(sp, &de.r16);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xD2: // {jnc addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if ((fl & 0x01) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xD3: // {out b1}
		mRes = memory->getByte(pc++, &b1);
		ports_out[b1] = a;
		if (prt[b1] != NULL) prt[b1]->setPortData(b1, a);
		NumTicks = 10;
		break;
	case 0xD4: // {cnc addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if ((fl & 0x01) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xD5: // {push d}
		sp -= 2;
		mRes = memory->setWordToCurrentBank(sp, de.r16);
		NumTicks = 11;
		break;
	case 0xD6: // {sui}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, 0, 1);
		NumTicks = 7;
		break;
	case 0xD7: // {rst 2}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 16;
		NumTicks = 11;
		break;
	case 0xD8: // {rc}
		if ((fl & 0x01) == 1)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xDA: // {jc addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if ((fl & 0x01) == 1) pc = addr;
		NumTicks = 10;
		break;
	case 0xDB: // {in port}
		mRes = memory->getByte(pc++, &b1);
		a = ports_in[b1];
		if (prt[b1] != NULL) a = prt[b1]->getPortData(b1);
		NumTicks = 10;
		break;
	case 0xDC: // {cc addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if ((fl & 0x01) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xDE: // {sbi}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, fl & 0x01, 1);
		NumTicks = 7;
		break;
	case 0xDF: // {rst 3}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 24;
		NumTicks = 11;
		break;
	case 0xE0: // {rpo}
		if (((fl & 0x04) >> 2) == 0)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xE1: // {pop h}
		mRes = memory->getWordFromCurrentBank(sp, &hl.r16);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xE2: // {jpo addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x04) >> 2) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xE3: // {xthl}
		mRes = memory->getWordFromCurrentBank(sp, &addr);
		mRes = memory->setWordToCurrentBank(sp, hl.r16);
		hl.r16 = addr;
		NumTicks = 18;
		break;
	case 0xE4: // {cpo addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x04) >> 2) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xE5: // {push h}
		sp -= 2;
		mRes = memory->setWordToCurrentBank(sp, hl.r16);
		NumTicks = 11;
		break;
	case 0xE6: // {ani}
		mRes = memory->getByte(pc++, &b1);
		olda = a | b1;
		olda >>= 3;
		a &= b1;
		setfl();
		fl &= 0xFE;
		if ((olda & 0x01) == 1) fl |= 0x10; else fl &= 0xEF;
		NumTicks = 7;
		break;
	case 0xE7: // {rst 4}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 32;
		NumTicks = 11;
		break;
	case 0xE8: // {rpe}
		if (((fl & 0x04) >> 2) == 1)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xE9: // {pchl}
		pc = hl.r16;
		NumTicks = 5;
		break;
	case 0xEA: // {jpe addr}
		mRes = memory->getWord(pc, &addr);
		if (((fl & 0x04) >> 2) == 1) pc = addr;
		NumTicks = 10;
		break;
	case 0xEB: // {xchg}
		addr = hl.r16;
		hl.r16 = de.r16;
		de.r16 = addr;
		NumTicks = 4;
		break;
	case 0xEC: // {cpe addr}
		mRes = memory->getWord(pc, &addr);
		if (((fl & 0x04) >> 2) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xEE: // {xri}
		mRes = memory->getByte(pc++, &b1);
		a ^= b1;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 7;
		break;
	case 0xEF: // {rst 5}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 40;
		NumTicks = 11;
		break;
	case 0xF0: // {rp}
		if (((fl & 0x80) >> 7) == 0)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xF1: // {pop psw}
		mRes = memory->getWordFromCurrentBank(sp, &afl);
		sp += 2;
		a = afl & 0xFF;
		fl = afl >> 8;
		fl &= 0xD7;
		fl |= 0x02;
		NumTicks = 10;
		break;
	case 0xF2: // {jp addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x80) >> 7) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xF3: // {di}
		INTE = false;
		NumTicks = 4;
		break;
	case 0xF4: // {cp addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x80) >> 7) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xF5: // {push psw}
		sp -= 2;
		afl = fl;
		afl <<= 8;
		afl |= a;
		mRes = memory->setWordToCurrentBank(sp, afl);
		NumTicks = 11;
		break;
	case 0xF6: // {ori}
		mRes = memory->getByte(pc++, &b1);
		a |= b1;
		setfl();
		fl &= 0xFE;
		fl &= 0xEF;
		NumTicks = 7;
		break;
	case 0xF7: // {rst 6}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 48;
		NumTicks = 11;
		break;
	case 0xF8: // {rm}
		if (((fl & 0x80) >> 7) == 1)
		{
			mRes = memory->getWordFromCurrentBank(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xF9: // {sphl}
		sp = hl.r16;
		NumTicks = 5;
		break;
	case 0xFA: // {jm addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x80) >> 7) == 1) pc = addr;
		NumTicks = 10;
		break;
	case 0xFB: // {ei}
		INTE = true;
		NumTicks = 4;
		break;
	case 0xFC: // {cm addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (((fl & 0x80) >> 7) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToCurrentBank(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 11;
		break;
	case 0xFE: // {cpi}
		mRes = memory->getByte(pc++, &b1);
		olda = a;
		summ(b1, 0, 1);
		a = olda;
		NumTicks = 7;
		break;
	case 0xFF: // {rst 7}
		sp -= 2;;
		mRes = memory->setWordToCurrentBank(sp, pc);
		pc = 56;
		NumTicks = 11;
		break;
	}
	
	if (mRes) {
		return 0;
	}
	return NumTicks;
}

bool i8080::isHalted() {
	return HLT;
}

string i8080::dasm(uint16_t *addr)
{
	string s = "";
	uint8_t code, b1;
	uint16_t w1;

	bool mRes = memory->getByte(*addr, &code);
	if (mRes) {
		return s;
	}
	(*addr)++;

	switch (code)
	{
	case 0x00:
	case 0x08:
	case 0x10:
	case 0x18:
	case 0x20:
	case 0x28:
	case 0x30:
	case 0x38:			//{ nop }
		s = s + "NOP";
		break;
	case 0x01:			 //{ lxi b,d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LXI B," + hexaddr(w1);
		break;
	case 0x02:			 //{ stax b }
		s = s + "STAX B";
		break;
	case 0x03:			 //{ inx b }
		s = s + "INX B";
		break;
	case 0x04:			 //{ inr b }
		s = s + "INR B";
		break;
	case 0x05:			 //{ dcr b }
		s = s + "DCR B";
		break;
	case 0x06:			 //{ mvi b }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI B," + hex(b1);
		break;
	case 0x07:			//{ rlc }
		s = s + "RLC";
		break;
	case 0x09:			//{ dad b }
		s = s + "DAD B";
		break;
	case 0x0A:			//{ ldax b }
		s = s + "LDAX B";
		break;
	case 0x0B:			//{ dcx b }
		s = s + "DCX B";
		break;
	case 0x0C:			//{ inr c }
		s = s + "INR C";
		break;
	case 0x0D:			//{ dcr c }
		s = s + "DCR C";
		break;
	case 0x0E:			//{ mvi c }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI C," + hex(b1);
		break;
	case 0x0F:			//{ rrc }
		s = s + "RRC";
		break;
	case 0x11: //{ lxi d,d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LXI D," + hexaddr(w1);
		break;
	case 0x12: //{ stax d }
		s = s + "STAX D";
		break;
	case 0x13: //{ inx d }
		s = s + "INX D";
		break;
	case 0x14: //{ inr d }
		s = s + "INR D";
		break;
	case 0x15: //{ dcr d }
		s = s + "DCR D";
		break;
	case 0x16: //{ mvi d }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI D," + hex(b1);
		break;
	case 0x17: //{ ral }
		s = s + "RAL";
		break;
	case 0x19: //{ dad d }
		s = s + "DAD D";
		break;
	case 0x1A: //{ ldax d }
		s = s + "LDAX D";
		break;
	case 0x1B: //{ dcx d }
		s = s + "DCX D";
		break;
	case 0x1C: //{ inr e }
		s = s + "INR E";
		break;
	case 0x1D: //{ dcr e }
		s = s + "DCR E";
		break;
	case 0x1E: //{ mvi e }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI E," + hex(b1);
		break;
	case 0x1F: //{ rar }
		s = s + "RAR";
		break;
	case 0x21: //{ lxi h,d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LXI H," + hexaddr(w1);
		break;
	case 0x22: //{ shld addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "SHLD " + hexaddr(w1);
		break;
	case 0x23: //{ inx h }
		s = s + "INX H";
		break;
	case 0x24: //{ inr h }
		s = s + "INR H";
		break;
	case 0x25: //{ dcr h }
		s = s + "DCR H";
		break;
	case 0x26: //{ mvi h }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI H," + hex(b1);
		break;
	case 0x27: //{ daa }
		s = s + "DAA";
		break;
	case 0x29: //{ dad h }
		s = s + "DAD H";
		break;
	case 0x2A: //{ lhld addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LHLD " + hexaddr(w1);
		break;
	case 0x2B: //{ dcx h }
		s = s + "DCX H";
		break;
	case 0x2C: //{ inr l }
		s = s + "INR L";
		break;
	case 0x2D: //{ dcr l }
		s = s + "DCR L";
		break;
	case 0x2E: //{ mvi l }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI L," + hex(b1);
		break;
	case 0x2F: //{ cma }
		s = s + "CMA";
		break;
	case 0x31: //{ lxi sp,d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LXI SP," + hexaddr(w1);
		break;
	case 0x32: //{ sta addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "STA " + hexaddr(w1);
		break;
	case 0x33: //{ inx sp }
		s = s + "INX SP";
		break;
	case 0x34: //{ inr m }
		s = s + "INR M";
		break;
	case 0x35: //{ dcr m }
		s = s + "DCR M";
		break;
	case 0x36: //{ mvi m }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI M," + hex(b1);
		break;
	case 0x37: //{ stc }
		s = s + "STC";
		break;
	case 0x39: //{ dad sp }
		s = s + "DAD SP";
		break;
	case 0x3A: //{ lda addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LDA " + hexaddr(w1);
		break;
	case 0x3B: //{ dcx sp }
		s = s + "DCX SP";
		break;
	case 0x3C: //{ inr a }
		s = s + "INR A";
		break;
	case 0x3D: //{ dcr a }
		s = s + "DCR A";
		break;
	case 0x3E: //{ mvi a,b1 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "MVI A," + hex(b1);
		break;
	case 0x3F: //{ cmc }
		s = s + "CMC";
		break;
	case 0x40: //{ mov b,b }
		s = s + "MOV B,B";
		break;
	case 0x41: //{ mov b,c }
		s = s + "MOV B,C";
		break;
	case 0x42: //{ mov b,d }
		s = s + "MOV B,D";
		break;
	case 0x43: //{ mov b,e }
		s = s + "MOV B,E";
		break;
	case 0x44: //{ mov b,h }
		s = s + "MOV B,H";
		break;
	case 0x45: //{ mov b,l }
		s = s + "MOV B,L";
		break;
	case 0x46: //{ mov b,m }
		s = s + "MOV B,M";
		break;
	case 0x47: //{ mov b,a }
		s = s + "MOV B,A";
		break;
	case 0x48: //{ mov c,b }
		s = s + "MOV C,B";
		break;
	case 0x49: //{ mov c,c }
		s = s + "MOV C,C";
		break;
	case 0x4A: //{ mov c,d }
		s = s + "MOV C,D";
		break;
	case 0x4B: //{ mov c,e }
		s = s + "MOV C,E";
		break;
	case 0x4C: //{ mov c,h }
		s = s + "MOV C,H";
		break;
	case 0x4D: //{ mov c,l }
		s = s + "MOV C,L";
		break;
	case 0x4E: //{ mov c,m }
		s = s + "MOV C,M";
		break;
	case 0x4F: //{ mov c,a }
		s = s + "MOV C,A";
		break;
	case 0x50: //{ mov d,b }
		s = s + "MOV D,B";
		break;
	case 0x51: //{ mov d,c }
		s = s + "MOV D,C";
		break;
	case 0x52: //{ mov d,d }
		s = s + "MOV D,D";
		break;
	case 0x53: //{ mov d,e }
		s = s + "MOV D,E";
		break;
	case 0x54: //{ mov d,h }
		s = s + "MOV D,H";
		break;
	case 0x55: //{ mov d,l }
		s = s + "MOV D,L";
		break;
	case 0x56: //{ mov d,m }
		s = s + "MOV D,M";
		break;
	case 0x57: //{ mov d,a }
		s = s + "MOV D,A";
		break;
	case 0x58: //{ mov e,b }
		s = s + "MOV E,B";
		break;
	case 0x59: //{ mov e,c }
		s = s + "MOV E,C";
		break;
	case 0x5A: //{ mov e,d }
		s = s + "MOV E,D";
		break;
	case 0x5B: //{ mov e,e }
		s = s + "MOV E,E";
		break;
	case 0x5C: //{ mov e,h }
		s = s + "MOV E,H";
		break;
	case 0x5D: //{ mov e,l }
		s = s + "MOV E,L";
		break;
	case 0x5E: //{ mov e,m }
		s = s + "MOV E,M";
		break;
	case 0x5F: //{ mov e,a }
		s = s + "MOV E,A";
		break;
	case 0x60: //{ mov h,b }
		s = s + "MOV H,B";
		break;
	case 0x61: //{ mov h,c }
		s = s + "MOV H,C";
		break;
	case 0x62: //{ mov h,d }
		s = s + "MOV H,D";
		break;
	case 0x63: //{ mov h,e }
		s = s + "MOV H,E";
		break;
	case 0x64: //{ mov h,h }
		s = s + "MOV H,H";
		break;
	case 0x65: //{ mov h,l }
		s = s + "MOV H,L";
		break;
	case 0x66: //{ mov h,m }
		s = s + "MOV H,M";
		break;
	case 0x67: //{ mov h,a }
		s = s + "MOV H,A";
		break;
	case 0x68: //{ mov l,b }
		s = s + "MOV L,B";
		break;
	case 0x69: //{ mov l,c }
		s = s + "MOV L,C";
		break;
	case 0x6A: //{ mov l,d }
		s = s + "MOV L,D";
		break;
	case 0x6B: //{ mov l,e }
		s = s + "MOV L,E";
		break;
	case 0x6C: //{ mov l,h }
		s = s + "MOV L,H";
		break;
	case 0x6D: //{ mov l,l }
		s = s + "MOV L,L";
		break;
	case 0x6E: //{ mov l,m }
		s = s + "MOV L,M";
		break;
	case 0x6F: //{ mov l,a }
		s = s + "MOV L,A";
		break;
	case 0x70: //{ mov m,b }
		s = s + "MOV M,B";
		break;
	case 0x71: //{ mov m,c }
		s = s + "MOV M,C";
		break;
	case 0x72: //{ mov m,d }
		s = s + "MOV M,D";
		break;
	case 0x73: //{ mov m,e }
		s = s + "MOV M,E";
		break;
	case 0x74: //{ mov m,h }
		s = s + "MOV M,H";
		break;
	case 0x75: //{ mov m,l }
		s = s + "MOV M,L";
		break;
	case 0x76: //{ hlt }
		s = s + "HLT";
		break;
	case 0x77: //{ mov m,a }
		s = s + "MOV M,A";
		break;
	case 0x78: //{ mov a,b }
		s = s + "MOV A,B";
		break;
	case 0x79: //{ mov a,c }
		s = s + "MOV A,C";
		break;
	case 0x7A: //{ mov a,d }
		s = s + "MOV A,D";
		break;
	case 0x7B: //{ mov a,e }
		s = s + "MOV A,E";
		break;
	case 0x7C: //{ mov a,h }
		s = s + "MOV A,H";
		break;
	case 0x7D: //{ mov a,l }
		s = s + "MOV A,L";
		break;
	case 0x7E: //{ mov a,m }
		s = s + "MOV A,M";
		break;
	case 0x7F: //{ mov a,a }
		s = s + "MOV A,A";
		break;
	case 0x80: //{ add b }
		s = s + "ADD B";
		break;
	case 0x81: //{ add c }
		s = s + "ADD C";
		break;
	case 0x82: //{ add d }
		s = s + "ADD D";
		break;
	case 0x83: //{ add e }
		s = s + "ADD E";
		break;
	case 0x84: //{ add h }
		s = s + "ADD H";
		break;
	case 0x85: //{ add l }
		s = s + "ADD L";
		break;
	case 0x86: //{ add m }
		s = s + "ADD M";
		break;
	case 0x87: //{ add a }
		s = s + "ADD A";
		break;
	case 0x88: //{ adc b }
		s = s + "ADC B";
		break;
	case 0x89: //{ adc c }
		s = s + "ADC C";
		break;
	case 0x8A: //{ adc d }
		s = s + "ADC D";
		break;
	case 0x8B: //{ adc e }
		s = s + "ADC E";
		break;
	case 0x8C: //{ adc h }
		s = s + "ADC H";
		break;
	case 0x8D: //{ adc l }
		s = s + "ADC L";
		break;
	case 0x8E: //{ adc m }
		s = s + "ADC M";
		break;
	case 0x8F: //{ adc a }
		s = s + "ADC A";
		break;
	case 0x90: //{ sub b }
		s = s + "SUB B";
		break;
	case 0x91: //{ sub c }
		s = s + "SUB C";
		break;
	case 0x92: //{ sub d }
		s = s + "SUB D";
		break;
	case 0x93: //{ sub e }
		s = s + "SUB E";
		break;
	case 0x94: //{ sub h }
		s = s + "SUB H";
		break;
	case 0x95: //{ sub l }
		s = s + "SUB L";
		break;
	case 0x96: //{ sub m }
		s = s + "SUB M";
		break;
	case 0x97: //{ sub a }
		s = s + "SUB A";
		break;
	case 0x98: //{ sbb b }
		s = s + "SBB B";
		break;
	case 0x99: //{ sub c }
		s = s + "SBB C";
		break;
	case 0x9A: //{ sub d }
		s = s + "SBB D";
		break;
	case 0x9B: //{ sub e }
		s = s + "SBB E";
		break;
	case 0x9C: //{ sub h }
		s = s + "SBB H";
		break;
	case 0x9D: //{ sub l }
		s = s + "SBB L";
		break;
	case 0x9E: //{ sub m }
		s = s + "SBB M";
		break;
	case 0x9F: //{ sub a }
		s = s + "SBB A";
		break;
	case 0xA0: //{ ana b }
		s = s + "ANA B";
		break;
	case 0xA1: //{ ana c }
		s = s + "ANA C";
		break;
	case 0xA2: //{ ana d }
		s = s + "ANA D";
		break;
	case 0xA3: //{ ana e }
		s = s + "ANA E";
		break;
	case 0xA4: //{ ana h }
		s = s + "ANA H";
		break;
	case 0xA5: //{ ana l }
		s = s + "ANA L";
		break;
	case 0xA6: //{ ana m }
		s = s + "ANA M";
		break;
	case 0xA7: //{ ana a }
		s = s + "ANA A";
		break;
	case 0xA8: //{ xra b }
		s = s + "XRA B";
		break;
	case 0xA9: //{ xra c }
		s = s + "XRA C";
		break;
	case 0xAA: //{ xra d }
		s = s + "XRA D";
		break;
	case 0xAB: //{ xra e }
		s = s + "XRA E";
		break;
	case 0xAC: //{ xra h }
		s = s + "XRA H";
		break;
	case 0xAD: //{ xra l }
		s = s + "XRA L";
		break;
	case 0xAE: //{ xra m }
		s = s + "XRA M";
		break;
	case 0xAF: //{ xra a }
		s = s + "XRA A";
		break;
	case 0xB0: //{ ora b }
		s = s + "ORA B";
		break;
	case 0xB1: //{ ora c }
		s = s + "ORA C";
		break;
	case 0xB2: //{ ora d }
		s = s + "ORA D";
		break;
	case 0xB3: //{ ora e }
		s = s + "ORA E";
		break;
	case 0xB4: //{ ra h }
		s = s + "ORA H";
		break;
	case 0xB5: //{ ora l }
		s = s + "ORA L";
		break;
	case 0xB6: //{ ora m }
		s = s + "ORA M";
		break;
	case 0xB7: //{ ora a }
		s = s + "ORA A";
		break;
	case 0xB8: //{ cmp b }
		s = s + "CMP B";
		break;
	case 0xB9: //{ cmp c }
		s = s + "CMP C";
		break;
	case 0xBA: //{ cmp d }
		s = s + "CMP D";
		break;
	case 0xBB: //{ cmp e }
		s = s + "CMP E";
		break;
	case 0xBC: //{ cmp h }
		s = s + "CMP H";
		break;
	case 0xBD: //{ cmp l }
		s = s + "CMP L";
		break;
	case 0xBE: //{ cmp m }
		s = s + "CMP M";
		break;
	case 0xBF: //{ cmp a }
		s = s + "CMP A";
		break;
	case 0xC0: //{ rnz }
		s = s + "RNZ";
		break;
	case 0xC1: //{ pop b }
		s = s + "POP B";
		break;
	case 0xC2: //{ jnz addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JNZ " + hexaddr(w1);
		break;
	case 0xC3:
	case 0xCB: //{ jmp addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JMP " + hexaddr(w1);
		break;
	case 0xC4: //{ cnz addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CNZ " + hex(w1);
		break;
	case 0xC5: //{ push b }
		s = s + "PUSH B";
		break;
	case 0xC6: //{ adi }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ADI " + hex(b1);
		break;
	case 0xC7: //{ rst 0 }
		s = s + "RST 0";
		break;
	case 0xC8: //{ rz }
		s = s + "RZ";
		break;
	case 0xC9:
	case 0xD9: //{ ret }
		s = s + "RET";
		break;
	case 0xCA: //{ jz addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JZ " + hexaddr(w1);
		break;
	case 0xCC: //{ cz addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CZ " + hexaddr(w1);
		break;
	case 0xCD:
	case 0xDD:
	case 0xED:
	case 0xFD: //{ call addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL " + hexaddr(w1);
		break;
	case 0xCE: //{ aci }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ACI " + hex(b1);
		break;
	case 0xCF: //{ rst 1 }
		s = s + "RST 1";
		break;
	case 0xD0: //{ rnc }
		s = s + "RNC";
		break;
	case 0xD1: //{ pop d }
		s = s + "POP D";
		break;
	case 0xD2: //{ jnc addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JNC " + hexaddr(w1);
		break;
	case 0xD3: //{ out b1 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "OUT " + hex(b1);
		break;
	case 0xD4: //{ cnc addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CNC " + hexaddr(w1);
		break;
	case 0xD5: //{ push d }
		s = s + "PUSH D";
		break;
	case 0xD6: //{ sui }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "SUI " + hex(b1);
		break;
	case 0xD7: //{ rst 2 }
		s = s + "RST 2";
		break;
	case 0xD8: //{ rc }
		s = s + "RC";
		break;
	case 0xDA: //{ jc addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JC " + hexaddr(w1);
		break;
	case 0xDB: //{ in port }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "IN " + hex(b1);
		break;
	case 0xDC: //{ cc addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CC " + hexaddr(w1);
		break;
	case 0xDE: //{ sbi }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "SBI " + hex(b1);
		break;
	case 0xDF: //{ rst 3 }
		s = s + "RST 3";
		break;
	case 0xE0: //{ rpo }
		s = s + "RPO";
		break;
	case 0xE1: //{ pop h }
		s = s + "POP H";
		break;
	case 0xE2: //{ jpo addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JPO " + hexaddr(w1);
		break;
	case 0xE3: //{ xthl }
		s = s + "XTHL";
		break;
	case 0xE4: //{ cpo addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CPO " + hexaddr(w1);
		break;
	case 0xE5: //{ push h }
		s = s + "PUSH H";
		break;
	case 0xE6: //{ ani }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ANI " + hex(b1);
		break;
	case 0xE7: //{ rst 4 }
		s = s + "RST 4";
		break;
	case 0xE8: //{ rpe }
		s = s + "RPE";
		break;
	case 0xE9: //{ pchl }
		s = s + "PCHL";
		break;
	case 0xEA: //{ jpe addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JPE " + hexaddr(w1);
		break;
	case 0xEB: //{ xchg }
		s = s + "XCHG";
		break;
	case 0xEC: //{ cpe addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CPE " + hexaddr(w1);
		break;
	case 0xEE: //{ xri }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "XRI " + hex(b1);
		break;
	case 0xEF: //{ rst 5 }
		s = s + "RST 5";
		break;
	case 0xF0: //{ rp }
		s = s + "RP";
		break;
	case 0xF1: //{ pop psw }
		s = s + "POP PSW";
		break;
	case 0xF2: //{ jp addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP " + hexaddr(w1);
		break;
	case 0xF3: //{ di }
		s = s + "DI";
		break;
	case 0xF4: //{ cp addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CP " + hexaddr(w1);
		break;
	case 0xF5: //{ push psw }
		s = s + "PUSH PSW";
		break;
	case 0xF6: //{ ori }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ORI " + hex(b1);
		break;
	case 0xF7: //{ rst 6 }
		s = s + "RST 6";
		break;
	case 0xF8: //{ rm }
		s = s + "RM";
		break;
	case 0xF9: //{ sphl }
		s = s + "SPHL";
		break;
	case 0xFA: //{ jm addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JM " + hexaddr(w1);
		break;
	case 0xFB: //{ ei }
		s = s + "EI";
		break;
	case 0xFC: //{ cm addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CM " + hexaddr(w1);
		break;
	case 0xFE: //{ cpi }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "CPI " + hex(b1);
		break;
	case 0xFF: //{ rst 7 }
		s = s + "RST 7";
		break;
	}
	return s;
}

string i8080::disAsm(uint16_t* addr, uint8_t n) {
	string res = "";
	for (int i = 0; i < n; i++) {
		res += hexaddr(*addr);
		res += "  ";
		res += dasm(addr);
		res += "\n";
	}

	return res;
}


void i8080::interruptRequest() {
	if (INTE) {
		INTREQ = true;
		HLT = false;
	}
}

uint16_t i8080::getPC() {
	return pc;
}

void i8080::SetDebugValue(uint16_t tp, uint16_t num, std::string data) {
	uint16_t dta = hexToDec(data);
	if (tp == 1) {
		switch (num) {
		case 0: a = dta;
			break;
		case 1: bc.r8.hi = dta;
			break;
		case 2: bc.r8.lo = dta;
			break;
		case 3: de.r8.hi = dta;
			break;
		case 4: de.r8.lo = dta;
			break;
		case 5: hl.r8.hi = dta;
			break;
		case 6: hl.r8.lo = dta;
			break;
		case 7: bc.r16 = dta;
			break;
		case 8: de.r16 = dta;
			break;
		case 9: hl.r16 = dta;
			break;
		case 10: sp = dta;
			break;
		case 11: pc = dta;
			break;
		case 12: fl = dta;
			break;
		}
	}
	if (tp == 2) {
		switch (num) {
		case 0: if (dta == 1) fl |= (1 << 7); else fl &= (~(1 << 7));
			break;
		case 1: if (dta == 1) fl |= (1 << 6); else fl &= (~(1 << 6));
			break;
		case 2: if (dta == 1) fl |= (1 << 4); else fl &= (~(1 << 4));
			break;
		case 3: if (dta == 1) fl |= (1 << 2); else fl &= (~(1 << 2));
			break;
		case 4: if (dta == 1) fl |= 1; else fl &= 0xFE;
			break;
		}
	}
}

std::string i8080::getParam(std::string s, int* n) {
	std::string res = "";
	int i = *n;
	if (i < s.length()) {
		while (s[i]==' ' || s[i]==',')
		{
			i++;
			if (i == s.length()) break;
		}
		while (i<s.length())
		{
			if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'Z')) res += s[i]; else break;
			i++;
		}
	}
	*n = i;
	return res;
}

uint8_t i8080::GetIdRegsToName(std::string n) {
	if (n == "A") return 7;
	if (n == "B") return 0;
	if (n == "C") return 1;
	if (n == "D") return 2;
	if (n == "E") return 3;
	if (n == "H") return 4;
	if (n == "L") return 5;
	if (n == "M") return 6;
	return 255;
}

uint8_t i8080::GetIdRegsPairToName(std::string n) {
	if (n == "B") return 0x00;
	if (n == "D") return 0x10;
	if (n == "H") return 0x20;
	if (n == "SP") return 0x30;
	if (n == "PSW") return 0x30;
	return 255;
}

void i8080::SetAssemblerValue(std::string addr, std::string value) {
	
	uint16_t dAddr = hexToDec(addr);
	printf("Use address: %d\n", dAddr);

	for (auto& c : value) c = toupper(c);
	printf("new value: %s\n", value.c_str());

	uint8_t paramCount = 1;
	for (int i = 0; i < value.length(); i++) if (value[i] == ' ' || value[i] == ',') paramCount++;
	printf("Params count: %d\n", paramCount);

	std::string cmd = "";
	int i = 0;
	while (i<value.length())
	{
		if (value[i] == ' ') break;
		cmd += value[i];
		i++;
	}
	printf("Instruction: %s\n", cmd.c_str());

	if (cmd == "ADD") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0x80 | rid);
		}
	}
	if (cmd == "ADI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xC6);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "ADC") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0x88 | rid);
		}
	}
	if (cmd == "ACI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xCE);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "ANA") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0xA0 | rid);
		}
	}
	if (cmd == "ANI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xE6);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "CALL") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xCD);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CZ") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xCC);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CNZ") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xC4);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CP") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xF4);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CM") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xFC);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CC") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xDC);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CNC") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xD4);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CPE") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xEC);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CPO") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xE4);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "CMA") {
		memory->setDebugByte(dAddr, 0x2F);
	}
	if (cmd == "CMC") {
		memory->setDebugByte(dAddr, 0x3F);
	}
	if (cmd == "CMP") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0xB8 | rid);
		}
	}
	if (cmd == "CPI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xFE);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "DAA") {
		memory->setDebugByte(dAddr, 0x27);
	}
	if (cmd == "DAD") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsPairToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0x09 | rid);
		}
	}
	if (cmd == "DCR") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		switch (rid)
		{
		case 0:	memory->setDebugByte(dAddr, 0x05);
			break;
		case 1:	memory->setDebugByte(dAddr, 0x0D);
			break;
		case 2:	memory->setDebugByte(dAddr, 0x15);
			break;
		case 3:	memory->setDebugByte(dAddr, 0x1D);
			break;
		case 4:	memory->setDebugByte(dAddr, 0x25);
			break;
		case 5:	memory->setDebugByte(dAddr, 0x2D);
			break;
		case 6:	memory->setDebugByte(dAddr, 0x35);
			break;
		case 7:	memory->setDebugByte(dAddr, 0x3D);
			break;
		default:
			break;
		}
	}
	if (cmd == "DCX") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsPairToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0x0B | rid);
		}
	}
	if (cmd == "DI") {
		memory->setDebugByte(dAddr, 0xF3);
	}
	if (cmd == "EI") {
		memory->setDebugByte(dAddr, 0xFB);
	}
	if (cmd == "HLT") {
		memory->setDebugByte(dAddr, 0x76);
	}
	if (cmd == "IN") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xDB);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "INR") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		switch (rid)
		{
		case 0:	memory->setDebugByte(dAddr, 0x04);
			break;
		case 1:	memory->setDebugByte(dAddr, 0x0C);
			break;
		case 2:	memory->setDebugByte(dAddr, 0x14);
			break;
		case 3:	memory->setDebugByte(dAddr, 0x1C);
			break;
		case 4:	memory->setDebugByte(dAddr, 0x24);
			break;
		case 5:	memory->setDebugByte(dAddr, 0x2C);
			break;
		case 6:	memory->setDebugByte(dAddr, 0x34);
			break;
		case 7:	memory->setDebugByte(dAddr, 0x3C);
			break;
		default:
			break;
		}
	}
	if (cmd == "INX") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsPairToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0x03 | rid);
		}
	}
	if (cmd == "JMP") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xC3);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JZ") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xCA);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JNZ") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xC2);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JP") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xF2);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JM") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xFA);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JC") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xDA);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JNC") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xD2);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JPE") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xEA);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "JPO") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xE2);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "LDA") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0x3A);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "LDAX") {
		std::string p1 = getParam(value, &i);
		if (p1 == "B") memory->setDebugByte(dAddr, 0x0A);
		if (p1 == "D") memory->setDebugByte(dAddr, 0x1A);
	}
	if (cmd == "LHLD") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0x2A);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "LXI") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsPairToName(p1);
		std::string p2 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p2);
		if (rid != 255) {
			if (rid != 0x10) {
				memory->setDebugByte(dAddr, 0x01 | rid);
				memory->setDebugWord(dAddr + 1, dta16);
			}
		}
	}
	if (cmd == "MOV") {
		std::string p1 = getParam(value, &i);
		uint8_t ridd = GetIdRegsToName(p1);
		std::string p2 = getParam(value, &i);
		uint8_t rids = GetIdRegsToName(p2);
		if (ridd != 255 && rids != 255) {
			if (ridd != 6 && rids != 6) {
				memory->setDebugByte(dAddr, 0x40 | (ridd << 3) | (rids));
			}
		}
	}
	if (cmd == "MVI") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		std::string p2 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p2);
		if (rid != 255) {
			switch (rid)
			{
			case 0:	memory->setDebugByte(dAddr, 0x06);
				break;
			case 1:	memory->setDebugByte(dAddr, 0x0E);
				break;
			case 2:	memory->setDebugByte(dAddr, 0x16);
				break;
			case 3:	memory->setDebugByte(dAddr, 0x1E);
				break;
			case 4:	memory->setDebugByte(dAddr, 0x26);
				break;
			case 5:	memory->setDebugByte(dAddr, 0x2E);
				break;
			case 6:	memory->setDebugByte(dAddr, 0x36);
				break;
			case 7:	memory->setDebugByte(dAddr, 0x3E);
				break;
			default:
				break;
			}
			memory->setDebugByte(dAddr + 1, dta8);
		}
	}
	if (cmd == "NOP") {
		memory->setDebugByte(dAddr, 0x00);
	}
	if (cmd == "ORA") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0xB0 | rid);
		}
	}
	if (cmd == "ORI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xF6);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "OUT") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xD3);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "PCHL") {
		memory->setDebugByte(dAddr, 0xE9);
	}
	if (cmd == "POP") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsPairToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0xC1 | rid);
		}
	}
	if (cmd == "PUSH") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsPairToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0xC5 | rid);
		}
	}
	if (cmd == "RAL") {
		memory->setDebugByte(dAddr, 0x17);
	}
	if (cmd == "RAR") {
		memory->setDebugByte(dAddr, 0x1F);
	}
	if (cmd == "RLC") {
		memory->setDebugByte(dAddr, 0x07);
	}
	if (cmd == "RRC") {
		memory->setDebugByte(dAddr, 0x0F);
	}
	if (cmd == "RET") {
		memory->setDebugByte(dAddr, 0xC9);
	}
	if (cmd == "RZ") {
		memory->setDebugByte(dAddr, 0xC8);
	}
	if (cmd == "RNZ") {
		memory->setDebugByte(dAddr, 0xC0);
	}
	if (cmd == "RP") {
		memory->setDebugByte(dAddr, 0xF0);
	}
	if (cmd == "RM") {
		memory->setDebugByte(dAddr, 0xF8);
	}
	if (cmd == "RC") {
		memory->setDebugByte(dAddr, 0xD8);
	}
	if (cmd == "RNC") {
		memory->setDebugByte(dAddr, 0xD0);
	}
	if (cmd == "RPE") {
		memory->setDebugByte(dAddr, 0xE8);
	}
	if (cmd == "RPO") {
		memory->setDebugByte(dAddr, 0xE0);
	}
	if (cmd == "RST") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		switch (dta8)
		{
		case 0:	memory->setDebugByte(dAddr, 0xC7);
			break;
		case 1:	memory->setDebugByte(dAddr, 0xCF);
			break;
		case 2:	memory->setDebugByte(dAddr, 0xD7);
			break;
		case 3:	memory->setDebugByte(dAddr, 0xDF);
			break;
		case 4:	memory->setDebugByte(dAddr, 0xE7);
			break;
		case 5:	memory->setDebugByte(dAddr, 0xEF);
			break;
		case 6:	memory->setDebugByte(dAddr, 0xF7);
			break;
		case 7:	memory->setDebugByte(dAddr, 0xFF);
			break;
		default:
			break;
		}
	}
	if (cmd == "SPHL") {
		memory->setDebugByte(dAddr, 0xF9);
	}
	if (cmd == "SHLD") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0x22);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "STA") {
		std::string p1 = getParam(value, &i);
		uint16_t dta16 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0x32);
		memory->setDebugWord(dAddr + 1, dta16);
	}
	if (cmd == "STAX") {
		std::string p1 = getParam(value, &i);
		if (p1 == "B") memory->setDebugByte(dAddr, 0x02);
		if (p1 == "D") memory->setDebugByte(dAddr, 0x12);
	}
	if (cmd == "STC") {
		memory->setDebugByte(dAddr, 0x37);
	}
	if (cmd == "SUB") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0x90 | rid);
		}
	}
	if (cmd == "SUI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xD6);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "SBB") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0x98 | rid);
		}
	}
	if (cmd == "SBI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xDE);
		memory->setDebugByte(dAddr + 1, dta8);
	}
	if (cmd == "XCHG") {
		memory->setDebugByte(dAddr, 0xEB);
	}
	if (cmd == "XTHL") {
		memory->setDebugByte(dAddr, 0xE3);
	}
	if (cmd == "XRA") {
		std::string p1 = getParam(value, &i);
		uint8_t rid = GetIdRegsToName(p1);
		if (rid != 255) {
			memory->setDebugByte(dAddr, 0xA8 | rid);
		}
	}
	if (cmd == "XRI") {
		std::string p1 = getParam(value, &i);
		uint8_t dta8 = hexToDec(p1);
		memory->setDebugByte(dAddr, 0xEE);
		memory->setDebugByte(dAddr + 1, dta8);
	}
}

void i8080::setPort(uint16_t num, Ports* dev) {
	prt[num] = dev;
}

uint8_t i8080::getPort(uint8_t num) {
	return ports_out[num];
}

uint16_t i8080::getPort(uint16_t num) {
	return 0;
}