#include <iostream>
#include "z80.h"
#include "hex.h"

using namespace std;

#define A af[currentAF].r8.hi
#define F af[currentAF].r8.lo
#define B blockRegs[currentBlock].bc.r8.hi
#define C blockRegs[currentBlock].bc.r8.lo
#define D blockRegs[currentBlock].de.r8.hi
#define E blockRegs[currentBlock].de.r8.lo
#define H blockRegs[currentBlock].hl.r8.hi
#define L blockRegs[currentBlock].hl.r8.lo
#define AF af[currentAF].r16
#define BC blockRegs[currentBlock].bc.r16
#define DE blockRegs[currentBlock].de.r16
#define HL blockRegs[currentBlock].hl.r16
#define I IR.r8.hi
#define R IR.r8.lo


#define fC 0
#define fN 1
#define fPV 2
#define f3 3
#define fH 4
#define f5 5
#define fZ 6
#define fS 7

#define Cf	1
#define Nf	2
#define PVf	4
#define F3f	8
#define Hf	16
#define F5f	32
#define Zf	64
#define Sf	128

#define SET(n) (F |= (1 << (n)))
#define CLEAR(n) (F &= ~(1 << (n)))
#define FLAG(n) ((F >> (n)) & 1)


z80::z80(Memory* mem) {
	memory = mem;
	for (int i = 0; i < 65536; i++)
	{
		ports_in[i] = 0;
		ports_out[i] = 0;
	}
	HLT = false;
	TickCount = 0;
	currentBlock = 0;
	currentAF = 0;
	INTE = false;
	INTREQ = false;
	pc = 0;
	sp = 0;
	IR.r16 = 0;
	IX.r16 = 0;
	IY.r16 = 0;
	IFF1 = false;
	IFF2 = false;
	interruptMode = 0;
	af[0].r16 = 0x02;
	blockRegs[0].bc.r16 = 0;
	blockRegs[0].de.r16 = 0;
	blockRegs[0].hl.r16 = 0;
	af[1].r16 = 0x02;
	blockRegs[1].bc.r16 = 0;
	blockRegs[1].de.r16 = 0;
	blockRegs[1].hl.r16 = 0;
	for (int i = 0; i < 65536; i++) prt[i] = NULL;
}

z80::~z80() {

}

uint8_t z80::getPortData(uint8_t port) {
	return ports_out[port];
}

void z80::setPortData(uint8_t port, uint8_t data) {
	ports_in[port] = data;
}

Registers z80::getRegisters() {
	Registers reg;
	reg.count = 0;
	reg.registers[reg.count].name = "A";
	reg.registers[reg.count].data = (uint32_t)af[0].r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(af[0].r8.hi);
	reg.registers[reg.count].name = "A`";
	reg.registers[reg.count].data = (uint32_t)af[1].r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(af[1].r8.hi);
	reg.registers[reg.count].name = "B";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].bc.r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[0].bc.r8.hi);
	reg.registers[reg.count].name = "B`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].bc.r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[1].bc.r8.hi);
	reg.registers[reg.count].name = "C";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].bc.r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[0].bc.r8.lo);
	reg.registers[reg.count].name = "C`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].bc.r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[1].bc.r8.lo);
	reg.registers[reg.count].name = "D";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].de.r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[0].de.r8.hi);
	reg.registers[reg.count].name = "D`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].de.r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[1].de.r8.hi);
	reg.registers[reg.count].name = "E";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].de.r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[0].de.r8.lo);
	reg.registers[reg.count].name = "E`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].de.r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[1].de.r8.lo);
	reg.registers[reg.count].name = "H";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].hl.r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[0].hl.r8.hi);
	reg.registers[reg.count].name = "H`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].hl.r8.hi;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[1].hl.r8.hi);
	reg.registers[reg.count].name = "L";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].hl.r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[0].hl.r8.lo);
	reg.registers[reg.count].name = "L`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].hl.r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(blockRegs[1].hl.r8.lo);
	reg.registers[reg.count].name = "BC";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].bc.r16;
	reg.registers[reg.count++].strData = decToHexWord(blockRegs[0].bc.r16);
	reg.registers[reg.count].name = "BC`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].bc.r16;
	reg.registers[reg.count++].strData = decToHexWord(blockRegs[1].bc.r16);
	reg.registers[reg.count].name = "DE";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].de.r16;
	reg.registers[reg.count++].strData = decToHexWord(blockRegs[0].de.r16);
	reg.registers[reg.count].name = "DE`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].de.r16;
	reg.registers[reg.count++].strData = decToHexWord(blockRegs[1].de.r16);
	reg.registers[reg.count].name = "HL";
	reg.registers[reg.count].data = (uint32_t)blockRegs[0].hl.r16;
	reg.registers[reg.count++].strData = decToHexWord(blockRegs[0].hl.r16);
	reg.registers[reg.count].name = "HL`";
	reg.registers[reg.count].data = (uint32_t)blockRegs[1].hl.r16;
	reg.registers[reg.count++].strData = decToHexWord(blockRegs[1].hl.r16);
	reg.registers[reg.count].name = "IR";
	reg.registers[reg.count].data = (uint32_t)IR.r16;
	reg.registers[reg.count++].strData = decToHexWord(IR.r16);
	reg.registers[reg.count].name = "IX";
	reg.registers[reg.count].data = (uint32_t)IX.r16;
	reg.registers[reg.count++].strData = decToHexWord(IX.r16);
	reg.registers[reg.count].name = "IY";
	reg.registers[reg.count].data = (uint32_t)IY.r16;
	reg.registers[reg.count++].strData = decToHexWord(IY.r16);
	reg.registers[reg.count].name = "SP";
	reg.registers[reg.count].data = (uint32_t)sp;
	reg.registers[reg.count++].strData = decToHexWord(sp);
	reg.registers[reg.count].name = "PC";
	reg.registers[reg.count].data = (uint32_t)pc;
	reg.registers[reg.count++].strData = decToHexWord(pc);
	reg.registers[reg.count].name = "FLAGS";
	reg.registers[reg.count].data = (uint32_t)af[0].r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(af[0].r8.lo);
	reg.registers[reg.count].name = "FLAGS`";
	reg.registers[reg.count].data = (uint32_t)af[1].r8.lo;
	reg.registers[reg.count++].strData = decToHexByte(af[1].r8.lo);
	reg.flags.count = 0;
	reg.flags.flags[reg.flags.count].name = "S";
	reg.flags.flags[reg.flags.count].data = af[0].r8.lo & 0x80;
	reg.flags.flags[reg.flags.count++].strData = af[0].r8.lo & 0x80 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "Z";
	reg.flags.flags[reg.flags.count].data = af[0].r8.lo & 0x40;
	reg.flags.flags[reg.flags.count++].strData = af[0].r8.lo & 0x40 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "H";
	reg.flags.flags[reg.flags.count].data = af[0].r8.lo & 0x10;
	reg.flags.flags[reg.flags.count++].strData = af[0].r8.lo & 0x10 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "P";
	reg.flags.flags[reg.flags.count].data = af[0].r8.lo & 0x4;
	reg.flags.flags[reg.flags.count++].strData = af[0].r8.lo & 0x4 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "N";
	reg.flags.flags[reg.flags.count].data = af[0].r8.lo & 0x2;
	reg.flags.flags[reg.flags.count++].strData = af[0].r8.lo & 0x2 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "C";
	reg.flags.flags[reg.flags.count].data = af[0].r8.lo & 0x1;
	reg.flags.flags[reg.flags.count++].strData = af[0].r8.lo & 0x1 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "S`";
	reg.flags.flags[reg.flags.count].data = af[1].r8.lo & 0x80;
	reg.flags.flags[reg.flags.count++].strData = af[1].r8.lo & 0x80 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "Z`";
	reg.flags.flags[reg.flags.count].data = af[1].r8.lo & 0x40;
	reg.flags.flags[reg.flags.count++].strData = af[1].r8.lo & 0x40 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "H`";
	reg.flags.flags[reg.flags.count].data = af[1].r8.lo & 0x10;
	reg.flags.flags[reg.flags.count++].strData = af[1].r8.lo & 0x10 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "P`";
	reg.flags.flags[reg.flags.count].data = af[1].r8.lo & 0x4;
	reg.flags.flags[reg.flags.count++].strData = af[1].r8.lo & 0x4 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "N`";
	reg.flags.flags[reg.flags.count].data = af[1].r8.lo & 0x2;
	reg.flags.flags[reg.flags.count++].strData = af[1].r8.lo & 0x2 ? "1" : "0";
	reg.flags.flags[reg.flags.count].name = "C`";
	reg.flags.flags[reg.flags.count].data = af[1].r8.lo & 0x1;
	reg.flags.flags[reg.flags.count++].strData = af[1].r8.lo & 0x1 ? "1" : "0";
	return reg;
}

void z80::reset() {
	for (int i = 0; i < 65536; i++)
	{
		ports_in[i] = 0;
		ports_out[i] = 0;
	}
	HLT = false;
	TickCount = 0;
	INTE = false;
	INTREQ = false;
	pc = 0;
	sp = 0;
	IR.r16 = 0;
	IX.r16 = 0;
	IY.r16 = 0;
	currentBlock = 0;
	currentAF = 0;
	IFF1 = false;
	IFF2 = false;
	interruptMode = 0;
	for (int i = 0; i < 2; i++) {
		af[i].r16 = 0;
		blockRegs[i].bc.r16 = 0;
		blockRegs[i].de.r16 = 0;
		blockRegs[i].hl.r16 = 0;
	}
}


void z80::setfl(bool h)
{
	uint8_t k, b;
	
	F = A & (F3f | F5f);

	b = A;
	k = 0;
	do
	{
		if (b % 2 == 1) k++;
		b /= 2;
	} while (b != 0);

	if (A == 0) SET(fZ); else CLEAR(fZ);
	if (A > 127) SET(fS); else CLEAR(fS);
	if ((k & 0x01) == 0) SET(fPV); else CLEAR(fPV);
	if (h) SET(fH); else CLEAR(fH);
	CLEAR(fC);
	CLEAR(fN);
}

void z80::setfl(uint8_t data, bool c, bool h, bool n)
{
	uint8_t k, b;

	F = data & (F3f | F5f);

	b = data;
	k = 0;
	do
	{
		if (b % 2 == 1) k++;
		b /= 2;
	} while (b != 0);

	if (data == 0) SET(fZ); else CLEAR(fZ);
	if (data > 127) SET(fS); else CLEAR(fS);
	if ((k & 0x01) == 0) SET(fPV); else CLEAR(fPV);
	if (h) SET(fH); else CLEAR(fH);
	if (c) SET(fC); else CLEAR(fC);
	if (n) SET(fN); else CLEAR(fN);
}

/*
void z80::summ(uint8_t sl, uint8_t ppp, uint8_t typ)
{
	uint16_t temp = 0;
	uint8_t iAC = 0;
	F = A & (F3f | F5f);

	if (typ)
	{
		temp = A - sl - ppp;
		iAC = (((A & 0x08) >> 1) | ((sl & 0x08) >> 2) | ((temp & 0x08) >> 3)) & 0x07;
		if (subACtable[iAC] == 1) CLEAR(fH); else SET(fH);
		SET(fN);
	}
	else
	{
		temp = A + sl + ppp;
		iAC = (((A & 0x08) >> 1) | ((sl & 0x08) >> 2) | ((temp & 0x08) >> 3)) & 0x07;
		if (addACtable[iAC] == 1) SET(fH); else CLEAR(fH);
		CLEAR(fN);
	}

	A = (uint8_t)temp;

	if (A == 0) SET(fZ); else CLEAR(fZ);
	if ((A & 0x80) != 0) SET(fS); else CLEAR(fS);
	if (pTable[A] == 1) SET(fPV); else CLEAR(fPV);
	if ((temp & 0x0100) != 0) SET(fC); else CLEAR(fC);
}

void z80::summ16(uint16_t rg, uint16_t ppp, uint8_t typ)
{
	uint32_t temp = 0;
	uint8_t iAC = 0;

	if (typ)
	{
		temp = HL - rg - ppp;
		F = temp & (F3f | F5f);
		iAC = (((H & 0x08) >> 1) | ((rg & 0x08) >> 2) | ((temp & 0x08) >> 3)) & 0x07;
		if (subACtable[iAC] == 1) CLEAR(fH); else SET(fH);
		SET(fN);
	}
	else
	{
		temp = HL + rg + ppp;
		F = temp & (F3f | F5f);
		iAC = (((H & 0x08) >> 1) | ((rg & 0x08) >> 2) | ((temp & 0x08) >> 3)) & 0x07;
		if (addACtable[iAC] == 1) SET(fH); else CLEAR(fH);
		CLEAR(fN);
	}

	HL = (uint16_t)temp;

	if (H == 0) SET(fZ); else CLEAR(fZ);
	if ((H & 0x80) != 0) SET(fS); else CLEAR(fS);
	if (pTable[H] == 1) SET(fPV); else CLEAR(fPV);
	if ((temp & 0x010000) != 0) SET(fC); else CLEAR(fC);
}


uint8_t z80::inrdcr(uint8_t sl, uint8_t typ)
{
	if (typ)
	{
		sl--;
		if ((sl & 0x0F) == 0x0F) CLEAR(fH); else SET(fH);
		SET(fN);
	}
	else
	{
		sl++;
		if ((sl & 0x0F) == 0) SET(fH); else CLEAR(fH);
		CLEAR(fN);
	}
	if (sl == 0) SET(fZ); else CLEAR(fZ);
	if ((sl & 0x80) != 0) SET(fS); else CLEAR(fS);
	if (pTable[sl] == 1) SET(fPV); else CLEAR(fPV);
	return sl;
}
*/

void z80::summ(uint8_t sl, uint8_t ppp, uint8_t typ)
{
	uint16_t temp = 0;
	uint8_t oldA = A;
	F = 0;

	if (typ)
	{
		temp = A - sl - ppp;
		if (((oldA & 0x0F) - (sl & 0x0F) - ppp) & 0x10) SET(fH);
		if (((oldA ^ sl) & (oldA ^ temp) & 0x80) != 0) SET(fPV);
		SET(fN);
	}
	else
	{
		temp = A + sl + ppp;
		if (((oldA & 0x0F) + (sl & 0x0F) + ppp) & 0x10) SET(fH);
		if ((~(oldA ^ sl) & (oldA ^ temp) & 0x80) != 0) SET(fPV);
	}

	A = (uint8_t)temp;
	F |= A & (F3f | F5f);

	if (A == 0) SET(fZ);
	if ((A & 0x80) != 0) SET(fS);
	if ((temp & 0x0100) != 0) SET(fC);
}

void z80::summ16(uint16_t rg, uint16_t ppp, uint8_t typ)
{
	uint32_t temp = 0;
	uint16_t oldHL = HL;
	F = 0;

	if (typ)
	{
		temp = oldHL - rg - ppp;
		if (((oldHL & 0x0FFF) - (rg & 0x0FFF) - ppp) & 0x1000) SET(fH);
		if (((oldHL ^ rg) & (oldHL ^ temp) & 0x8000) != 0) SET(fPV);
		SET(fN);
	}
	else
	{
		temp = oldHL + rg + ppp;
		if (((oldHL & 0x0FFF) + (rg & 0x0FFF) + ppp) & 0x1000) SET(fH);
		if ((~(oldHL ^ rg) & (oldHL ^ temp) & 0x8000) != 0) SET(fPV);
	}

	HL = (uint16_t)temp;
	F |= H & (F3f | F5f);

	if (HL == 0) SET(fZ);
	if ((HL & 0x8000) != 0) SET(fS);
	if ((temp & 0x010000) != 0) SET(fC);
}

uint8_t z80::inrdcr(uint8_t sl, uint8_t typ)
{
	F &= Cf; // C сохраняется
	if (typ)
	{
		sl--;
		if ((sl & 0x0F) == 0x0F) SET(fH);
		if (sl == 0x7F) SET(fPV);
		SET(fN);
	}
	else
	{
		sl++;
		if ((sl & 0x0F) == 0) SET(fH);
		if (sl == 0x80) SET(fPV);
	}
	if (sl == 0) SET(fZ);
	if ((sl & 0x80) != 0) SET(fS);
	F |= sl & (F3f | F5f);
	return sl;
}

uint16_t z80::add16(uint16_t a, uint16_t b) {
	uint32_t tmp = a + b;
	F = uint8_t(F & (Sf | Zf | PVf) | ((tmp >> 8) & (F3f | F5f)) | (((a ^ b ^ tmp) >> 8) & Hf) | ((tmp >> 16) & 1));
	return (uint16_t)tmp;
}

uint8_t z80::execute(uint16_t addr) {
	uint16_t  tmpPC = pc;
	pc = addr;
	uint8_t res = execute();
	pc = tmpPC;
	return res;
}

bool z80::getRegDataFromNumRegs(uint8_t numReg, uint8_t *regData) {

	bool mRes = false;

	switch (numReg)
	{
	case 0:
		*regData = B;
		break;
	case 1:
		*regData = C;
		break;
	case 2:
		*regData = D;
		break;
	case 3:
		*regData = E;
		break;
	case 4:
		*regData = H;
		break;
	case 5:
		*regData = L;
		break;
	case 6:
		mRes = memory->getByte(HL, regData);
		break;
	case 7:
		*regData = A;
		break;
	}
	return mRes;
}

bool z80::setRegDataOfNumRegs(uint8_t numReg, uint8_t regData) {
	bool mRes = false;

	switch (numReg)
	{
	case 0:
		B = regData;
		break;
	case 1:
		C = regData;
		break;
	case 2:
		D = regData;
		break;
	case 3:
		E = regData;
		break;
	case 4:
		H = regData;
		break;
	case 5:
		L = regData;
		break;
	case 6:
		mRes = memory->setByte(HL, regData);
		break;
	case 7:
		A = regData;
		break;
	}
	
	return mRes;
}

uint8_t z80::bitOps(bool ix, bool iy) {
	uint8_t NumTicks = 0;
	uint8_t code;
	bool mRes = false;
	uint8_t regData;
	uint8_t oldRegData;
	uint8_t numReg;
	uint8_t olda;
	uint8_t dopcode;
	uint16_t addr;

	if (!ix && !iy) {
		mRes = memory->getByte(pc++, &code);
		if (mRes) return 0;
		numReg = code & 0x07;

		mRes = getRegDataFromNumRegs(numReg, &regData);
		if (mRes) return 0;

	}
	else {
		int8_t data;
		mRes = memory->getByte(pc++, (uint8_t*)&data);
		if (mRes) return 0;
		if (ix) addr = IX.r16 + data; else {
			if (iy) addr = IY.r16 + data;
		}
		mRes = memory->getByte(pc++, &code);
		if (mRes) return 0;
		numReg = code & 0x07;

		mRes = memory->getByte(addr, &regData);
		if (mRes) return 0;
	}

	code >>= 3;
	//if ((code & 0x07) == 6) NumTicks = 15; else NumTicks = 8;
	if (ix || iy) NumTicks = 23; else 
		if (numReg == 6) NumTicks = 15;	else NumTicks = 8;

	switch (code) {
	case 0x00: // rlc
		olda = (regData & 0x80) >> 7;
		regData <<= 1;
		if (olda == 0x01) regData |= 0x01; else regData &= 0xFE;
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x01: // rrc
		olda = regData & 0x01;
		regData = regData >> 1;
		if (olda == 0x01) regData |= 0x80; else regData &= 0x7F;
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x02: // rl
		olda = (regData & 0x80) >> 7;
		dopcode = F & Cf;
		regData = (regData << 1) | dopcode;
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x03: // rr
		olda = regData & 0x01;
		dopcode = (F & Cf) << 7;
		regData = dopcode | (regData >> 1);
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x04: // sla
		olda = (regData & 0x80) >> 7;
		regData <<= 1;
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x05: // sra
		olda = regData & 0x01;
		dopcode = regData & 0x80;
		regData >>= 1;
		if (dopcode) regData |= 0x80; else regData &= 0x7F;
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x06: // sll
		olda = (regData & 0x80) >> 7;
		regData <<= 1;
		regData |= 0x01;
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x07: // srl
		olda = regData & 0x01;
		regData >>= 1;
		regData &= 0x7F;
		setfl(regData, (bool)olda, false, false);
		break;
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f: // bit
		oldRegData = regData;
		olda = code - 8;
		dopcode = 1 << olda;
		regData = (regData & dopcode) >> olda;
		if (!regData) {
			SET(fZ);
			SET(fPV);
			CLEAR(fS);
			CLEAR(f3);
			CLEAR(f5);
		}
		else {
			CLEAR(fZ);
			CLEAR(fPV);
			uint8_t bitNum = code - 8;
			if (bitNum == 7) SET(fS);
			if (bitNum == 3) SET(f3);
			if (bitNum == 5) SET(f5);
		}
		SET(fH);
		CLEAR(fN);
		//if (NumTicks == 15) NumTicks = 12;
		if (NumTicks == 15) NumTicks = 12; else 
			if (NumTicks == 23) NumTicks = 20;
		regData = oldRegData;
		break;
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17: // res
		olda = code - 0x10;
		dopcode = ~(1 << olda);
		regData &= dopcode;
		break;
	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x1c:
	case 0x1d:
	case 0x1e:
	case 0x1f: // set
		olda = code - 0x18;
		dopcode = (1 << olda);
		regData |= dopcode;
		break;
	}
	
	if (!ix && !iy) {
		mRes = setRegDataOfNumRegs(numReg, regData);
	}
	else {
		mRes = memory->setByte(addr, regData);
	}

	if (mRes) NumTicks = 0;
	return NumTicks;
}
uint8_t z80::miscOps() {
	uint8_t NumTicks = 0;
	uint8_t code;
	bool mRes = false;
	uint16_t addr;
	uint8_t temp;
	uint8_t temp1;
	uint8_t temp2;
	uint16_t portAddr;
	
	mRes = memory->getByte(pc++, &code);
	if (mRes) return NumTicks;
	
	switch (code)
	{
	case 0x40: //in b,(c)
		portAddr = BC;
		B = ports_in[portAddr];
		if (prt[portAddr] != NULL) B = prt[portAddr]->getPortData(portAddr);
		setfl(B, false, false, false);
		NumTicks = 12;
		break;
	case 0x41: //out (c), b
		portAddr =BC;
		ports_out[portAddr] = B;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, B);
		NumTicks = 12;
		break;
	case 0x42: //sbc hl, bc
		summ16(BC, F & 0x01, 1);
		NumTicks = 15;
		break;
	case 0x43: //ld (addr), bc
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setWord(addr, BC);
		NumTicks = 20;
		break;
	case 0x44: //neg
		temp = A;
		A = 0;
		summ(temp, 0, 1);
		NumTicks = 8;
		break;
	case 0x45: //retn
		mRes = memory->getWordFromStack(sp, &pc);
		sp += 2;
		IFF1 = IFF2;
		INTE = IFF1;
		NumTicks = 14;
		break;
	case 0x46: //im 0
		interruptMode = 0;
		NumTicks = 8;
		break;
	case 0x47: //ld i, a
		I = A;
		NumTicks = 9;
		break;
	case 0x48: //in c,(c)
		portAddr = BC;
		C = ports_in[portAddr];
		if (prt[portAddr] != NULL) C = prt[portAddr]->getPortData(portAddr);
		setfl(C, false, false, false);
		NumTicks = 12;
		break;
	case 0x49: //out (c), c
		portAddr = BC;
		ports_out[portAddr] = C;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, C);
		NumTicks = 12;
		break;
	case 0x4A: //adc hl, bc
		summ16(BC, F & 0x01, 0);
		NumTicks = 15;
		break;
	case 0x4B: //ld bc,(addr)
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getWord(addr, &BC);
		NumTicks = 20;
		break;
	case 0x4D: //reti
		mRes = memory->getWordFromStack(sp, &pc);
		sp += 2;
		IFF1 = IFF2;
		INTE = IFF1;
		NumTicks = 14;
		break;
	case 0x4f: //ld r, a
		R = A;
		NumTicks = 9;
		break;
	case 0x50: //in d,(c)
		portAddr = BC;
		D = ports_in[portAddr];
		if (prt[portAddr] != NULL) D = prt[portAddr]->getPortData(portAddr);
		setfl(D, false, false, false);
		NumTicks = 12;
		break;
	case 0x51: //out (c), d
		portAddr = BC;
		ports_out[portAddr] = D;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, D);
		NumTicks = 12;
		break;
	case 0x52: //sbc hl, de
		summ16(DE, F & 0x01, 1);
		NumTicks = 15;
		break;
	case 0x53: //ld (addr), de
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setWord(addr, DE);
		NumTicks = 20;
		break;
	case 0x56: //im 1
		interruptMode = 1;
		NumTicks = 8;
		break;
	case 0x57: //ld a, i
		A = I;
		F = uint8_t((F & Cf) | (A & (F3f | F5f)) | (A == 0 ? Zf : 0) | ((A & 0x80) ? Sf : 0) | (IFF2 ? PVf : 0));
		NumTicks = 9;
		break;
	case 0x58: //in e,(c)
		portAddr = BC;
		E = ports_in[portAddr];
		if (prt[portAddr] != NULL) E = prt[portAddr]->getPortData(portAddr);
		setfl(E, false, false, false);
		NumTicks = 12;
		break;
	case 0x59: //out (c), e
		portAddr = BC;
		ports_out[portAddr] = E;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, E);
		NumTicks = 12;
		break;
	case 0x5A: //adc hl, de
		summ16(DE, F & 0x01, 0);
		NumTicks = 15;
		break;
	case 0x5B: //ld de,(addr)
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getWord(addr, &DE);
		NumTicks = 20;
		break;
	case 0x5e: //im 2
		interruptMode = 2;
		NumTicks = 8;
		break;
	case 0x5f: //ld a, r
		A = R;
		F = uint8_t((F & Cf)|(A & (F3f | F5f))|(A == 0 ? Zf : 0)|((A & 0x80) ? Sf : 0)| (IFF2 ? PVf : 0));
		NumTicks = 9;
		break;
	case 0x60: //in h,(c)
		portAddr = BC;
		H = ports_in[portAddr];
		if (prt[portAddr] != NULL) H = prt[portAddr]->getPortData(portAddr);
		setfl(H, false, false, false);
		NumTicks = 12;
		break;
	case 0x61: //out (c), h
		portAddr = BC;
		ports_out[portAddr] = H;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, H);
		NumTicks = 12;
		break;
	case 0x62: //sbc hl, hl
		summ16(HL, F & 0x01, 1);
		NumTicks = 15;
		break;
	case 0x63: //ld (addr), hl
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setWord(addr, HL);
		NumTicks = 20;
		break;
	case 0x67: //rrd
		mRes = memory->getByte(HL, &temp);
		temp1 = (A & 0x0F) << 4;
		temp2 = temp & 0x0F;
		temp >>= 4;
		temp = temp | temp1;
		A = (A & 0xF0) | temp2;
		mRes = memory->setByte(HL, temp);
		NumTicks = 18;
		break;
	case 0x68: //in l,(c)
		portAddr = BC;
		L = ports_in[portAddr];
		if (prt[portAddr] != NULL) L = prt[portAddr]->getPortData(portAddr);
		setfl(L, false, false, false);
		NumTicks = 12;
		break;
	case 0x69: //out (c), l
		portAddr = BC;
		ports_out[portAddr] = L;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, L);
		NumTicks = 12;
		break;
	case 0x6A: //adc hl, hl
		summ16(HL, F & 0x01, 0);
		NumTicks = 15;
		break;
	case 0x6B: //ld hl,(addr)
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getWord(addr, &HL);
		NumTicks = 20;
		break;
	case 0x6f: //rld
		mRes = memory->getByte(HL, &temp);
		temp1 = (A & 0x0F);
		temp2 = (temp & 0xF0) >> 4;
		temp <<= 4;
		temp = temp | temp1;
		A = (A & 0xF0) | temp2;
		mRes = memory->setByte(HL, temp);
		NumTicks = 18;
		break;
	case 0x70: //in (c) n/d
		portAddr = BC;
		temp = ports_in[portAddr];
		if (prt[portAddr] != NULL) temp = prt[portAddr]->getPortData(portAddr);
		setfl(temp, false, false, false);
		NumTicks = 12;
		break;
	case 0x71: //out (c), 0 n/d
		portAddr = BC;
		ports_out[portAddr] = 0;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, 0);
		NumTicks = 12;
		break;
	case 0x72: //sbc hl, sp
		summ16(sp, F & 0x01, 1);
		NumTicks = 15;
		break;
	case 0x73: //ld (addr), sp
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setWord(addr, sp);
		NumTicks = 20;
		break;
	case 0x78: //in a,(c)
		portAddr = BC;
		A = ports_in[portAddr];
		if (prt[portAddr] != NULL) A = prt[portAddr]->getPortData(portAddr);
		setfl(A, false, false, false);
		NumTicks = 12;
		break;
	case 0x79: //out (c), a
		portAddr = BC;
		ports_out[portAddr] = A;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, A);
		NumTicks = 12;
		break;
	case 0x7A: //adc hl, sp
		summ16(sp, F & 0x01, 0);
		NumTicks = 15;
		break;
	case 0x7B: //ld sp,(addr)
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getWord(addr, &sp);
		NumTicks = 20;
		break;
	case 0xA0: //ldi
		mRes = memory->getByte(HL, &temp);
		mRes = memory->setByte(DE, temp);
		DE++;
		HL++;
		BC--;
		CLEAR(fN);
		CLEAR(fH);
		if (BC) SET(fPV); else CLEAR(fPV);
		NumTicks = 16;
		break;
	case 0xA1: //cpi
		mRes = memory->getByte(HL, &temp);
		temp1 = A;
		temp2 = F & Cf;
		summ(temp, 0, 1);
		A = temp1;
		F = (F & ~(Cf | PVf)) | temp2;
		HL++;
		BC--;
		if (BC) SET(fPV); else CLEAR(fPV);
		NumTicks = 16;
		break;
	case 0xA2: //ini
		portAddr = BC;
		temp = ports_in[portAddr];
		if (prt[portAddr] != NULL) temp = prt[portAddr]->getPortData(portAddr);
		mRes = memory->setByte(HL, temp);
		HL++;
		B--;
		setfl(B, false, false, false);
		NumTicks = 16;
		break;
	case 0xA3: //outi
		mRes = memory->getByte(HL, &temp);
		portAddr = BC;
		ports_out[portAddr] = temp;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, temp);
		HL++;
		B--;
		setfl(B, false, false, false);
		NumTicks = 16;
		break;
	case 0xA8: //ldd
		mRes = memory->getByte(HL, &temp);
		mRes = memory->setByte(DE, temp);
		DE--;
		HL--;
		BC--;
		CLEAR(fN);
		CLEAR(fH);
		if (BC) SET(fPV); else CLEAR(fPV);
		NumTicks = 16;
		break;
	case 0xA9: //cpd
		mRes = memory->getByte(HL, &temp);
		temp1 = A;
		temp2 = F & Cf;
		summ(temp, 0, 1);
		A = temp1;
		F = (F & ~(Cf | PVf)) | temp2;
		HL--;
		BC--;
		if (BC) SET(fPV); else CLEAR(fPV);
		NumTicks = 16;
		break;
	case 0xAA: //ind
		portAddr = BC;
		temp = ports_in[portAddr];
		if (prt[portAddr] != NULL) temp = prt[portAddr]->getPortData(portAddr);
		mRes = memory->setByte(HL, temp);
		HL--;
		B--;
		setfl(B, false, false, false);
		NumTicks = 16;
		break;
	case 0xAB: //outd
		mRes = memory->getByte(HL, &temp);
		portAddr = BC;
		ports_out[portAddr] = temp;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, temp);
		HL--;
		B--;
		setfl(B, false, false, false);
		NumTicks = 16;
		break;
	case 0xB0: //ldir
		mRes = memory->getByte(HL, &temp);
		mRes = memory->setByte(DE, temp);
		DE++;
		HL++;
		BC--;
		CLEAR(fN);
		CLEAR(fH);
		if (BC) {
			SET(fPV);
			NumTicks = 21;
			pc -= 2;
		}
		else {
			CLEAR(fPV);
			NumTicks = 16;
		}
		break;
	case 0xB1: //cpir
		mRes = memory->getByte(HL, &temp);
		temp1 = A;
		temp2 = F & Cf;
		summ(temp, 0, 1);
		A = temp1;
		F = (F & ~(Cf | PVf)) | temp2;
		HL++;
		BC--;
		if (BC) SET(fPV);
		if (BC && !(F & Zf)) {
			NumTicks = 21;
			pc -= 2;
		}
		else {
			NumTicks = 16;
		}
		break;
	case 0xB2: //inir
		portAddr = BC;
		temp = ports_in[portAddr];
		if (prt[portAddr] != NULL) temp = prt[portAddr]->getPortData(portAddr);
		mRes = memory->setByte(HL, temp);
		HL++;
		B--;
		setfl(B, false, false, false);
		if (B) {
			NumTicks = 21;
			pc -= 2;
		}
		else {
			NumTicks = 16;
		}
		break;
	case 0xB3: //outir
		mRes = memory->getByte(HL, &temp);
		portAddr = BC;
		ports_out[portAddr] = temp;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, temp);
		HL++;
		B--;
		setfl(B, false, false, false);
		if (B) {
			NumTicks = 21;
			pc -= 2;
		}
		else {
			NumTicks = 16;
		}
		break;
	case 0xB8: //lddr
		mRes = memory->getByte(HL, &temp);
		mRes = memory->setByte(DE, temp);
		DE--;
		HL--;
		BC--;
		CLEAR(fN);
		CLEAR(fH);
		if (BC) {
			SET(fPV);
			NumTicks = 21;
			pc -= 2;
		}
		else {
			CLEAR(fPV);
			NumTicks = 16;
		}
		break;
	case 0xB9: //cpdr
		mRes = memory->getByte(HL, &temp);
		temp1 = A;
		temp2 = F & Cf;
		summ(temp, 0, 1);
		A = temp1;
		F = (F & ~(Cf | PVf)) | temp2;
		HL--;
		BC--;
		if (BC) SET(fPV);
		if (BC && !(F & Zf)) {
			NumTicks = 21;
			pc -= 2;
		}
		else {
			NumTicks = 16;
		}
		break;
	case 0xBA: //indr
		portAddr = BC;
		temp = ports_in[portAddr];
		if (prt[portAddr] != NULL) temp = prt[portAddr]->getPortData(portAddr);
		mRes = memory->setByte(HL, temp);
		HL--;
		B--;
		setfl(B, false, false, false);
		if (B) {
			NumTicks = 21;
			pc -= 2;
		}
		else {
			NumTicks = 16;
		}
		break;
	case 0xBB: //outdr
		mRes = memory->getByte(HL, &temp);
		portAddr = BC;
		ports_out[portAddr] = temp;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, temp);
		HL--;
		B--;
		setfl(B, false, false, false);
		if (B) {
			NumTicks = 21;
			pc -= 2;
		}
		else {
			NumTicks = 16;
		}
		break;
	default:
		NumTicks = 8;
	}

	return NumTicks;
}

uint8_t z80::ixiyOps(bool isIX, bool isIY) {
	uint16_t addr;
	uint8_t dopcode, olda, b1;
	uint8_t NumTicks = 0;
	uint16_t afl;
	z80reg16 ixiytemp;

	if (isIX) ixiytemp = IX;
	if (isIY) ixiytemp = IY;
	
	uint8_t code;
	bool mRes = false;
	mRes = memory->getByte(pc++, &code);
	if (mRes) {
		return NumTicks;
	}

	switch (code)
	{
	case 0x04: //{inc b}
		B = inrdcr(B, 0);
		NumTicks = 8;
		break;
	case 0x05: //{dec b}
		B = inrdcr(B, 1);
		NumTicks = 8;
		break;
	case 0x06: //{ld b, d8}
		mRes = memory->getByte(pc++, &B);
		NumTicks = 11;
		break;
	case 0x09: //{add ix,bc}
		ixiytemp.r16 = add16(ixiytemp.r16, BC);
		NumTicks = 15;
		break;
	case 0x0C: // {inc c}
		C = inrdcr(C, 0);
		NumTicks = 8;
		break;
	case 0x0D: //{dec c}
		C = inrdcr(C, 1);
		NumTicks = 8;
		break;
	case 0x0E: // {ld c, d8}
		mRes = memory->getByte(pc++, &C);
		NumTicks = 11;
		break;
	case 0x14: // {inc d}
		D = inrdcr(D, 0);
		NumTicks = 8;
		break;
	case 0x15: // {dec d}
		D = inrdcr(D, 1);
		NumTicks = 8;
		break;
	case 0x16: // {ld d, d8}
		mRes = memory->getByte(pc++, &D);
		NumTicks = 11;
		break;
	case 0x19: // {add ix, de}
		ixiytemp.r16 = add16(ixiytemp.r16, DE);
		NumTicks = 15;
		break;
	case 0x1C: // {inc e}
		E = inrdcr(E, 0);
		NumTicks = 8;
		break;
	case 0x1D: // {dec e}
		E = inrdcr(E, 1);
		NumTicks = 8;
		break;
	case 0x1E: // {ld e, d8}
		mRes = memory->getByte(pc++, &E);
		NumTicks = 11;
		break;
	case 0x21: // {ld ix, d16}
		mRes = memory->getWord(pc, &ixiytemp.r16);
		pc += 2;
		NumTicks = 14;
		break;
	case 0x22: // {ld (d16), ix}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setWord(addr, ixiytemp.r16);
		NumTicks = 20;
		break;
	case 0x23: // {inc ix}
		ixiytemp.r16++;
		NumTicks = 10;
		break;
	case 0x24: // {inc ixh}
		ixiytemp.r8.hi = inrdcr(ixiytemp.r8.hi, 0);
		NumTicks = 8;
		break;
	case 0x25: /// {dec ixh}
		ixiytemp.r8.hi = inrdcr(ixiytemp.r8.hi, 1);
		NumTicks = 8;
		break;
	case 0x26: // {ld ixh, d8}
		mRes = memory->getByte(pc++, &ixiytemp.r8.hi);
		NumTicks = 11;
		break;
	case 0x29: // {add ix, ix}
		ixiytemp.r16 = add16(ixiytemp.r16, ixiytemp.r16);
		NumTicks = 15;
		break;
	case 0x2A: // {ld ix, (addr)}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getWord(addr, &ixiytemp.r16);
		NumTicks = 20;
		break;
	case 0x2B: // {dec ix}
		ixiytemp.r16--;
		NumTicks = 10;
		break;
	case 0x2C: // {inc ixl}
		ixiytemp.r8.lo = inrdcr(ixiytemp.r8.lo, 0);
		NumTicks = 8;
		break;
	case 0x2D: // {dec ixl}
		ixiytemp.r8.lo = inrdcr(ixiytemp.r8.lo, 1);
		NumTicks = 8;
		break;
	case 0x2E: // {ld ixl, d8}
		mRes = memory->getByte(pc++, &ixiytemp.r8.lo);
		NumTicks = 11;
		break;
	case 0x34: // {inc (ix+d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &dopcode);
		dopcode = inrdcr(dopcode, 0);
		mRes = memory->setByte(addr, dopcode);
		NumTicks = 23;
		break;
	case 0x35: // {dec (ix+d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &dopcode);
		dopcode = inrdcr(dopcode, 1);
		mRes = memory->setByte(addr, dopcode);
		NumTicks = 23;
		break;
	case 0x36: // {ld (ix+d), d8}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(pc++, &dopcode);
		mRes = memory->setByte(addr, dopcode);
		NumTicks = 19;
		break;
	case 0x39: // {add ix, sp}
		ixiytemp.r16 = add16(ixiytemp.r16, sp);
		NumTicks = 15;
		break;
	case 0x3C: // {inc a}
		A = inrdcr(A, 0);
		NumTicks = 8;
		break;
	case 0x3D: // {dec a}
		A = inrdcr(A, 1);
		NumTicks = 8;
		break;
	case 0x3E: // {ld a,b1}
		mRes = memory->getByte(pc++, &A);
		NumTicks = 11;
		break;
	case 0x40: // {ld b,b}
		NumTicks = 8;
		break;
	case 0x41: // {ld b,c}
		B = C;
		NumTicks = 8;
		break;
	case 0x42: // {ld b,d}
		B = D;
		NumTicks = 8;
		break;
	case 0x43: // {ld b,e}
		B = E;
		NumTicks = 8;
		break;
	case 0x44: // {ld b,ixh}
		B = ixiytemp.r8.hi;
		NumTicks = 8;
		break;
	case 0x45: // {ld b,ixl}
		B = ixiytemp.r8.lo;
		NumTicks = 8;
		break;
	case 0x46: // {ld b,(ix+d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &B);
		NumTicks = 19;
		break;
	case 0x47: // {ld b,a}
		B = A;
		NumTicks = 8;
		break;
	case 0x48: // {ld c,b}
		C = B;
		NumTicks = 8;
		break;
	case 0x49: // {ld c,c}
		NumTicks = 8;
		break;
	case 0x4A: // {ld c,d}
		C = D;
		NumTicks = 8;
		break;
	case 0x4B: // {ld c,e}
		C = E;
		NumTicks = 8;
		break;
	case 0x4C: // {ld c,ixh}
		C = ixiytemp.r8.hi;
		NumTicks = 8;
		break;
	case 0x4D: // {ld c,ixl}
		C = ixiytemp.r8.lo;
		NumTicks = 8;
		break;
	case 0x4E: // {ld c,(ix+d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &C);
		NumTicks = 19;
		break;
	case 0x4F: // {ld c,a}
		C = A;
		NumTicks = 8;
		break;
	case 0x50: // {ld d,b}
		D = B;
		NumTicks = 8;
		break;
	case 0x51: // {ld d,c}
		D = C;
		NumTicks = 8;
		break;
	case 0x52: // {ld d,d}
		NumTicks = 8;
		break;
	case 0x53: // {ld d,e}
		D = E;
		NumTicks = 8;
		break;
	case 0x54: // {ld d,ixh}
		D = ixiytemp.r8.hi;
		NumTicks = 8;
		break;
	case 0x55: // {ld d,ixl}
		D = ixiytemp.r8.lo;
		NumTicks = 8;
		break;
	case 0x56: // {ld d,(ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &D);
		NumTicks = 19;
		break;
	case 0x57: // {ld d,a}
		D = A;
		NumTicks = 8;
		break;
	case 0x58: // {ld e,b}
		E = B;
		NumTicks = 8;
		break;
	case 0x59: // {ld e,c}
		E = C;
		NumTicks = 8;
		break;
	case 0x5A: // {ld e,d}
		E = D;
		NumTicks = 8;
		break;
	case 0x5B: // {ld e,e}
		NumTicks = 8;
		break;
	case 0x5C: // {ld e,ixh}
		E = ixiytemp.r8.hi;
		NumTicks = 8;
		break;
	case 0x5D: // {ld e,ixl}
		E = ixiytemp.r8.lo;
		NumTicks = 8;
		break;
	case 0x5E: // {ld e,(ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &E);
		NumTicks = 19;
		break;
	case 0x5F: // {ld e,a}
		E = A;
		NumTicks = 8;
		break;
	case 0x60: // {ld ixh,b}
		ixiytemp.r8.hi = B;
		NumTicks = 8;
		break;
	case 0x61: // {ld ixh,c}
		ixiytemp.r8.hi = C;
		NumTicks = 8;
		break;
	case 0x62: // {ld ixh,d}
		ixiytemp.r8.hi = D;
		NumTicks = 8;
		break;
	case 0x63: // {ld ixh,e}
		ixiytemp.r8.hi = E;
		NumTicks = 8;
		break;
	case 0x64: // {ld ixh,ixh}
		NumTicks = 8;
		break;
	case 0x65: // {ld ixh,ixl}
		ixiytemp.r8.hi = ixiytemp.r8.lo;
		NumTicks = 8;
		break;
	case 0x66: // {ld h,(ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &H);
		NumTicks = 19;
		break;
	case 0x67: // {ld ixh,a}
		ixiytemp.r8.hi = A;
		NumTicks = 8;
		break;
	case 0x68: // {ld ixl,b}
		ixiytemp.r8.lo = B;
		NumTicks = 8;
		break;
	case 0x69: // {ld ixl,c}
		ixiytemp.r8.lo = C;
		NumTicks = 8;
		break;
	case 0x6A: // {ld ixl,d}
		ixiytemp.r8.lo = D;
		NumTicks = 8;
		break;
	case 0x6B: // {ld ixl,e}
		ixiytemp.r8.lo = E;
		NumTicks = 8;
		break;
	case 0x6C: // {ld ixl,ixh}
		ixiytemp.r8.lo = ixiytemp.r8.hi;
		NumTicks = 8;
		break;
	case 0x6D: // {ld ixl,ixl}
		NumTicks = 8;
		break;
	case 0x6E: // {ld l,(ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &L);
		NumTicks = 19;
		break;
	case 0x6F: // {ld ixl,a}
		ixiytemp.r8.lo = A;
		NumTicks = 8;
		break;
	case 0x70: // {ld (ix + d),b}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->setByte(addr, B);
		NumTicks = 19;
		break;
	case 0x71: // {ld (ix + d),c}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->setByte(addr, C);
		NumTicks = 19;
		break;
	case 0x72: // {ld (ix + d),d}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->setByte(addr, D);
		NumTicks = 19;
		break;
	case 0x73: // {ld (ix + d),e}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->setByte(addr, E);
		NumTicks = 19;
		break;
	case 0x74: // {ld (ix + d),h}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->setByte(addr, H);
		NumTicks = 19;
		break;
	case 0x75: // {ld (ix + d),l}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->setByte(addr, L);
		NumTicks = 19;
		break;
	case 0x77: // ld (ix + d),a}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->setByte(addr, A);
		NumTicks = 19;
		break;
	case 0x78: // {ld a,b}
		A = B;
		NumTicks = 8;
		break;
	case 0x79: // {ld a,c}
		A = C;
		NumTicks = 8;
		break;
	case 0x7A: // {ld a,d}
		A = D;
		NumTicks = 8;
		break;
	case 0x7B: // {ld a,e}
		A = E;
		NumTicks = 8;
		break;
	case 0x7C: // {ld a,ixh}
		A = ixiytemp.r8.hi;
		NumTicks = 8;
		break;
	case 0x7D: // {ld a,ixl}
		A = ixiytemp.r8.lo;
		NumTicks = 8;
		break;
	case 0x7E: // {ld a,(ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &A);
		NumTicks = 19;
		break;
	case 0x7F: // {ld a,a}
		NumTicks = 8;
		break;
	case 0x80: // {add a, b}
		summ(B, 0, 0);
		NumTicks = 8;
		break;
	case 0x81: // {add a, c}
		summ(C, 0, 0);
		NumTicks = 8;
		break;
	case 0x82: // {add a, d}
		summ(D, 0, 0);
		NumTicks = 8;
		break;
	case 0x83: // {add a, e}
		summ(E, 0, 0);
		NumTicks = 8;
		break;
	case 0x84: // {add a, ixh}
		summ(ixiytemp.r8.hi, 0, 0);
		NumTicks = 8;
		break;
	case 0x85: // {add a, ixl}
		summ(ixiytemp.r8.lo, 0, 0);
		NumTicks = 8;
		break;
	case 0x86: // {add a, (ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &b1);
		summ(b1, 0, 0);
		NumTicks = 19;
		break;
	case 0x87: // {add a, a}
		summ(A, 0, 0);
		NumTicks = 8;
		break;
	case 0x88: // {adc a, b}
		summ(B, F & 0x01, 0);
		NumTicks = 8;
		break;
	case 0x89: // {adc a, c}
		summ(C, F & 0x01, 0);
		NumTicks = 8;
		break;
	case 0x8A: // {adc a, d}
		summ(D, F & 0x01, 0);
		NumTicks = 8;
		break;
	case 0x8B: // {adc a, e}
		summ(E, F & 0x01, 0);
		NumTicks = 8;
		break;
	case 0x8C: // {adc a, ixh}
		summ(ixiytemp.r8.hi, F & 0x01, 0);
		NumTicks = 8;
		break;
	case 0x8D: // {adc a, ixl}
		summ(ixiytemp.r8.lo, F & 0x01, 0);
		NumTicks = 8;
		break;
	case 0x8E: // {adc a, (ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &b1);
		summ(b1, F & 0x01, 0);
		NumTicks = 19;
		break;
	case 0x8F: // {adc a, a}
		summ(A, F & 0x01, 0);
		NumTicks = 8;
		break;
	case 0x90: // {sub b}
		summ(B, 0, 1);
		NumTicks = 8;
		break;
	case 0x91: // {sub c}
		summ(C, 0, 1);
		NumTicks = 8;
		break;
	case 0x92: // {sub d}
		summ(D, 0, 1);
		NumTicks = 8;
		break;
	case 0x93: // {sub e}
		summ(E, 0, 1);
		NumTicks = 8;
		break;
	case 0x94: // {sub ixh}
		summ(ixiytemp.r8.hi, 0, 1);
		NumTicks = 8;
		break;
	case 0x95: // {sub ixl}
		summ(ixiytemp.r8.lo, 0, 1);
		NumTicks = 8;
		break;
	case 0x96: // {sub (ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &b1);
		summ(b1, 0, 1);
		NumTicks = 19;
		break;
	case 0x97: // {sub a}
		summ(A, 0, 1);
		NumTicks = 8;
		break;
	case 0x98: // {sbc a, b}
		summ(B, F & 0x01, 1);
		NumTicks = 8;
		break;
	case 0x99: // {sbc a, c}
		summ(C, F & 0x01, 1);
		NumTicks = 8;
		break;
	case 0x9A: // {sbc a, d}
		summ(D, F & 0x01, 1);
		NumTicks = 8;
		break;
	case 0x9B: // {sbc a, e}
		summ(E, F & 0x01, 1);
		NumTicks = 8;
		break;
	case 0x9C: // {sbc a, ixh}
		summ(ixiytemp.r8.hi, F & 0x01, 1);
		NumTicks = 8;
		break;
	case 0x9D: // {sbc a, ixl}
		summ(ixiytemp.r8.lo, F & 0x01, 1);
		NumTicks = 8;
		break;
	case 0x9E: // {sbc a, (ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &b1);
		summ(b1, F & 0x01, 1);
		NumTicks = 19;
		break;
	case 0x9F: // {sbc a, a}
		summ(A, F & 0x01, 1);
		NumTicks = 8;
		break;
	case 0xA0: // {and b}
		A &= B;
		setfl(true);
		NumTicks = 8;
		break;
	case 0xA1: // {and c}
		A &= C;
		setfl(true);
		NumTicks = 8;
		break;
	case 0xA2: // {and d}
		A &= D;
		setfl(true);
		NumTicks = 8;
		break;
	case 0xA3: // {and e}
		A &= E;
		setfl(true);
		NumTicks = 8;
		break;
	case 0xA4: // {and ixh}
		A &= ixiytemp.r8.hi;
		setfl(true);
		NumTicks = 8;
		break;
	case 0xA5: // {and ixl}
		A &= ixiytemp.r8.lo;
		setfl(true);
		NumTicks = 8;
		break;
	case 0xA6: // {and (ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &b1);
		A &= b1;
		setfl(true);
		NumTicks = 19;
		break;
	case 0xA7: // {and a}
		A &= A;
		setfl(true);
		NumTicks = 8;
		break;
	case 0xA8: // {xor b}
		A ^= B;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xA9: // {xor c}
		A ^= C;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xAA: // {xor d}
		A ^= D;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xAB: // {xor e}
		A ^= E;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xAC: // {xor ixh}
		A ^= ixiytemp.r8.hi;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xAD: // {xor ixl}
		A ^= ixiytemp.r8.lo;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xAE: // {xor (ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &b1);
		A ^= b1;
		setfl(false);
		NumTicks = 19;
		break;
	case 0xAF: // {xor a}
		A ^= A;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB0: // {or b}
		A |= B;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB1: // {or c}
		A |= C;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB2: // {or d}
		A |= D;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB3: // {or e}
		A |= E;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB4: // {or ixh}
		A |= ixiytemp.r8.hi;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB5: // {or ixl}
		A |= ixiytemp.r8.lo;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB6: // {or (ix + d)}
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &b1);
		A |= b1;
		setfl(false);
		NumTicks = 19;
		break;
	case 0xB7: // {or a}
		A |= A;
		setfl(false);
		NumTicks = 8;
		break;
	case 0xB8: // {cp b}
		olda = A;
		summ(B, 0, 1);
		A = olda;
		NumTicks = 8;
		break;
	case 0xB9: // {cp c}
		olda = A;
		summ(C, 0, 1);
		A = olda;
		NumTicks = 8;
		break;
	case 0xBA: // {cp d}
		olda = A;
		summ(D, 0, 1);
		A = olda;
		NumTicks = 8;
		break;
	case 0xBB: // {cp e}
		olda = A;
		summ(E, 0, 1);
		A = olda;
		NumTicks = 8;
		break;
	case 0xBC: // {cp ixh}
		olda = A;
		summ(ixiytemp.r8.hi, 0, 1);
		A = olda;
		NumTicks = 8;
		break;
	case 0xBD: // {cp ixl}
		olda = A;
		summ(ixiytemp.r8.lo, 0, 1);
		A = olda;
		NumTicks = 8;
		break;
	case 0xBE: // {cp (ix + d)}
		olda = A;
		mRes = memory->getByte(pc, &b1);
		pc++;
		addr = ixiytemp.r16 + (int8_t)b1;
		mRes = memory->getByte(addr, &dopcode);
		summ(dopcode, 0, 1);
		A = olda;
		NumTicks = 19;
		break;
	case 0xBF: // {cp a}
		olda = A;
		summ(A, 0, 1);
		A = olda;
		NumTicks = 8;
		break;
	case 0xCB: // prefix bit operation
		NumTicks = bitOps(isIX, isIY);
		break;
	case 0xE1: // {pop ix}
		mRes = memory->getWordFromStack(sp, &ixiytemp.r16);
		sp += 2;
		NumTicks = 14;
		break;
	case 0xE3: // {ex (sp), ix}
		mRes = memory->getWordFromStack(sp, &addr);
		mRes = memory->setWordToStack(sp, ixiytemp.r16);
		ixiytemp.r16 = addr;
		NumTicks = 23;
		break;
	case 0xE5: // {push ix}
		sp -= 2;
		mRes = memory->setWordToStack(sp, ixiytemp.r16);
		NumTicks = 15;
		break;
	case 0xE9: // {jp (ix)}
		pc = ixiytemp.r16;
		NumTicks = 8;
		break;
	case 0xF9: // {ld sp, ix}
		sp = ixiytemp.r16;
		NumTicks = 10;
		break;
	}

	if (isIX) IX = ixiytemp;
	if (isIY) IY = ixiytemp;

	if (mRes) {
		return 0;
	}
	return NumTicks;

}

uint8_t z80::execute() {
	uint16_t addr;
	uint8_t dopcode, olda, b1;
	uint8_t NumTicks = 0;
	uint16_t afl;
	uint8_t tmp;

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
	case 0x00: //{nop}
		NumTicks = 4;
		break;
	case 0x01: //{ld bc,d16}
		mRes = memory->getWord(pc, &BC);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x02: //{ld (bc), a}
		mRes = memory->setByte(BC, A);
		NumTicks = 7;
		break;
	case 0x03: //{inc bc}
		BC++;
		NumTicks = 6;
		break;
	case 0x04: //{inc b}
		B = inrdcr(B, 0);
		NumTicks = 4;
		break;
	case 0x05: //{dec b}
		B = inrdcr(B, 1);
		NumTicks = 4;
		break;
	case 0x06: //{ld b, d8}
		mRes = memory->getByte(pc++, &B);
		NumTicks = 7;
		break;
	case 0x07: // {rlca}
		olda = A & 0x80;
		A <<= 1;
		if (olda == 0x80)
		{
			A |= 0x01;
			SET(fC);
		}
		else CLEAR(fC);
		CLEAR(fN);
		CLEAR(fH);
		NumTicks = 4;
		break;
	case 0x08: // ex af, af`
		if (currentAF == 0) currentAF = 1; else currentAF = 0;
		NumTicks = 4;
		break;
	case 0x09: //{add hl,bc}
		HL = add16(HL, BC);
		NumTicks = 11;
		break;
	case 0x0A: // {ld a, (bc)}
		mRes = memory->getByte(BC, &A);
		NumTicks = 7;
		break;
	case 0x0B: // {dec bc}
		BC--;
		NumTicks = 6;
		break;
	case 0x0C: // {inc c}
		C = inrdcr(C, 0);
		NumTicks = 4;
		break;
	case 0x0D: //{dec c}
		C = inrdcr(C, 1);
		NumTicks = 4;
		break;
	case 0x0E: // {ld c, d8}
		mRes = memory->getByte(pc++, &C);
		NumTicks = 7;
		break;
	case 0x0F: // {rrc}
		olda = A & 0x01;
		A = A >> 1;
		if (olda == 0x01)
		{
			A |= 0x80;
			SET(fC);
		}
		else CLEAR(fC);
		CLEAR(fN);
		CLEAR(fH);
		NumTicks = 4;
		break;
	case 0x10: // djnz d8
		mRes = memory->getByte(pc++, &b1);
		B--;
		if (B == 0) {
			NumTicks = 8;
		}
		else {
			pc += (int8_t)b1;
			NumTicks = 13;
		}
		break;
	case 0x11: // {ld de,d16}
		mRes = memory->getWord(pc, &DE);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x12: // {ld (de),a}
		mRes = memory->setByte(DE, A);
		NumTicks = 7;
		break;
	case 0x13: // {inc de}
		DE++;
		NumTicks = 6;
		break;
	case 0x14: // {inc d}
		D = inrdcr(D, 0);
		NumTicks = 4;
		break;
	case 0x15: // {dec d}
		D = inrdcr(D, 1);
		NumTicks = 4;
		break;
	case 0x16: // {ld d, d8}
		mRes = memory->getByte(pc++, &D);
		NumTicks = 7;
		break;
	case 0x17: // {rla}
		olda = A & 0x80;
		dopcode = F & 0x01;
		A = (A << 1) | dopcode;
		if (olda == 0x80) SET(fC); else CLEAR(fC);
		CLEAR(fN);
		CLEAR(fH);
		NumTicks = 4;
		break;
	case 0x18: // jr d8
		mRes = memory->getByte(pc++, &b1);
		pc += (int8_t)b1;
		NumTicks = 12;
		break;
	case 0x19: // {add hl, de}
		HL = add16(HL, DE);
		NumTicks = 11;
		break;
	case 0x1A: // {ld a, (de)}
		mRes = memory->getByte(DE, &A);
		NumTicks = 7;
		break;
	case 0x1B: // {dec de}
		DE--;
		NumTicks = 6;
		break;
	case 0x1C: // {inc e}
		E = inrdcr(E, 0);
		NumTicks = 4;
		break;
	case 0x1D: // {dec e}
		E = inrdcr(E, 1);
		NumTicks = 4;
		break;
	case 0x1E: // {ld e, d8}
		mRes = memory->getByte(pc++, &E);
		NumTicks = 7;
		break;
	case 0x1F: // {rra}
		olda = A & 0x01;
		dopcode = F & 0x01;
		dopcode <<= 7;
		A = dopcode | (A >> 1);
		if (olda == 0x01) SET(fC); else CLEAR(fC);
		CLEAR(fN);
		CLEAR(fH);
		NumTicks = 4;
		break;
	case 0x20: // jr nz, d8
		mRes = memory->getByte(pc++, &b1);
		if ((F & Zf) == 0) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			NumTicks = 7;
		}
		break;
	case 0x21: // {ld hl, d16}
		mRes = memory->getWord(pc, &HL);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x22: // {ld (d16), hl}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setWord(addr, HL);
		NumTicks = 16;
		break;
	case 0x23: // {inc hl}
		HL++;
		NumTicks = 6;
		break;
	case 0x24: // {inc h}
		H = inrdcr(H, 0);
		NumTicks = 4;
		break;
	case 0x25: /// {dec h}
		H = inrdcr(H, 1);
		NumTicks = 4;
		break;
	case 0x26: // {ld h, d8}
		mRes = memory->getByte(pc++, &H);
		NumTicks = 7;
		break;
	case 0x27: // {daa}
		{
			uint16_t tempAF = AF;
			AF = daaAFTable[(tempAF >> 8) | ((tempAF & (Cf | Nf)) << 8) | ((tempAF & Hf) << 6)];
			NumTicks = 4;
		}
		break;
	case 0x28: // jr z,d8
		mRes = memory->getByte(pc++, &b1);
		if ((F & Zf) == Zf) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			NumTicks = 7;
		}
		break;
	case 0x29: // {add hl, hl}
		HL = add16(HL, HL);
		NumTicks = 11;
		break;
	case 0x2A: // {ld hl, (addr)}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getWord(addr, &HL);
		NumTicks = 16;
		break;
	case 0x2B: // {dec hl}
		HL--;
		NumTicks = 6;
		break;
	case 0x2C: // {inc l}
		L = inrdcr(L, 0);
		NumTicks = 4;
		break;
	case 0x2D: // {dec l}
		L = inrdcr(L, 1);
		NumTicks = 4;
		break;
	case 0x2E: // {ld l, d8}
		mRes = memory->getByte(pc++, &L);
		NumTicks = 7;
		break;
	case 0x2F: // {cpl}
		F = (uint8_t)((F & (Sf | Zf | PVf | Cf)) | (((A = ~A) & (F3f | F5f)) | Hf | Nf));
		NumTicks = 4;
		break;
	case 0x30: // jr nc, d8
		mRes = memory->getByte(pc++, &b1);
		if ((F & Cf) == 0) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			NumTicks = 7;
		}
		break;
	case 0x31: // {ld sp, d16}
		mRes = memory->getWord(pc, &sp);
		pc += 2;
		NumTicks = 10;
		break;
	case 0x32: // {ld (addr), a}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->setByte(addr, A);
		NumTicks = 13;
		break;
	case 0x33: // {inc sp}
		sp++;
		NumTicks = 6;
		break;
	case 0x34: // {inc (hl)}
		mRes = memory->getByte(HL, &dopcode);
		dopcode = inrdcr(dopcode, 0);
		mRes = memory->setByte(HL, dopcode);
		NumTicks = 11;
		break;
	case 0x35: // {dec (hl)}
		mRes = memory->getByte(HL, &dopcode);
		dopcode = inrdcr(dopcode, 1);
		mRes = memory->setByte(HL, dopcode);
		NumTicks = 11;
		break;
	case 0x36: // {ld (hl), d8}
		mRes = memory->getByte(pc++, &dopcode);
		mRes = memory->setByte(HL, dopcode);
		NumTicks = 10;
		break;
	case 0x37: // {scf}
		F = (uint8_t)((F & (Sf | Zf | PVf)) | (A & (F3f | F5f)) | Cf);
		NumTicks = 4;
		break;
	case 0x38: // jr c, d8
		mRes = memory->getByte(pc++, &b1);
		if ((F & Cf) == Cf) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			NumTicks = 7;
		}
		break;
	case 0x39: // {add hl, sp}
		HL = add16(HL, sp);
		NumTicks = 11;
		break;
	case 0x3A: // {ld a, (addr)}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		mRes = memory->getByte(addr, &A);
		NumTicks = 13;
		break;
	case 0x3B: // {dec sp}
		sp--;
		NumTicks = 6;
		break;
	case 0x3C: // {inc a}
		A = inrdcr(A, 0);
		NumTicks = 4;
		break;
	case 0x3D: // {dec a}
		A = inrdcr(A, 1);
		NumTicks = 4;
		break;
	case 0x3E: // {ld a,b1}
		mRes = memory->getByte(pc++, &A);
		NumTicks = 7;
		break;
	case 0x3F: // {ccf}
	{
		uint8_t oldCf = (F & Cf) << 4;
		F = (uint8_t)((F & (Sf | Zf | PVf | Cf) ^ Cf) | (A & (F3f | F5f)) | oldCf);
		NumTicks = 4;
	}
		break;
	case 0x40: // {ld b,b}
		NumTicks = 4;
		break;
	case 0x41: // {ld b,c}
		B = C;
		NumTicks = 4;
		break;
	case 0x42: // {ld b,d}
		B = D;
		NumTicks = 4;
		break;
	case 0x43: // {ld b,e}
		B = E;
		NumTicks = 4;
		break;
	case 0x44: // {ld b,h}
		B = H;
		NumTicks = 4;
		break;
	case 0x45: // {ld b,l}
		B = L;
		NumTicks = 4;
		break;
	case 0x46: // {ld b,(hl)}
		mRes = memory->getByte(HL, &B);
		NumTicks = 7;
		break;
	case 0x47: // {ld b,a}
		B = A;
		NumTicks = 4;
		break;
	case 0x48: // {ld c,b}
		C = B;
		NumTicks = 4;
		break;
	case 0x49: // {ld c,c}
		NumTicks = 4;
		break;
	case 0x4A: // {ld c,d}
		C = D;
		NumTicks = 4;
		break;
	case 0x4B: // {ld c,e}
		C = E;
		NumTicks = 4;
		break;
	case 0x4C: // {ld c,h}
		C = H;
		NumTicks = 4;
		break;
	case 0x4D: // {ld c,l}
		C = L;
		NumTicks = 4;
		break;
	case 0x4E: // {ld c,(hl)}
		mRes = memory->getByte(HL, &C);
		NumTicks = 7;
		break;
	case 0x4F: // {ld c,a}
		C = A;
		NumTicks = 4;
		break;
	case 0x50: // {ld d,b}
		D = B;
		NumTicks = 4;
		break;
	case 0x51: // {ld d,c}
		D = C;
		NumTicks = 4;
		break;
	case 0x52: // {ld d,d}
		NumTicks = 4;
		break;
	case 0x53: // {ld d,e}
		D = E;
		NumTicks = 4;
		break;
	case 0x54: // {ld d,h}
		D = H;
		NumTicks = 4;
		break;
	case 0x55: // {ld d,l}
		D = L;
		NumTicks = 4;
		break;
	case 0x56: // {ld d,(hl)}
		mRes = memory->getByte(HL, &D);
		NumTicks = 7;
		break;
	case 0x57: // {ld d,a}
		D = A;
		NumTicks = 4;
		break;
	case 0x58: // {ld e,b}
		E = B;
		NumTicks = 4;
		break;
	case 0x59: // {ld e,c}
		E = C;
		NumTicks = 4;
		break;
	case 0x5A: // {ld e,d}
		E = D;
		NumTicks = 4;
		break;
	case 0x5B: // {ld e,e}
		NumTicks = 4;
		break;
	case 0x5C: // {ld e,h}
		E = H;
		NumTicks = 4;
		break;
	case 0x5D: // {ld e,l}
		E = L;
		NumTicks = 4;
		break;
	case 0x5E: // {ld e,(hl)}
		mRes = memory->getByte(HL, &E);
		NumTicks = 7;
		break;
	case 0x5F: // {ld e,a}
		E = A;
		NumTicks = 4;
		break;
	case 0x60: // {ld h,b}
		H = B;
		NumTicks = 4;
		break;
	case 0x61: // {ld h,c}
		H = C;
		NumTicks = 4;
		break;
	case 0x62: // {ld h,d}
		H = D;
		NumTicks = 4;
		break;
	case 0x63: // {ld h,e}
		H = E;
		NumTicks = 4;
		break;
	case 0x64: // {ld h,h}
		NumTicks = 4;
		break;
	case 0x65: // {ld h,l}
		H = L;
		NumTicks = 4;
		break;
	case 0x66: // {ld h,(hl)}
		mRes = memory->getByte(HL, &H);
		NumTicks = 7;
		break;
	case 0x67: // {ld h,a}
		H = A;
		NumTicks = 4;
		break;
	case 0x68: // {ld l,b}
		L = B;
		NumTicks = 4;
		break;
	case 0x69: // {ld l,c}
		L = C;
		NumTicks = 4;
		break;
	case 0x6A: // {ld l,d}
		L = D;
		NumTicks = 4;
		break;
	case 0x6B: // {ld l,e}
		L = E;
		NumTicks = 4;
		break;
	case 0x6C: // {ld l,h}
		L = H;
		NumTicks = 4;
		break;
	case 0x6D: // {ld l,l}
		NumTicks = 4;
		break;
	case 0x6E: // {ld l,(hl)}
		mRes = memory->getByte(HL, &L);
		NumTicks = 7;
		break;
	case 0x6F: // {ld l,a}
		L = A;
		NumTicks = 4;
		break;
	case 0x70: // {ld (hl),b}
		mRes = memory->setByte(HL, B);
		NumTicks = 7;
		break;
	case 0x71: // {ld (hl),c}
		mRes = memory->setByte(HL, C);
		NumTicks = 7;
		break;
	case 0x72: // {ld (hl),d}
		mRes = memory->setByte(HL, D);
		NumTicks = 7;
		break;
	case 0x73: // {ld (hl),e}
		mRes = memory->setByte(HL, E);
		NumTicks = 7;
		break;
	case 0x74: // {ld (hl),h}
		mRes = memory->setByte(HL, H);
		NumTicks = 7;
		break;
	case 0x75: // {ld (hl),l}
		mRes = memory->setByte(HL, L);
		NumTicks = 7;
		break;
	case 0x76: // {halt}
		HLT = true;
		NumTicks = 4;
		break;
	case 0x77: // ld (hl),a}
		mRes = memory->setByte(HL, A);
		NumTicks = 7;
		break;
	case 0x78: // {ld a,b}
		A = B;
		NumTicks = 4;
		break;
	case 0x79: // {ld a,c}
		A = C;
		NumTicks = 4;
		break;
	case 0x7A: // {ld a,d}
		A = D;
		NumTicks = 4;
		break;
	case 0x7B: // {ld a,e}
		A = E;
		NumTicks = 4;
		break;
	case 0x7C: // {ld a,h}
		A = H;
		NumTicks = 4;
		break;
	case 0x7D: // {ld a,l}
		A = L;
		NumTicks = 4;
		break;
	case 0x7E: // {ld a,(HL)}
		mRes = memory->getByte(HL, &A);
		NumTicks = 7;
		break;
	case 0x7F: // {ld a,a}
		NumTicks = 4;
		break;
	case 0x80: // {add a, b}
		summ(B, 0, 0);
		NumTicks = 4;
		break;
	case 0x81: // {add a, c}
		summ(C, 0, 0);
		NumTicks = 4;
		break;
	case 0x82: // {add a, d}
		summ(D, 0, 0);
		NumTicks = 4;
		break;
	case 0x83: // {add a, e}
		summ(E, 0, 0);
		NumTicks = 4;
		break;
	case 0x84: // {add a, h}
		summ(H, 0, 0);
		NumTicks = 4;
		break;
	case 0x85: // {add a, l}
		summ(L, 0, 0);
		NumTicks = 4;
		break;
	case 0x86: // {add a, (hl)}
		mRes = memory->getByte(HL, &tmp);
		summ(tmp, 0, 0);
		NumTicks = 7;
		break;
	case 0x87: // {add a, a}
		summ(A, 0, 0);
		NumTicks = 4;
		break;
	case 0x88: // {adc a, b}
		summ(B, F & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x89: // {adc a, c}
		summ(C, F & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8A: // {adc a, d}
		summ(D, F & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8B: // {adc a, e}
		summ(E, F & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8C: // {adc a, h}
		summ(H, F & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8D: // {adc a, l}
		summ(L, F & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x8E: // {adc a, (hl)}
		mRes = memory->getByte(HL, &b1);
		summ(b1, F & 0x01, 0);
		NumTicks = 7;
		break;
	case 0x8F: // {adc a, a}
		summ(A, F & 0x01, 0);
		NumTicks = 4;
		break;
	case 0x90: // {sub b}
		summ(B, 0, 1);
		NumTicks = 4;
		break;
	case 0x91: // {sub c}
		summ(C, 0, 1);
		NumTicks = 4;
		break;
	case 0x92: // {sub d}
		summ(D, 0, 1);
		NumTicks = 4;
		break;
	case 0x93: // {sub e}
		summ(E, 0, 1);
		NumTicks = 4;
		break;
	case 0x94: // {sub h}
		summ(H, 0, 1);
		NumTicks = 4;
		break;
	case 0x95: // {sub l}
		summ(L, 0, 1);
		NumTicks = 4;
		break;
	case 0x96: // {sub (hl)}
		mRes = memory->getByte(HL, &b1);
		summ(b1, 0, 1);
		NumTicks = 7;
		break;
	case 0x97: // {sub a}
		summ(A, 0, 1);
		NumTicks = 4;
		break;
	case 0x98: // {sbc a, b}
		summ(B, F & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x99: // {sbc a, c}
		summ(C, F & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9A: // {sbc a, d}
		summ(D, F & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9B: // {sbc a, e}
		summ(E, F & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9C: // {sbc a, h}
		summ(H, F & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9D: // {sbc a, l}
		summ(L, F & 0x01, 1);
		NumTicks = 4;
		break;
	case 0x9E: // {sbc a, (hl)}
		mRes = memory->getByte(HL, &b1);
		summ(b1, F & 0x01, 1);
		NumTicks = 7;
		break;
	case 0x9F: // {sbc a, a}
		summ(A, F & 0x01, 1);
		NumTicks = 4;
		break;
	case 0xA0: // {and b}
		A &= B;
		setfl(true);
		NumTicks = 4;
		break;
	case 0xA1: // {and c}
		A &= C;
		setfl(true);
		NumTicks = 4;
		break;
	case 0xA2: // {and d}
		A &= D;
		setfl(true);
		NumTicks = 4;
		break;
	case 0xA3: // {and e}
		A &= E;
		setfl(true);
		NumTicks = 4;
		break;
	case 0xA4: // {and h}
		A &= H;
		setfl(true);
		NumTicks = 4;
		break;
	case 0xA5: // {and l}
		A &= L;
		setfl(true);
		NumTicks = 4;
		break;
	case 0xA6: // {and (hl)}
		mRes = memory->getByte(HL, &b1);
		A &= b1;
		setfl(true);
		NumTicks = 7;
		break;
	case 0xA7: // {and a}
		A &= A;
		setfl(true);
		NumTicks = 4;
		break;
	case 0xA8: // {xor b}
		A ^= B;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xA9: // {xor c}
		A ^= C;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xAA: // {xor d}
		A ^= D;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xAB: // {xor e}
		A ^= E;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xAC: // {xor h}
		A ^= H;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xAD: // {xor l}
		A ^= L;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xAE: // {xor (hl)}
		mRes = memory->getByte(HL, &b1);
		A ^= b1;
		setfl(false);
		NumTicks = 7;
		break;
	case 0xAF: // {xor a}
		A ^= A;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB0: // {or b}
		A |= B;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB1: // {or c}
		A |= C;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB2: // {or d}
		A |= D;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB3: // {or e}
		A |= E;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB4: // {or h}
		A |= H;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB5: // {or l}
		A |= L;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB6: // {or (hl)}
		mRes = memory->getByte(HL, &b1);
		A |= b1;
		setfl(false);
		NumTicks = 7;
		break;
	case 0xB7: // {or a}
		A |= A;
		setfl(false);
		NumTicks = 4;
		break;
	case 0xB8: // {cp b}
		olda = A;
		summ(B, 0, 1);
		A = olda;
		NumTicks = 4;
		break;
	case 0xB9: // {cp c}
		olda = A;
		summ(C, 0, 1);
		A = olda;
		NumTicks = 4;
		break;
	case 0xBA: // {cp d}
		olda = A;
		summ(D, 0, 1);
		A = olda;
		NumTicks = 4;
		break;
	case 0xBB: // {cp e}
		olda = A;
		summ(E, 0, 1);
		A = olda;
		NumTicks = 4;
		break;
	case 0xBC: // {cp h}
		olda = A;
		summ(H, 0, 1);
		A = olda;
		NumTicks = 4;
		break;
	case 0xBD: // {cp l}
		olda = A;
		summ(L, 0, 1);
		A = olda;
		NumTicks = 4;
		break;
	case 0xBE: // {cp (hl)}
		olda = A;
		mRes = memory->getByte(HL, &dopcode);
		summ(dopcode, 0, 1);
		A = olda;
		NumTicks = 7;
		break;
	case 0xBF: // {cp a}
		olda = A;
		summ(A, 0, 1);
		A = olda;
		NumTicks = 4;
		break;
	case 0xC0: // {ret nz}
		if (FLAG(fZ) == 0)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xC1: // {pop bc}
		mRes = memory->getWordFromStack(sp, &BC);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xC2: // {jp nz, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fZ) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xC3: // {jp addr}
		mRes = memory->getWord(pc, &pc);
		NumTicks = 10;
		break;
	case 0xC4: // {call nz, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fZ) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xC5: // {push bc}
		sp -= 2;
		mRes = memory->setWordToStack(sp, BC);
		NumTicks = 11;
		break;
	case 0xC6: // {add a, n}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, 0, 0);
		NumTicks = 7;
		break;
	case 0xC7: // {rst 00h}
		sp -= 2;
		mRes = memory->setWordToStack(sp, pc);
		pc = 0;
		NumTicks = 11;
		break;
	case 0xC8: // {ret z}
		if (FLAG(fZ) == 1)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xC9: // {ret}
		mRes = memory->getWordFromStack(sp, &pc);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xCA: // {jp z, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fZ) == 1) pc = addr;
		NumTicks = 10;
		break;
	case 0xCB: // prefix bit operation
		NumTicks = bitOps(false, false);
		break;
	case 0xCC: // {call z, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fZ) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xCD: // {call addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		sp -= 2;
		mRes = memory->setWordToStack(sp, pc);
		pc = addr;
		NumTicks = 17;
		break;
	case 0xCE: // {adc a, n}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, FLAG(fC), 0);
		NumTicks = 7;
		break;
	case 0xCF: // {rst 08h}
		sp -= 2;;
		mRes = memory->setWordToStack(sp, pc);
		pc = 8;
		NumTicks = 11;
		break;
	case 0xD0: // {ret nc}
		if (FLAG(fC) == 0)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xD1: // {pop de}
		mRes = memory->getWordFromStack(sp, &DE);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xD2: // {jp nc, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fC) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xD3: // {out (n),a}
	{
		mRes = memory->getByte(pc++, &b1);
		uint16_t portAddr = ((uint16_t)A << 8) | b1;
		ports_out[portAddr] = A;
		if (prt[portAddr] != NULL) prt[portAddr]->setPortData(portAddr, A);
		NumTicks = 11;
	}
		break;
	case 0xD4: // {call nc, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fC) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xD5: // {push de}
		sp -= 2;
		mRes = memory->setWordToStack(sp, DE);
		NumTicks = 11;
		break;
	case 0xD6: // {sub a, n}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, 0, 1);
		NumTicks = 7;
		break;
	case 0xD7: // {rst 10h}
		sp -= 2;;
		mRes = memory->setWordToStack(sp, pc);
		pc = 16;
		NumTicks = 11;
		break;
	case 0xD8: // {ret c}
		if (FLAG(fC) == 1)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xD9: //exx
		if (currentBlock == 0) currentBlock = 1; else currentBlock = 0;
		NumTicks = 4;
		break;
	case 0xDA: // {jp c, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fC) == 1) pc = addr;
		NumTicks = 10;
		break;
	case 0xDB: // {in a, (n)}
	{
		mRes = memory->getByte(pc++, &b1);
		uint16_t portAddr = ((uint16_t)A << 8) | b1;
		A = ports_in[portAddr];
		if (prt[portAddr] != NULL) A = prt[portAddr]->getPortData(portAddr);
		NumTicks = 11;
	}
		break;
	case 0xDC: // {call c, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fC) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xDD: 
		mRes = memory->getByte(pc, &b1);
		if (b1 == 0xCB) {
			pc++;
			NumTicks = bitOps(true,false);
		}
		else {
			NumTicks = ixiyOps(true, false);
		}
		break;
	case 0xDE: // {sbc a, n}
		mRes = memory->getByte(pc++, &b1);
		summ(b1, FLAG(fC), 1);
		NumTicks = 7;
		break;
	case 0xDF: // {rst 18h}
		sp -= 2;;
		mRes = memory->setWordToStack(sp, pc);
		pc = 24;
		NumTicks = 11;
		break;
	case 0xE0: // {ret po}
		if (FLAG(fPV) == 0)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xE1: // {pop hl}
		mRes = memory->getWordFromStack(sp, &HL);
		sp += 2;
		NumTicks = 10;
		break;
	case 0xE2: // {jp po, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fPV) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xE3: // {ex (sp), hl}
		mRes = memory->getWordFromStack(sp, &addr);
		mRes = memory->setWordToStack(sp, HL);
		HL = addr;
		NumTicks = 19;
		break;
	case 0xE4: // {call po, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fPV) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xE5: // {push hl}
		sp -= 2;
		mRes = memory->setWordToStack(sp, HL);
		NumTicks = 11;
		break;
	case 0xE6: // {and n}
		mRes = memory->getByte(pc++, &b1);
		A &= b1;
		setfl(true);
		NumTicks = 7;
		break;
	case 0xE7: // {rst 20h}
		sp -= 2;;
		mRes = memory->setWordToStack(sp, pc);
		pc = 32;
		NumTicks = 11;
		break;
	case 0xE8: // {ret pe}
		if (FLAG(fPV) == 1)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xE9: // {jp (hl)}
		pc = HL;
		NumTicks = 4;
		break;
	case 0xEA: // {jp pe, addr}
		mRes = memory->getWord(pc, &addr);
		if (FLAG(fPV) == 1) pc = addr; else pc += 2;
		NumTicks = 10;
		break;
	case 0xEB: // {ex de, hl}
		addr = HL;
		HL = DE;
		DE = addr;
		NumTicks = 4;
		break;
	case 0xEC: // {call pe, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fPV) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xED:
		NumTicks = miscOps();
		break;
	case 0xEE: // {xor n}
		mRes = memory->getByte(pc++, &b1);
		A ^= b1;
		setfl(false);
		NumTicks = 7;
		break;
	case 0xEF: // {rst 28h}
		sp -= 2;;
		mRes = memory->setWordToStack(sp, pc);
		pc = 40;
		NumTicks = 11;
		break;
	case 0xF0: // {ret p}
		if (FLAG(fS) == 0)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xF1: // {pop af}
		mRes = memory->getWordFromStack(sp, &afl);
		sp += 2;
		F = afl & 0xFF;
		A = afl >> 8;
		NumTicks = 10;
		break;
	case 0xF2: // {jp p, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fS) == 0) pc = addr;
		NumTicks = 10;
		break;
	case 0xF3: // {di}
		INTE = false;
		IFF1 = false;
		IFF2 = false;
		NumTicks = 4;
		break;
	case 0xF4: // {call p, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fS) == 0)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xF5: // {push af}
		sp -= 2;
		afl = A;
		afl <<= 8;
		afl |= F;
		mRes = memory->setWordToStack(sp, afl);
		NumTicks = 11;
		break;
	case 0xF6: // {or n}
		mRes = memory->getByte(pc++, &b1);
		A |= b1;
		setfl(false);
		NumTicks = 7;
		break;
	case 0xF7: // {rst 30h}
		sp -= 2;;
		mRes = memory->setWordToStack(sp, pc);
		pc = 48;
		NumTicks = 11;
		break;
	case 0xF8: // {ret m}
		if (FLAG(fS) == 1)
		{
			mRes = memory->getWordFromStack(sp, &pc);
			sp += 2;
			NumTicks = 11;
		}
		else NumTicks = 5;
		break;
	case 0xF9: // {ld sp, hl}
		sp = HL;
		NumTicks = 6;
		break;
	case 0xFA: // {jp m, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fS) == 1) pc = addr;
		NumTicks = 10;
		break;
	case 0xFB: // {ei}
		INTE = true;
		IFF1 = true;
		IFF2 = true;
		NumTicks = 4;
		break;
	case 0xFC: // {call m, addr}
		mRes = memory->getWord(pc, &addr);
		pc += 2;
		if (FLAG(fS) == 1)
		{
			sp -= 2;
			mRes = memory->setWordToStack(sp, pc);
			pc = addr;
			NumTicks = 17;
		}
		else NumTicks = 10;
		break;
	case 0xFD:
		mRes = memory->getByte(pc, &b1);
		if (b1 == 0xCB) {
			pc++;
			NumTicks = bitOps(false, true);
		}
		else {
			NumTicks = ixiyOps(false, true);
		}
		break;
	case 0xFE: // {cp n}
		mRes = memory->getByte(pc++, &b1);
		olda = A;
		summ(b1, 0, 1);
		A = olda;
		NumTicks = 7;
		break;
	case 0xFF: // {rst 38h}
		sp -= 2;;
		mRes = memory->setWordToStack(sp, pc);
		pc = 56;
		NumTicks = 11;
		break;
	}

	if (mRes) {
		return 0;
	}
	return NumTicks;
}

bool z80::isHalted() {
	return HLT;
}

string getRegNameFromNumRegs(uint8_t numReg) {
	switch (numReg)
	{
	case 0: return "B";
	case 1: return "C";
	case 2: return "D";
	case 3: return "E";
	case 4: return "H";
	case 5: return "L";
	case 6: return "(HL)";
	case 7: return "A";
	}
	return "";
}

std::string z80::bitOpsDasm(uint16_t* addr, bool ix, bool iy) {
	string s = "";

	uint8_t code;
	bool mRes = false;
	uint8_t regData;
	uint8_t numReg;
	string regName = "";
	string srcName = "";
	uint8_t buf[16];
	int8_t data;

	if (!ix && !iy) {
		mRes = memory->getByte((*addr)++, &code);
		if (mRes) return "";
		numReg = code & 0x07;

		regName = getRegNameFromNumRegs(numReg);


	}
	else {
		mRes = memory->getByte((*addr)++, (uint8_t*)&data);
		if (mRes) return "";
		mRes = memory->getByte((*addr)++, &code);
		if (mRes) return "";
		numReg = code & 0x07;
		if (numReg != 6) {
			if (ix) srcName = "(IX+0x" + decToHexByte(data) + "),"; else {
				if (iy) srcName = "(IY+0x" + decToHexByte(data) + "),";
			}
			regName = getRegNameFromNumRegs(numReg);
		}
		else {
			if (ix) srcName = "(IX+0x" + decToHexByte(data) + ")"; else {
				if (iy) srcName = "(IY+0x" + decToHexByte(data) + ")";
			}
			regName = "";
		}
	}

	code >>= 3;
	
	switch (code) {
	case 0x00: // rlc
		s = "RLC " + srcName + regName;
		break;
	case 0x01: // rrc
		s = "RRC " + srcName + regName;
		break;
	case 0x02: // rl
		s = "RL " + srcName + regName;
		break;
	case 0x03: // rr
		s = "RR " + srcName + regName;
		break;
	case 0x04: // sla
		s = "SLA " + srcName + regName;
		break;
	case 0x05: // sra
		s = "SRA " + srcName + regName;
		break;
	case 0x06: // sll
		s = "SLL " + srcName + regName;
		break;
	case 0x07: // srl
		s = "SRL " + srcName + regName;
		break;
	case 0x08:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0e:
	case 0x0f: // bit
		{
			sprintf_s((char*)buf, 16, "%d\0", code - 8);
			std::string btNum((char*)buf);
			if (ix || iy) {
				if (ix) srcName = "(IX+0x" + decToHexByte(data) + ")"; else {
					if (iy) srcName = "(IY+0x" + decToHexByte(data) + ")";
				}
				s = "BIT " + btNum + "," + srcName;
			}
			else {
				s = "BIT " + btNum + "," + regName;
			}
		}
		break;
	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x16:
	case 0x17: // res
		{
			sprintf_s((char*)buf, 16, "%d\0", code - 0x10);
			std::string btNum((char*)buf);
			s = "RES " + btNum + "," + srcName + regName;
		}
		break;
	case 0x18:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x1c:
	case 0x1d:
	case 0x1e:
	case 0x1f: // set
		{
			sprintf_s((char*)buf, 16, "%d\0", code - 0x18);
			std::string btNum((char*)buf);
			s = "SET " + btNum + "," + srcName + regName;
		}
		break;
	}

	return s;
}

std::string z80::miscOpsDasm(uint16_t* addr) {
	string s = "";
	uint8_t code;
	bool mRes = false;
	uint8_t b1;
	uint16_t a1;

	mRes = memory->getByte(*addr, &code);
	if (mRes) {
		return s;
	}
	(*addr)++;

	switch (code)
	{
	case 0x40:			//{ in b,(c) }
		s = s + "IN B,(C)";
		break;
	case 0x41:			//{ out (c),b }
		s = s + "OUT (C),B";
		break;
	case 0x42:			//{ sbc hl,bc }
		s = s + "SBC HL,BC";
		break;
	case 0x43:			//{ ld (addr),bc }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD (" + decToHexWord(a1) + "),BC";
		break;
	case 0x44:			//{ neg }
		s = s + "NEG";
		break;
	case 0x45:			//{ retn }
		s = s + "RETN";
		break;
	case 0x46:			//{ im 0 }
		s = s + "IM 0";
		break;
	case 0x47:			//{ ld i,a }
		s = s + "LD I,A";
		break;
	case 0x48:			//{ in c,(c) }
		s = s + "IN C,(C)";
		break;
	case 0x49:			//{ out (c),c }
		s = s + "OUT (C),C";
		break;
	case 0x4A:			//{ adc hl,bc }
		s = s + "ADC HL,BC";
		break;
	case 0x4B:			//{ ld bc,(addr) }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD BC,(" + decToHexWord(a1) + ")";
		break;
	case 0x4D:			//{ reti }
		s = s + "RETI";
		break;
	case 0x4F:			//{ ld r,a }
		s = s + "LD R,A";
		break;
	case 0x50:			//{ in d,(c) }
		s = s + "IN D,(C)";
		break;
	case 0x51:			//{ out (c),d }
		s = s + "OUT (C),D";
		break;
	case 0x52:			//{ sbc hl,de }
		s = s + "SBC HL,DE";
		break;
	case 0x53:			//{ ld (addr),de }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD (" + decToHexWord(a1) + "),DE";
		break;
	case 0x56:			//{ im 1 }
		s = s + "IM 1";
		break;
	case 0x57:			//{ ld a,i }
		s = s + "LD A,I";
		break;
	case 0x58:			//{ in e,(c) }
		s = s + "IN E,(C)";
		break;
	case 0x59:			//{ out (c),e }
		s = s + "OUT (C),E";
		break;
	case 0x5A:			//{ adc hl,de }
		s = s + "ADC HL,DE";
		break;
	case 0x5B:			//{ ld de,(addr) }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD DE,(" + decToHexWord(a1) + ")";
		break;
	case 0x5E:			//{ im 2 }
		s = s + "IM 2";
		break;
	case 0x5F:			//{ ld a,r }
		s = s + "LD A,R";
		break;
	case 0x60:			//{ in h,(c) }
		s = s + "IN H,(C)";
		break;
	case 0x61:			//{ out (c),h }
		s = s + "OUT (C),H";
		break;
	case 0x62:			//{ sbc hl,hl }
		s = s + "SBC HL,HL";
		break;
	case 0x63:			//{ ld (addr),hl }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD (" + decToHexWord(a1) + "),HL";
		break;
	case 0x67:			//{ rrd }
		s = s + "RRD";
		break;
	case 0x68:			//{ in l,(c) }
		s = s + "IN L,(C)";
		break;
	case 0x69:			//{ out (c),l }
		s = s + "OUT (C),L";
		break;
	case 0x6A:			//{ adc hl,hl }
		s = s + "ADC HL,HL";
		break;
	case 0x6B:			//{ ld hl,(addr) }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD HL,(" + decToHexWord(a1) + ")";
		break;
	case 0x6F:			//{ rld }
		s = s + "RLD";
		break;
	case 0x70:			//{ in (c) }
		s = s + "IN (C)";
		break;
	case 0x71:			//{ out (c),0 }
		s = s + "OUT (C),0";
		break;
	case 0x72:			//{ sbc hl,sp }
		s = s + "SBC HL,SP";
		break;
	case 0x73:			//{ ld (addr),sp }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD (" + decToHexWord(a1) + "),SP";
		break;
	case 0x78:			//{ in a,(c) }
		s = s + "IN A,(C)";
		break;
	case 0x79:			//{ out (c),a }
		s = s + "OUT (C),A";
		break;
	case 0x7A:			//{ adc hl,sp }
		s = s + "ADC HL,SP";
		break;
	case 0x7B:			//{ ld sp,(addr) }
		mRes = memory->getWord(*addr, &a1);
		*addr += 2;
		s = s + "LD SP,(" + decToHexWord(a1) + ")";
		break;
	case 0xA0:			//{ ldi }
		s = s + "LDI";
		break;
	case 0xA1:			//{ cpi }
		s = s + "CPI";
		break;
	case 0xA2:			//{ ini }
		s = s + "INI";
		break;
	case 0xA3:			//{ outi }
		s = s + "OUTI";
		break;
	case 0xA8:			//{ ldd }
		s = s + "LDD";
		break;
	case 0xA9:			//{ cpd }
		s = s + "CPD";
		break;
	case 0xAA:			//{ ind }
		s = s + "IND";
		break;
	case 0xAB:			//{ outd }
		s = s + "OUTD";
		break;
	case 0xB0:			//{ ldir }
		s = s + "LDIR";
		break;
	case 0xB1:			//{ cpir }
		s = s + "CPIR";
		break;
	case 0xB2:			//{ inir }
		s = s + "INIR";
		break;
	case 0xB3:			//{ outir }
		s = s + "OUTIR";
		break;
	case 0xB8:			//{ lddr}
		s = s + "LDDR";
		break;
	case 0xB9:			//{ cpdr }
		s = s + "CPDR";
		break;
	case 0xBA:			//{ indr }
		s = s + "INDR";
		break;
	case 0xBB:			//{ outdr }
		s = s + "OUTDR";
		break;
	}

	return s;
}

std::string z80::ixiyOpsDasm(uint16_t* addr, bool isIX, bool isIY) {

	string s = "";
	uint8_t code, b1, b2;
	uint16_t w1;

	string prefix = "";
	if (isIX) prefix += "IX";
	if (isIY) prefix += "IY";

	bool mRes = memory->getByte(*addr, &code);
	if (mRes) {
		return s;
	}
	(*addr)++;

	switch (code)
	{
	case 0x04: //{inc b}
		s = s + "INC B";
		break;
	case 0x05: //{dec b}
		s = s + "DEC B";
		break;
	case 0x06: //{ld b, d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD B, " + decToHexByte(b1);
		break;
	case 0x09: //{add ix,bc}
		s = s + "ADD " + prefix + ", BC";
		break;
	case 0x0C: // {inc c}
		s = s + "INC C";
		break;
	case 0x0D: //{dec c}
		s = s + "DEC C";
		break;
	case 0x0E: // {ld c, d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD C, " + decToHexByte(b1);
		break;
	case 0x14: // {inc d}
		s = s + "INC D";
		break;
	case 0x15: // {dec d}
		s = s + "DEC D";
		break;
	case 0x16: // {ld d, d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD D, " + decToHexByte(b1);
		break;
	case 0x19: // {add ix, de}
		s = s + "ADD " + prefix + ", DE";
		break;
	case 0x1C: // {inc e}
		s = s + "INC E";
		break;
	case 0x1D: // {dec e}
		s = s + "DEC E";
		break;
	case 0x1E: // {ld e, d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD E, " + decToHexByte(b1);
		break;
	case 0x21: // {ld ix, d16}
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD " + prefix + "," + decToHexWord(w1);
		break;
	case 0x22: // {ld (d16), ix}
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD (" + decToHexWord(w1) + "), " + prefix;
		break;
	case 0x23: // {inc ix}
		s = s + "INC " + prefix;
		break;
	case 0x24: // {inc ixh}
		s = s + "INC " + prefix + "H";
		break;
	case 0x25: /// {dec ixh}
		s = s + "DEC " + prefix + "H";
		break;
	case 0x26: // {ld ixh, d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD " + prefix + "H, " + decToHexByte(b1);
		break;
	case 0x29: // {add ix, ix}
		s = s + "ADD " + prefix + ", " + prefix;
		break;
	case 0x2A: // {ld ix, (addr)}
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD " + prefix + ", (" + decToHexWord(w1) + ")";
		break;
	case 0x2B: // {dec ix}
		s = s + "DEC " + prefix;
		break;
	case 0x2C: // {inc ixl}
		s = s + "INC " + prefix + "L";
		break;
	case 0x2D: // {dec ixl}
		s = s + "DEC " + prefix + "L";
		break;
	case 0x2E: // {ld ixl, d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD " + prefix + "L, " + decToHexByte(b1);
		break;
	case 0x34: // {inc (ix+d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "INC (" + prefix + "+ " + decToHexByte(b1) + ")";
		break;
	case 0x35: // {dec (ix+d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "DEC (" + prefix + "+ " + decToHexByte(b1) + ")";
		break;
	case 0x36: // {ld (ix+d), d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		mRes = memory->getByte(*addr, &b2);
		*addr += 1;
		s = s + "LD (" + prefix + "+ " + decToHexByte(b1) + "), " + decToHexByte(b2);
		break;
	case 0x39: // {add ix, sp}
		s = s + "ADD " + prefix + ", SP";
		break;
	case 0x3C: // {inc a}
		s = s + "INC A";
		break;
	case 0x3D: // {dec a}
		s = s + "DEC A";
		break;
	case 0x3E: // {ld a,b1}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD A, " + decToHexByte(b1);
		break;
	case 0x40: // {ld b,b}
		s = s + "LD B, B";
		break;
	case 0x41: // {ld b,c}
		s = s + "LD B, C";
		break;
	case 0x42: // {ld b,d}
		s = s + "LD B, D";
		break;
	case 0x43: // {ld b,e}
		s = s + "LD B, E";
		break;
	case 0x44: // {ld b,ixh}
		s = s + "LD B, " + prefix + "H";
		break;
	case 0x45: // {ld b,ixl}
		s = s + "LD B, " + prefix + "L";
		break;
	case 0x46: // {ld b,(ix+d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD B, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x47: // {ld b,a}
		s = s + "LD B, A";
		break;
	case 0x48: // {ld c,b}
		s = s + "LD C, B";
		break;
	case 0x49: // {ld c,c}
		s = s + "LD C, C";
		break;
	case 0x4A: // {ld c,d}
		s = s + "LD C, D";
		break;
	case 0x4B: // {ld c,e}
		s = s + "LD C, E";
		break;
	case 0x4C: // {ld c,ixh}
		s = s + "LD C, " + prefix + "H";
		break;
	case 0x4D: // {ld c,ixl}
		s = s + "LD C, " + prefix + "L";
		break;
	case 0x4E: // {ld c,(ix+d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD C, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x4F: // {ld c,a}
		s = s + "LD C, A";
		break;
	case 0x50: // {ld d,b}
		s = s + "LD D, B";
		break;
	case 0x51: // {ld d,c}
		s = s + "LD D, C";
		break;
	case 0x52: // {ld d,d}
		s = s + "LD D, D";
		break;
	case 0x53: // {ld d,e}
		s = s + "LD D, E";
		break;
	case 0x54: // {ld d,ixh}
		s = s + "LD D, " + prefix + "H";
		break;
	case 0x55: // {ld d,ixl}
		s = s + "LD D, " + prefix + "L";
		break;
	case 0x56: // {ld d,(ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD D, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x57: // {ld d,a}
		s = s + "LD D, A";
		break;
	case 0x58: // {ld e,b}
		s = s + "LD E, B";
		break;
	case 0x59: // {ld e,c}
		s = s + "LD E, C";
		break;
	case 0x5A: // {ld e,d}
		s = s + "LD E, D";
		break;
	case 0x5B: // {ld e,e}
		s = s + "LD E, E";
		break;
	case 0x5C: // {ld e,ixh}
		s = s + "LD E, " + prefix + "H";
		break;
	case 0x5D: // {ld e,ixl}
		s = s + "LD E, " + prefix + "L";
		break;
	case 0x5E: // {ld e,(ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD E, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x5F: // {ld e,a}
		s = s + "LD E, A";
		break;
	case 0x60: // {ld ixh,b}
		s = s + "LD " + prefix + "H, B";
		break;
	case 0x61: // {ld ixh,c}
		s = s + "LD " + prefix + "H, C";
		break;
	case 0x62: // {ld ixh,d}
		s = s + "LD " + prefix + "H, D";
		break;
	case 0x63: // {ld ixh,e}
		s = s + "LD " + prefix + "H, E";
		break;
	case 0x64: // {ld ixh,ixh}
		s = s + "LD " + prefix + "H, " + prefix + "H";
		break;
	case 0x65: // {ld ixh,ixl}
		s = s + "LD " + prefix + "H, " + prefix + "L";
		break;
	case 0x66: // {ld h,(ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD H, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x67: // {ld ixh,a}
		s = s + "LD " + prefix + "H, A";
		break;
	case 0x68: // {ld ixl,b}
		s = s + "LD " + prefix + "L, B";
		break;
	case 0x69: // {ld ixl,c}
		s = s + "LD " + prefix + "L, C";
		break;
	case 0x6A: // {ld ixl,d}
		s = s + "LD " + prefix + "L, D";
		break;
	case 0x6B: // {ld ixl,e}
		s = s + "LD " + prefix + "L, E";
		break;
	case 0x6C: // {ld ixl,ixh}
		s = s + "LD " + prefix + "L, " + prefix + "H";
		break;
	case 0x6D: // {ld ixl,ixl}
		s = s + "LD " + prefix + "L, " + prefix + "L";
		break;
	case 0x6E: // {ld l,(ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD L, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x6F: // {ld ixl,a}
		s = s + "LD " + prefix + "L, A";
		break;
	case 0x70: // {ld (ix + d),b}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (" + prefix + " + " + decToHexByte(b1) + "), B";
		break;
	case 0x71: // {ld (ix + d),c}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (" + prefix + " + " + decToHexByte(b1) + "), C";
		break;
	case 0x72: // {ld (ix + d),d}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (" + prefix + " + " + decToHexByte(b1) + "), D";
		break;
	case 0x73: // {ld (ix + d),e}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (" + prefix + " + " + decToHexByte(b1) + "), E";
		break;
	case 0x74: // {ld (ix + d),h}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (" + prefix + " + " + decToHexByte(b1) + "), H";
		break;
	case 0x75: // {ld (ix + d),l}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (" + prefix + " + " + decToHexByte(b1) + "), L";
		break;
	case 0x77: // ld (ix + d),a}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (" + prefix + " + " + decToHexByte(b1) + "), A";
		break;
	case 0x78: // {ld a,b}
		s = s + "LD A, B";
		break;
	case 0x79: // {ld a,c}
		s = s + "LD A, C";
		break;
	case 0x7A: // {ld a,d}
		s = s + "LD A, D";
		break;
	case 0x7B: // {ld a,e}
		s = s + "LD A, E";
		break;
	case 0x7C: // {ld a,ixh}
		s = s + "LD A, " + prefix + "H";
		break;
	case 0x7D: // {ld a,ixl}
		s = s + "LD A, " + prefix + "L";
		break;
	case 0x7E: // {ld a,(ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD A, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x7F: // {ld a,a}
		s = s + "LD A, A";
		break;
	case 0x80: // {add a, b}
		s = s + "ADD A, B";
		break;
	case 0x81: // {add a, c}
		s = s + "ADD A, C";
		break;
	case 0x82: // {add a, d}
		s = s + "ADD A, D";
		break;
	case 0x83: // {add a, e}
		s = s + "ADD A, E";
		break;
	case 0x84: // {add a, ixh}
		s = s + "ADD A, " + prefix + "H";
		break;
	case 0x85: // {add a, ixl}
		s = s + "ADD A, " + prefix + "L";
		break;
	case 0x86: // {add a, (ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ADD A, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x87: // {add a, a}
		s = s + "ADD A, A";
		break;
	case 0x88: // {adc a, b}
		s = s + "ADC A, B";
		break;
	case 0x89: // {adc a, c}
		s = s + "ADC A, C";
		break;
	case 0x8A: // {adc a, d}
		s = s + "ADC A, D";
		break;
	case 0x8B: // {adc a, e}
		s = s + "ADC A, E";
		break;
	case 0x8C: // {adc a, ixh}
		s = s + "ADC A, " + prefix + "H";
		break;
	case 0x8D: // {adc a, ixl}
		s = s + "ADC A, " + prefix + "L";
		break;
	case 0x8E: // {adc a, (ix+d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ADC A, (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x8F: // {adc a, a}
		s = s + "ADC A, A";
		break;
	case 0x90: // {sub b}
		s = s + "SUB B";
		break;
	case 0x91: // {sub c}
		s = s + "SUB C";
		break;
	case 0x92: // {sub d}
		s = s + "SUB D";
		break;
	case 0x93: // {sub e}
		s = s + "SUB E";
		break;
	case 0x94: // {sub ixh}
		s = s + "SUB " + prefix + "H";
		break;
	case 0x95: // {sub ixl}
		s = s + "SUB " + prefix + "L";
		break;
	case 0x96: // {sub (ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "SUB (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x97: // {sub a}
		s = s + "SUB A";
		break;
	case 0x98: // {sbc a, b}
		s = s + "SBC B";
		break;
	case 0x99: // {sbc a, c}
		s = s + "SBC C";
		break;
	case 0x9A: // {sbc a, d}
		s = s + "SBC D";
		break;
	case 0x9B: // {sbc a, e}
		s = s + "SBC E";
		break;
	case 0x9C: // {sbc a, ixh}
		s = s + "SBC " + prefix + "H";
		break;
	case 0x9D: // {sbc a, ixl}
		s = s + "SBC " + prefix + "L";
		break;
	case 0x9E: // {sbc a, (ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "SBC (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0x9F: // {sbc a, a}
		s = s + "SBC A";
		break;
	case 0xA0: // {and b}
		s = s + "AND B";
		break;
	case 0xA1: // {and c}
		s = s + "AND C";
		break;
	case 0xA2: // {and d}
		s = s + "AND D";
		break;
	case 0xA3: // {and e}
		s = s + "AND E";
		break;
	case 0xA4: // {and ixh}
		s = s + "AND " + prefix + "H";
		break;
	case 0xA5: // {and ixl}
		s = s + "AND " + prefix + "L";
		break;
	case 0xA6: // {and (ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "AND (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0xA7: // {and a}
		s = s + "AND A";
		break;
	case 0xA8: // {xor b}
		s = s + "XOR B";
		break;
	case 0xA9: // {xor c}
		s = s + "XOR C";
		break;
	case 0xAA: // {xor d}
		s = s + "XOR D";
		break;
	case 0xAB: // {xor e}
		s = s + "XOR E";
		break;
	case 0xAC: // {xor ixh}
		s = s + "XOR " + prefix + "H";
		break;
	case 0xAD: // {xor ixl}
		s = s + "XOR " + prefix + "L";
		break;
	case 0xAE: // {xor (ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "XOR (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0xAF: // {xor a}
		s = s + "XOR A";
		break;
	case 0xB0: // {or b}
		s = s + "OR B";
		break;
	case 0xB1: // {or c}
		s = s + "OR C";
		break;
	case 0xB2: // {or d}
		s = s + "OR D";
		break;
	case 0xB3: // {or e}
		s = s + "OR E";
		break;
	case 0xB4: // {or ixh}
		s = s + "OR " + prefix + "H";
		break;
	case 0xB5: // {or ixl}
		s = s + "OR " + prefix + "L";
		break;
	case 0xB6: // {or (ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "OR (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0xB7: // {or a}
		s = s + "OR A";
		break;
	case 0xB8: // {cp b}
		s = s + "CP B";
		break;
	case 0xB9: // {cp c}
		s = s + "CP C";
		break;
	case 0xBA: // {cp d}
		s = s + "CP D";
		break;
	case 0xBB: // {cp e}
		s = s + "CP E";
		break;
	case 0xBC: // {cp ixh}
		s = s + "CP " + prefix + "H";
		break;
	case 0xBD: // {cp ixl}
		s = s + "CP " + prefix + "L";
		break;
	case 0xBE: // {cp (ix + d)}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "CP (" + prefix + " + " + decToHexByte(b1) + ")";
		break;
	case 0xBF: // {cp a}
		s = s + "CP A";
		break;
	case 0xCB: // prefix bit operation
		s = s + bitOpsDasm(addr, isIX, isIY);
		break;
	case 0xE1: // {pop ix}
		s = s + "POP " + prefix;
		break;
	case 0xE3: // {ex (sp), ix}
		s = s + "EX (SP), " + prefix;
		break;
	case 0xE5: // {push ix}
		s = s + "PUSH " + prefix;
		break;
	case 0xE9: // {jp (ix)}
		s = s + "JP (" + prefix + ")";
		break;
	case 0xF9: // {ld sp, ix}
		s = s + "LD SP, " + prefix;
		break;
	}

	return s;
}

string z80::dasm(uint16_t* addr)
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
	case 0x00:			//{ nop }
		s = s + "NOP";
		break;
	case 0x01:			 //{ ld bc,d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD BC," + decToHexWord(w1);
		break;
	case 0x02:			 //{ ld (bc), a }
		s = s + "LD (BC), A";
		break;
	case 0x03:			 //{ inc bc }
		s = s + "INC BC";
		break;
	case 0x04:			 //{ inc b }
		s = s + "INC B";
		break;
	case 0x05:			 //{ dec b }
		s = s + "DEC B";
		break;
	case 0x06:			 //{ ld b, n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD B," + decToHexByte(b1);
		break;
	case 0x07:			//{ rlca }
		s = s + "RLCA";
		break;
	case 0x08:			//{ ex af, af` }
		s = s + "EX AF, AF`";
		break;
	case 0x09:			//{ add hl, bc }
		s = s + "ADD HL, BC";
		break;
	case 0x0A:			//{ ld a, (bc) }
		s = s + "LD A, (BC)";
		break;
	case 0x0B:			//{ dec bc }
		s = s + "DEC BC";
		break;
	case 0x0C:			//{ inc c }
		s = s + "INC C";
		break;
	case 0x0D:			//{ dec c }
		s = s + "DEC C";
		break;
	case 0x0E:			//{ ld c, n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD C," + decToHexByte(b1);
		break;
	case 0x0F:			//{ rrca }
		s = s + "RRCA";
		break;
	case 0x10:			//{djnz d8}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "DJNZ " + decToHexByte(b1);
		break;
	case 0x11: //{ ld de, d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD DE," + decToHexWord(w1);
		break;
	case 0x12: //{ ld (de), a }
		s = s + "LD (DE), A";
		break;
	case 0x13: //{ inc de }
		s = s + "INC DE";
		break;
	case 0x14: //{ inc d }
		s = s + "INC D";
		break;
	case 0x15: //{ dec d }
		s = s + "DEC D";
		break;
	case 0x16: //{ ld d, n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD D," + decToHexByte(b1);
		break;
	case 0x17: //{ rla }
		s = s + "RLA";
		break;
	case 0x18:	// { jr d8 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "JR " + decToHexByte(b1);
		break;
	case 0x19: //{ add hl, de }
		s = s + "ADD HL, DE";
		break;
	case 0x1A: //{ ld a, (de) }
		s = s + "LD A, (DE)";
		break;
	case 0x1B: //{ dec de }
		s = s + "DEC DE";
		break;
	case 0x1C: //{ inc e }
		s = s + "INC E";
		break;
	case 0x1D: //{ dec e }
		s = s + "DEC E";
		break;
	case 0x1E: //{ ld e, n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD E," + decToHexByte(b1);
		break;
	case 0x1F: //{ rra }
		s = s + "RRA";
		break;
	case 0x20:	// { jr nz, d8 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "JR NZ," + decToHexByte(b1);
		break;
	case 0x21: //{ ld hl,d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD HL," + decToHexWord(w1);
		break;
	case 0x22: //{ ld (addr), hl }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD (" + decToHexWord(w1) + "), HL";
		break;
	case 0x23: //{ inc hl }
		s = s + "INC HL";
		break;
	case 0x24: //{ inc h }
		s = s + "INC H";
		break;
	case 0x25: //{ dec h }
		s = s + "DEC H";
		break;
	case 0x26: //{ ld h, n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD H," + decToHexByte(b1);
		break;
	case 0x27: //{ daa }
		s = s + "DAA";
		break;
	case 0x28:	// { jr z, d8 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "JR Z," + decToHexByte(b1);
		break;
	case 0x29: //{ add hl,hl }
		s = s + "ADD HL, HL";
		break;
	case 0x2A: //{ ld hl, (addr) }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD HL, (" + decToHexWord(w1) + ")";
		break;
	case 0x2B: //{ dec hl }
		s = s + "DEC HL";
		break;
	case 0x2C: //{ inc l }
		s = s + "INC L";
		break;
	case 0x2D: //{ dec l }
		s = s + "DEC L";
		break;
	case 0x2E: //{ ld l, n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD L," + decToHexByte(b1);
		break;
	case 0x2F: //{ cpl }
		s = s + "CPL";
		break;
	case 0x30:	// { jr nc,d8 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "JR NC," + decToHexByte(b1);
		break;
	case 0x31: //{ ld sp,d16 }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD SP," + decToHexWord(w1);
		break;
	case 0x32: //{ ld (addr), a }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD (" + decToHexWord(w1) + "),A";
		break;
	case 0x33: //{ inc sp }
		s = s + "INC SP";
		break;
	case 0x34: //{ inc (hl) }
		s = s + "INC (HL)";
		break;
	case 0x35: //{ dec (hl) }
		s = s + "DEC (HL)";
		break;
	case 0x36: //{ ld (hl),d8 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD (HL)," + decToHexByte(b1);
		break;
	case 0x37: //{ scf }
		s = s + "SCF";
		break;
	case 0x38:	// { jr c,d8 }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "JR C," + decToHexByte(b1);
		break;
	case 0x39: //{ add hl, sp }
		s = s + "ADD HL, SP";
		break;
	case 0x3A: //{ ld a, (addr) }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD A, (" + decToHexWord(w1) + ")";
		break;
	case 0x3B: //{ dec sp }
		s = s + "DEC SP";
		break;
	case 0x3C: //{ inc a }
		s = s + "INC A";
		break;
	case 0x3D: //{ dec a }
		s = s + "DEC A";
		break;
	case 0x3E: //{ ld a, n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "LD A," + decToHexByte(b1);
		break;
	case 0x3F: //{ ccf }
		s = s + "CCF";
		break;
	case 0x40: //{ ld b,b }
		s = s + "LD B,B";
		break;
	case 0x41: //{ ld b,c }
		s = s + "LD B,C";
		break;
	case 0x42: //{ ld b,d }
		s = s + "LD B,D";
		break;
	case 0x43: //{ ld b,e }
		s = s + "LD B,E";
		break;
	case 0x44: //{ ld b,h }
		s = s + "LD B,H";
		break;
	case 0x45: //{ ld b,l }
		s = s + "LD B,L";
		break;
	case 0x46: //{ ld b,(hl) }
		s = s + "LD B,(HL)";
		break;
	case 0x47: //{ ld b,a }
		s = s + "LD B,A";
		break;
	case 0x48: //{ ld c,b }
		s = s + "LD C,B";
		break;
	case 0x49: //{ ld c,c }
		s = s + "LD C,C";
		break;
	case 0x4A: //{ ld c,d }
		s = s + "LD C,D";
		break;
	case 0x4B: //{ ld c,e }
		s = s + "LD C,E";
		break;
	case 0x4C: //{ ld c,h }
		s = s + "LD C,H";
		break;
	case 0x4D: //{ ld c,l }
		s = s + "LD C,L";
		break;
	case 0x4E: //{ ld c,(hl) }
		s = s + "LD C,(HL)";
		break;
	case 0x4F: //{ ld c,a }
		s = s + "LD C,A";
		break;
	case 0x50: //{ ld d,b }
		s = s + "LD D,B";
		break;
	case 0x51: //{ ld d,c }
		s = s + "LD D,C";
		break;
	case 0x52: //{ ld d,d }
		s = s + "LD D,D";
		break;
	case 0x53: //{ ld d,e }
		s = s + "LD D,E";
		break;
	case 0x54: //{ ld d,h }
		s = s + "LD D,H";
		break;
	case 0x55: //{ ld d,l }
		s = s + "LD D,L";
		break;
	case 0x56: //{ ld d,(hl) }
		s = s + "LD D,(HL)";
		break;
	case 0x57: //{ ld d,a }
		s = s + "LD D,A";
		break;
	case 0x58: //{ ld e,b }
		s = s + "LD E,B";
		break;
	case 0x59: //{ ld e,c }
		s = s + "LD E,C";
		break;
	case 0x5A: //{ ld e,d }
		s = s + "LD E,D";
		break;
	case 0x5B: //{ ld e,e }
		s = s + "LD E,E";
		break;
	case 0x5C: //{ ld e,h }
		s = s + "LD E,H";
		break;
	case 0x5D: //{ ld e,l }
		s = s + "LD E,L";
		break;
	case 0x5E: //{ ld e,(hl) }
		s = s + "LD E,(HL)";
		break;
	case 0x5F: //{ ld e,a }
		s = s + "LD E,A";
		break;
	case 0x60: //{ ld h,b }
		s = s + "LD H,B";
		break;
	case 0x61: //{ ld h,c }
		s = s + "LD H,C";
		break;
	case 0x62: //{ ld h,d }
		s = s + "LD H,D";
		break;
	case 0x63: //{ ld h,e }
		s = s + "LD H,E";
		break;
	case 0x64: //{ ld h,h }
		s = s + "LD H,H";
		break;
	case 0x65: //{ ld h,l }
		s = s + "LD H,L";
		break;
	case 0x66: //{ ld h,(hl) }
		s = s + "LD H,(HL)";
		break;
	case 0x67: //{ ld h,a }
		s = s + "LD H,A";
		break;
	case 0x68: //{ ld l,b }
		s = s + "LD L,B";
		break;
	case 0x69: //{ ld l,c }
		s = s + "LD L,C";
		break;
	case 0x6A: //{ ld l,d }
		s = s + "LD L,D";
		break;
	case 0x6B: //{ ld l,e }
		s = s + "LD L,E";
		break;
	case 0x6C: //{ ld l,h }
		s = s + "LD L,H";
		break;
	case 0x6D: //{ ld l,l }
		s = s + "LD L,L";
		break;
	case 0x6E: //{ ld l,(hl) }
		s = s + "LD L,(HL)";
		break;
	case 0x6F: //{ ld l,a }
		s = s + "LD L,A";
		break;
	case 0x70: //{ ld (hl),b }
		s = s + "LD (HL),B";
		break;
	case 0x71: //{ ld (hl),c }
		s = s + "LD (HL),C";
		break;
	case 0x72: //{ ld (hl),d }
		s = s + "LD (HL),D";
		break;
	case 0x73: //{ ld (hl),e }
		s = s + "LD (HL),E";
		break;
	case 0x74: //{ ld (hl),h }
		s = s + "LD (HL),H";
		break;
	case 0x75: //{ ld (hl),l }
		s = s + "LD (HL),L";
		break;
	case 0x76: //{ halt }
		s = s + "HALT";
		break;
	case 0x77: //{ ld (hl),a }
		s = s + "LD (HL),A";
		break;
	case 0x78: //{ ld a,b }
		s = s + "LD A,B";
		break;
	case 0x79: //{ ld a,c }
		s = s + "LD A,C";
		break;
	case 0x7A: //{ ld a,d }
		s = s + "LD A,D";
		break;
	case 0x7B: //{ ld a,e }
		s = s + "LD A,E";
		break;
	case 0x7C: //{ ld a,h }
		s = s + "LD A,H";
		break;
	case 0x7D: //{ ld a,l }
		s = s + "LD A,L";
		break;
	case 0x7E: //{ ld a,(hl) }
		s = s + "LD A,(HL)";
		break;
	case 0x7F: //{ ld a,a }
		s = s + "LD A,A";
		break;
	case 0x80: //{ add a,b }
		s = s + "ADD A,B";
		break;
	case 0x81: //{ add a,c }
		s = s + "ADD A,C";
		break;
	case 0x82: //{ add a,d }
		s = s + "ADD A,D";
		break;
	case 0x83: //{ add a,e }
		s = s + "ADD A,E";
		break;
	case 0x84: //{ add a,h }
		s = s + "ADD A,H";
		break;
	case 0x85: //{ add a,l }
		s = s + "ADD A,L";
		break;
	case 0x86: //{ add a,(hl) }
		s = s + "ADD A,(HL)";
		break;
	case 0x87: //{ add a,a }
		s = s + "ADD A,A";
		break;
	case 0x88: //{ adc a,b }
		s = s + "ADC A,B";
		break;
	case 0x89: //{ adc a,c }
		s = s + "ADC A,C";
		break;
	case 0x8A: //{ adc a,d }
		s = s + "ADC A,D";
		break;
	case 0x8B: //{ adc a,e }
		s = s + "ADC A,E";
		break;
	case 0x8C: //{ adc a,h }
		s = s + "ADC A,H";
		break;
	case 0x8D: //{ adc a,l }
		s = s + "ADC A,L";
		break;
	case 0x8E: //{ adc a,()hl }
		s = s + "ADC A,(HL)";
		break;
	case 0x8F: //{ adc a,a }
		s = s + "ADC A,A";
		break;
	case 0x90: //{ sub a,b }
		s = s + "SUB A,B";
		break;
	case 0x91: //{ sub a,c }
		s = s + "SUB A,C";
		break;
	case 0x92: //{ sub a,d }
		s = s + "SUB A,D";
		break;
	case 0x93: //{ sub a,e }
		s = s + "SUB A,E";
		break;
	case 0x94: //{ sub a,h }
		s = s + "SUB A,H";
		break;
	case 0x95: //{ sub a,l }
		s = s + "SUB A,L";
		break;
	case 0x96: //{ sub a,(hl) }
		s = s + "SUB A,(HL)";
		break;
	case 0x97: //{ sub a,a }
		s = s + "SUB A,A";
		break;
	case 0x98: //{ sbc a,b }
		s = s + "SBC A,B";
		break;
	case 0x99: //{ sbc a,c }
		s = s + "SBC A,C";
		break;
	case 0x9A: //{ sbc a,d }
		s = s + "SBC A,D";
		break;
	case 0x9B: //{ sbc a,e }
		s = s + "SBC A,E";
		break;
	case 0x9C: //{ sbc a,h }
		s = s + "SBC A,H";
		break;
	case 0x9D: //{ sbc a,l }
		s = s + "SBC A,L";
		break;
	case 0x9E: //{ sbc a,(hl) }
		s = s + "SBC A,(HL)";
		break;
	case 0x9F: //{ sbc a,a }
		s = s + "SBC A,A";
		break;
	case 0xA0: //{ and a,b }
		s = s + "AND A,B";
		break;
	case 0xA1: //{ and a,c }
		s = s + "AND A,C";
		break;
	case 0xA2: //{ and a,d }
		s = s + "AND A,D";
		break;
	case 0xA3: //{ and a,e }
		s = s + "AND A,E";
		break;
	case 0xA4: //{ and a,h }
		s = s + "AND A,H";
		break;
	case 0xA5: //{ and a,l }
		s = s + "AND A,L";
		break;
	case 0xA6: //{ and a,(hl) }
		s = s + "AND A,(HL)";
		break;
	case 0xA7: //{ and a,a }
		s = s + "AND A,A";
		break;
	case 0xA8: //{ xor a,b }
		s = s + "XOR A,B";
		break;
	case 0xA9: //{ xor a,c }
		s = s + "XOR A,C";
		break;
	case 0xAA: //{ xor a,d }
		s = s + "XOR A,D";
		break;
	case 0xAB: //{ xor a,e }
		s = s + "XOR A,E";
		break;
	case 0xAC: //{ xor a,h }
		s = s + "XOR A,H";
		break;
	case 0xAD: //{ xor a,l }
		s = s + "XOR A,L";
		break;
	case 0xAE: //{ xor a,(hl) }
		s = s + "XOR A,(HL)";
		break;
	case 0xAF: //{ xor a,a }
		s = s + "XOR A,A";
		break;
	case 0xB0: //{ or a,b }
		s = s + "OR A,B";
		break;
	case 0xB1: //{ or a,c }
		s = s + "OR A,C";
		break;
	case 0xB2: //{ or a,d }
		s = s + "OR A,D";
		break;
	case 0xB3: //{ or a,e }
		s = s + "OR A,E";
		break;
	case 0xB4: //{ or a,h }
		s = s + "OR A,H";
		break;
	case 0xB5: //{ or a,l }
		s = s + "OR A,L";
		break;
	case 0xB6: //{ or a,(hl) }
		s = s + "OR A,(HL)";
		break;
	case 0xB7: //{ or a,a }
		s = s + "OR A,A";
		break;
	case 0xB8: //{ cp a,b }
		s = s + "CP A,B";
		break;
	case 0xB9: //{ cp a,c }
		s = s + "CP A,C";
		break;
	case 0xBA: //{ cp a,d }
		s = s + "CP A,D";
		break;
	case 0xBB: //{ cp a,e }
		s = s + "CP A,E";
		break;
	case 0xBC: //{ cp a,h }
		s = s + "CP A,H";
		break;
	case 0xBD: //{ cp a,l }
		s = s + "CP A,L";
		break;
	case 0xBE: //{ cp a,(hl) }
		s = s + "CP A,(HL)";
		break;
	case 0xBF: //{ cp a,a }
		s = s + "CP A,A";
		break;
	case 0xC0: //{ ret nz }
		s = s + "RET NZ";
		break;
	case 0xC1: //{ pop bc }
		s = s + "POP BC";
		break;
	case 0xC2: //{ jp nz, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP NZ," + decToHexWord(w1);
		break;
	case 0xC3: //{ jp addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP " + decToHexWord(w1);
		break;
	case 0xC4: //{ call nz, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL NZ," + decToHexWord(w1);
		break;
	case 0xC5: //{ push bc }
		s = s + "PUSH BC";
		break;
	case 0xC6: //{ add a,n}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ADD A," + decToHexByte(b1);
		break;
	case 0xC7: //{ rst 00h }
		s = s + "RST 00h";
		break;
	case 0xC8: //{ ret z }
		s = s + "RET Z";
		break;
	case 0xC9: //{ ret }
		s = s + "RET";
		break;
	case 0xCA: //{ jp z, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP Z," + decToHexWord(w1);
		break;
	case 0xCB: // bit ops
		s = s + bitOpsDasm(addr, false, false);
		break;
	case 0xCC: //{ call z, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL Z," + decToHexWord(w1);
		break;
	case 0xCD: //{ call addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL " + decToHexWord(w1);
		break;
	case 0xCE: //{ adc a,n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "ADC A," + decToHexByte(b1);
		break;
	case 0xCF: //{ rst 08h }
		s = s + "RST 08H";
		break;
	case 0xD0: //{ ret nc }
		s = s + "RET NC";
		break;
	case 0xD1: //{ pop de }
		s = s + "POP DE";
		break;
	case 0xD2: //{ jp nc, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP NC," + decToHexWord(w1);
		break;
	case 0xD3: //{ out (n), a }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "OUT (" + decToHexByte(b1) + "),A";
		break;
	case 0xD4: //{ call nc, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL NC," + decToHexWord(w1);
		break;
	case 0xD5: //{ push de }
		s = s + "PUSH DE";
		break;
	case 0xD6: //{ sub a,n}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "SUB A," + decToHexByte(b1);
		break;
	case 0xD7: //{ rst 10h }
		s = s + "RST 10h";
		break;
	case 0xD8: //{ ret c }
		s = s + "RET C";
		break;
	case 0xD9: //{ exx }
		s = s + "EXX";
		break;
	case 0xDA: //{ jp c, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP C," + decToHexWord(w1);
		break;
	case 0xDB: //{ in a, (port) }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "IN A,(" + decToHexByte(b1) + ")";
		break;
	case 0xDC: //{ call c, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL C," + decToHexWord(w1);
		break;
	case 0xDD: // {IX ops}
		s = s + ixiyOpsDasm(addr, true, false);
		break;
	case 0xDE: //{ sbc a,n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "SBC A," + decToHexByte(b1);
		break;
	case 0xDF: //{ rst 18h }
		s = s + "RST 18H";
		break;
	case 0xE0: //{ ret po }
		s = s + "RET PO";
		break;
	case 0xE1: //{ pop hl }
		s = s + "POP HL";
		break;
	case 0xE2: //{ jp po, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP PO," + decToHexWord(w1);
		break;
	case 0xE3: //{ ex (sp),hl }
		s = s + "EX (SP),HL";
		break;
	case 0xE4: //{ call po, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL PO," + decToHexWord(w1);
		break;
	case 0xE5: //{ push hl }
		s = s + "PUSH HL";
		break;
	case 0xE6: //{ and a, n}
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "AND A," + decToHexByte(b1);
		break;
	case 0xE7: //{ rst 20h }
		s = s + "RST 20h";
		break;
	case 0xE8: //{ ret pe }
		s = s + "RET PE";
		break;
	case 0xE9: //{ jp (hl) }
		s = s + "JP (HL)";
		break;
	case 0xEA: //{ jp pe, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP PE," + decToHexWord(w1);
		break;
	case 0xEB: //{ ex de, dl }
		s = s + "EX DE,HL";
		break;
	case 0xEC: //{ call pe, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL PE," + decToHexWord(w1);
		break;
	case 0xED: //{ misc ops }
		s = s + miscOpsDasm(addr);
		break;
	case 0xEE: //{ xor a,n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "XOR A," + decToHexByte(b1);
		break;
	case 0xEF: //{ rst 28h }
		s = s + "RST 28H";
		break;
	case 0xF0: //{ ret p }
		s = s + "RET P";
		break;
	case 0xF1: //{ pop af }
		s = s + "POP AF";
		break;
	case 0xF2: //{ jp p, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP P," + decToHexWord(w1);
		break;
	case 0xF3: //{ di }
		s = s + "DI";
		break;
	case 0xF4: //{ call p, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL P," + decToHexWord(w1);
		break;
	case 0xF5: //{ push af }
		s = s + "PUSH AF";
		break;
	case 0xF6: //{ or a,n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "OR A," + decToHexByte(b1);
		break;
	case 0xF7: //{ rst 30h }
		s = s + "RST 30H";
		break;
	case 0xF8: //{ ret m }
		s = s + "RET M";
		break;
	case 0xF9: //{ ld sp,hl }
		s = s + "LD SP,HL";
		break;
	case 0xFA: //{ jp m,addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "JP M," + decToHexWord(w1);
		break;
	case 0xFB: //{ ei }
		s = s + "EI";
		break;
	case 0xFC: //{ call m, addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "CALL M," + decToHexWord(w1);
		break;
	case 0xFD: //IY ops
		s = s + ixiyOpsDasm(addr, false, true);
		break;
	case 0xFE: //{ cp a,n }
		mRes = memory->getByte(*addr, &b1);
		*addr += 1;
		s = s + "CP A," + decToHexByte(b1);
		break;
	case 0xFF: //{ rst 38h }
		s = s + "RST 38H";
		break;
	}
	return s;
}

string z80::disAsm(uint16_t* addr, uint8_t n) {
	string res = "";
	for (int i = 0; i < n; i++) {
		res += decToHexWord(*addr);
		res += "  ";
		res += dasm(addr);
		res += "\n";
	}

	return res;
}


void z80::interruptRequest() {
	if (INTE) {
		INTREQ = true;
		HLT = false;
	}
}

uint16_t z80::getPC() {
	return pc;
}

void z80::SetDebugValue(uint16_t tp, uint16_t num, std::string data) {
	uint16_t dta = hexToDec(data);
	if (tp == 1) {
		switch (num) {
		case 0: A = dta;
			break;
		case 1: B = dta;
			break;
		case 2: C = dta;
			break;
		case 3: D = dta;
			break;
		case 4: E = dta;
			break;
		case 5: H = dta;
			break;
		case 6: L = dta;
			break;
		case 7: BC = dta;
			break;
		case 8: DE = dta;
			break;
		case 9: HL = dta;
			break;
		case 10: sp = dta;
			break;
		case 11: pc = dta;
			break;
		case 12: F = dta;
			break;
		}
	}
	if (tp == 2) {
		switch (num) {
		case 0: if (dta == 1) F |= (1 << 7); else F &= (~(1 << 7));
			break;
		case 1: if (dta == 1) F |= (1 << 6); else F &= (~(1 << 6));
			break;
		case 2: if (dta == 1) F |= (1 << 5); else F &= (~(1 << 5));
			break;
		case 3: if (dta == 1) F |= (1 << 4); else F &= (~(1 << 4));
			break;
		case 4: if (dta == 1) F |= (1 << 3); else F &= (~(1 << 3));
			break;
		case 5: if (dta == 1) F |= (1 << 2); else F &= (~(1 << 2));
			break;
		case 6: if (dta == 1) F |= (1 << 1); else F &= (~(1 << 1));
			break;
		case 7: if (dta == 1) F |= 1; else F &= 0xFE;
			break;
		}
	}
}


string z80::delSpace(string data) {
	string nData = "";
	int i = 0;
	if (data[0] == 0x20) {
		while (data[i] == 0x20)
		{
			i++;
			if (i >= data.length()) break;
		}
	}
	while (data[i] != 0x20)
	{
		nData += data[i];
		i++;
		if (i >= data.length()) return nData;
	}
	nData += data[i];
	i++;
	while (i < data.length())
	{
		if (data[i] != 0x20) nData += data[i];
		i++;
	}

	return nData;
}

string z80::getData(string data, int n) {
	string inst = "";
	int i = 0;
	while (i < data.length()) {
		if (data[i] == 0x20) break;
		inst += data[i];
		i++;
	}
	if (n == 1) return inst;
	i++;
	inst = "";
	while (i < data.length()) {
		if (data[i] == ',') break;
		inst += data[i];
		i++;
		if (i >= data.length()) break;
	}
	if (n == 2) return inst;
	i++;
	inst = "";
	while (i < data.length()) {
		if (data[i] == ',') break;
		inst += data[i];
		i++;
		if (i >= data.length()) break;
	}
	if (n == 3) return inst;
	i++;
	inst = "";
	while (i < data.length()) {
		if (data[i] == ',') break;
		inst += data[i];
		i++;
		if (i >= data.length()) break;
	}
	if (n == 4) return inst;
	return "";
}

int z80::getParamIndex(string data) {
	if (data == "B") return 0;
	if (data == "C") return 1;
	if (data == "D") return 2;
	if (data == "E") return 3;
	if (data == "H") return 4;
	if (data == "L") return 5;
	if (data == "(HL)") return 6;
	if (data == "A") return 7;
	if (data == "BC") return 8;
	if (data == "DE") return 9;
	if (data == "HL") return 10;
	if (data == "SP") return 11;
	if (data.find("(IX+", 0) != std::string::npos) return 12;
	if (data.find("(IY+", 0) != std::string::npos) return 13;
	if (data == "IX") return 14;
	if (data == "IY") return 15;
	if (data == "NZ") return 16;
	if (data == "Z") return 17;
	if (data == "NC") return 18;
	if (data == "C") return 19;
	if (data == "PO") return 20;
	if (data == "PE") return 21;
	if (data == "P") return 22;
	if (data == "M") return 23;
	if (data == "(SP)") return 24;
	if (data == "AF") return 25;
	if (data == "AF`") return 26;
	if (data == "(C)") return 27;
	if (data == "(IX)") return 28;
	if (data == "(IY)") return 29;
	if (data == "(BC)") return 30;
	if (data == "(DE)") return 31;
	if (data == "I") return 32;
	if (data == "R") return 33;

	return -1;
}

int z80::getHexData(string data) {
	int res = 0;
	for (int i = 0; i < data.length(); i++) {
		if ((data[i] >= '0' && data[i] <= '9') || (data[i] >= 'A' && data[i] <= 'F')) {
			uint8_t p = 0;
			if (data[i] >= 'A') p = data[i] - 55; else p = data[i] - 48;
			res = ((res << 4) | p);
		}
		else return -1;
	}
	return res;
}

int z80::getIndexData(string data) {
	string p = "";
	for (int i = 4; i < data.length(); i++) {
		if (data[i] == ')') break;
		p += data[i];
	}
	return getHexData(p);
}

int z80::getMemoryData(string data) {
	string p = "";
	if (data[0] == '(') {
		for (int i = 1; i < data.length(); i++) {
			if (data[i] == ')') break;
			p += data[i];
		}
		return getHexData(p);
	}
	else return 0;
}


void z80::SetAssemblerValue(std::string addr, std::string value) {
	int res = 0;
	uint8_t buf[5];

	uint16_t dAddr = hexToDec(addr);
	printf("Use address: %d\n", dAddr);

	for (auto& c : value) c = toupper(c);
	printf("new value: %s\n", value.c_str());
	
	if (!value.length()) return;

	std::string cmd = delSpace(value);
	//printf("%s\n", cmd.c_str());
	string inst = getData(cmd, 1);
	//printf("%s\n", inst.c_str());
	string p1 = getData(cmd, 2);
	string p2 = getData(cmd, 3);
	string p3 = getData(cmd, 4);
	//printf("%s\n", p1.c_str());
	//printf("%s\n", p2.c_str());
	//printf("%s\n", p3.c_str());


	if (inst == "ADC") {
		//printf("Found adc command\n");
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == 7 && pi2 >= 0 && pi2 < 8) {
				buf[res++] = 0x88 | pi2;
			}
			if (pi1 == 7 && (pi2 == -1 || pi2 == 8 || pi2 == 9)) {
				int p = getHexData(p2);
				if (p >= 0 && p < 256) {
					buf[res++] = 0xCE;
					buf[res++] = p;
				}
			}
			if (pi1 == 7 && (pi2 == 12 || pi2 == 13)) {
				int p = getIndexData(p2);
				if (p >= 0 && p < 256) {
					if (pi2 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
					buf[res++] = 0x8E;
					buf[res++] = p;
				}
			}
			if (pi1 == 10 && pi2 >= 8 && pi2 <= 11) {
				buf[res++] = 0xED;
				buf[res++] = 0x40 | ((pi2 - 8) << 4) | 0x0A;
			}
		}
	}
	if (inst == "ADD") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == 7 && pi2 >= 0 && pi2 < 8) {
				buf[res++] = 0x80 | pi2;
			}
			if (pi1 == 7 && (pi2 == -1 || pi2 == 8 || pi2 == 9)) {
				int p = getHexData(p2);
				if (p >= 0 && p < 256) {
					buf[res++] = 0xC6;
					buf[res++] = p;
				}
			}
			if (pi1 == 7 && (pi2 == 12 || pi2 == 13)) {
				int p = getIndexData(p2);
				if (p >= 0 && p < 256) {
					if (pi2 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
					buf[res++] = 0x86;
					buf[res++] = p;
				}
			}
			if (pi1 == 10 && pi2 >= 8 && pi2 <= 11) {
				buf[res++] = ((pi2 - 8) << 4) | 0x09;
			}
			if (!((pi1 == 14 && pi2 == 15) || (pi1 == 15 && pi2 == 14))) {
				if ((pi1 == 14 || pi1 == 15) && ((pi2 >= 8 && pi2 <= 11 && pi2 != 10) || (pi2 == 14 || pi2 == 15))) {
					if (pi1 == 14) buf[res++] = 0xDD; else buf[res++] = 0xFD;
					if (pi2 == 14) pi2 -= 4;
					if (pi2 == 15) pi2 -= 5;
					buf[res++] = ((pi2 - 8) << 4) | 0x09;
				}
			}
		}
	}
	if (inst == "AND") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xA0 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xA6;
				buf[res++] = p;
			}
			if (pi1 == -1 || pi1 == 8 || pi1 == 9) {
				int p = getHexData(p1);
				if (p >= 0 && p < 256) {
					buf[res++] = 0xE6;
					buf[res++] = p;
				}
			}
		}
	}
	if (inst == "BIT") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == -1) {
				int p = getHexData(p1);
				if (p >= 0 && p <= 7) {
					if (pi2 >= 0 && pi2 <= 7) {
						buf[res++] = 0xCB;
						buf[res++] = 0x40 | (p << 3) | pi2;
					}
					if (pi2 == 12 || pi2 == 13) {
						if (pi2 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
						buf[res++] = 0xCB;
						int pp = getIndexData(p2);
						buf[res++] = pp;
						buf[res++] = 0x40 | (p << 3) | 0x06;
					}
				}
			}

		}
	}
	if (inst == "CALL") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if ((pi1 >= 16 && pi1 <= 23) || pi1 == 1) {
				if (pi1 == 1) pi1 = 19; else pi1 -= 16;
				if (pi2 == -1) {
					int p = getHexData(p2);
					if (p >= 0 && p <= 0xFFFF) {
						buf[res++] = 0xC0 | (pi1 << 3) | 0x04;
						buf[res++] = p & 0xFF;
						buf[res++] = (p & 0xFF00) >> 8;
					}
				}
			}
		}
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 == -1) {
				int p = getHexData(p1);
				if (p >= 0 && p <= 0xFFFF) {
					buf[res++] = 0xCD;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
			}
		}
	}
	if (inst == "CCF") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x3F;
		}
	}
	if (inst == "CP") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xB8 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xBE;
				buf[res++] = p;
			}
			if (pi1 == -1 || pi1 == 8 || pi1 == 9) {
				int p = getHexData(p1);
				if (p >= 0 && p < 256) {
					buf[res++] = 0xFE;
					buf[res++] = p;
				}
			}
		}
	}
	if (inst == "CPD") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xA9;
		}
	}
	if (inst == "CPDR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xB9;
		}
	}
	if (inst == "CPI") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xA1;
		}
	}
	if (inst == "CPIR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xB1;
		}
	}
	if (inst == "CPL") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x2F;
		}
	}
	if (inst == "DAA") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x27;
		}
	}
	if (inst == "DEC") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = (pi1 << 3) | 0x05;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (p >= 0 && p < 256) {
					if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
					buf[res++] = 0x35;
					buf[res++] = p;
				}
			}
			if (pi1 >= 8 && pi1 <= 11) {
				buf[res++] = ((pi1 - 8) << 4) | 0x0B;
			}
			if (pi1 == 14 || pi1 == 15) {
				if (pi1 == 14) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0x2B;
			}
		}
	}
	if (inst == "DI") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xF3;
		}
	}
	if (inst == "DJNZ") {
		if (p1 != "" && p2 == "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 == -1 || pi1 == 8 || pi1 == 9) {
				int p = getHexData(p1);
				if (p >= 0 && p < 256) {
					buf[res++] = 0x10;
					buf[res++] = p;
				}
			}
		}
	}
	if (inst == "EI") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xFB;
		}
	}
	if (inst == "EX") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == 9 && pi2 == 10) {
				buf[res++] = 0xEB;
			}
			if (pi1 == 25 && pi2 == 26) {
				buf[res++] = 0x08;
			}
			if (pi1 == 24) {
				if (pi2 == 10 || pi2 == 14 || pi2 == 15) {
					if (pi2 == 14) buf[res++] = 0xDD;
					if (pi2 == 15) buf[res++] = 0xFD;
					buf[res++] = 0xE3;
				}
			}
		}
	}
	if (inst == "EXX") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xD9;
		}
	}
	if (inst == "HALT") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x76;
		}
	}
	if (inst == "IM") {
		if (p1 != "" && p2 == "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 == -1) {
				int p = getHexData(p1);
				if (p >= 0 && p <= 2) {
					buf[res++] = 0xED;
					switch (p) {
					case 0: buf[res++] = 0x46;
						break;
					case 1: buf[res++] = 0x56;
						break;
					case 2: buf[res++] = 0x5E;
						break;
					}
				}
			}
		}
	}
	if (inst == "IN") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == 7 && pi2 == -1) {
				buf[res++] = 0xDB;
				buf[res++] = getMemoryData(p2);
			}
			if (pi1 >= 0 && pi1 <= 7 && pi2 == 27) {
				buf[res++] = 0xED;
				buf[res++] = 0x40 | (pi1 << 3);
			}
			if (p1 == "F" && pi2 == 27) {
				buf[res++] = 0xED;
				buf[res++] = 0x70;
			}
		}
	}
	if (inst == "INC") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = (pi1 << 3) | 0x04;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (p >= 0 && p < 256) {
					if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
					buf[res++] = 0x34;
					buf[res++] = p;
				}
			}
			if (pi1 >= 8 && pi1 <= 11) {
				buf[res++] = ((pi1 - 8) << 4) | 0x03;
			}
			if (pi1 == 14 || pi1 == 15) {
				if (pi1 == 14) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0x23;
			}
		}
	}
	if (inst == "IND") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xAA;
		}
	}
	if (inst == "INDR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xBA;
		}
	}
	if (inst == "INI") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xA2;
		}
	}
	if (inst == "INIR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xB2;
		}
	}
	if (inst == "JP") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if ((pi1 >= 16 && pi1 <= 23) || pi1 == 1) {
				if (pi1 == 1) pi1 = 19; else pi1 -= 16;
				if (pi2 == -1) {
					int p = getHexData(p2);
					if (p >= 0 && p <= 0xFFFF) {
						buf[res++] = 0xC0 | (pi1 << 3) | 0x02;
						buf[res++] = p & 0xFF;
						buf[res++] = (p & 0xFF00) >> 8;
					}
				}
			}
		}
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 == -1) {
				int p = getHexData(p1);
				if (p >= 0 && p <= 0xFFFF) {
					buf[res++] = 0xC3;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
			}
			if (pi1 == 6 || pi1 == 28 || pi1 == 29) {
				if (pi1 == 28) buf[res++] = 0xDD;
				if (pi1 == 29) buf[res++] = 0xFD;
				buf[res++] = 0xE9;
			}
		}
	}
	if (inst == "JR") {
		if (p1 != "" && p2 == "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 == -1 || pi1 == 8 || pi1 == 9) {
				int p = getHexData(p1);
				buf[res++] = 0x18;
				buf[res++] = p;
			}
		}
		if (p1 != "" && p2 != "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (((pi1 >= 16 && pi1 <= 18) || (pi1 == 1)) && (pi2 == -1 || pi2 == 8 || pi2 == 9)) {
				int p = getHexData(p2);
				switch (pi1)
				{
				case 16: buf[res++] = 0x20;
					break;
				case 17: buf[res++] = 0x28;
					break;
				case 18: buf[res++] = 0x30;
					break;
				case 1: buf[res++] = 0x38;
					break;
				}
				buf[res++] = p;
			}
		}
	}
	if (inst == "LD") {
		if (p1 != "" && p2 != "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 >= 0 && pi1 <= 7 && pi2 >= 0 && pi2 <= 7) {
				buf[res++] = 0x40 | (pi1 << 3) | pi2;
			}
			if ((pi1 >= 0 && pi1 <= 7) && (pi2 == -1 || pi2 == 8 || pi2 == 9)) {
				int p = getHexData(p2);
				if (p >= 0 && p <= 255 && p2[0] != '(') {
					buf[res++] = (pi1 << 3) | 0x06;
					buf[res++] = p;
				}
				else {
					if (pi1 == 7 && p2[0] == '(') {
						int p = getMemoryData(p2);
						buf[res++] = 0x3A;
						buf[res++] = p & 0xFF;
						buf[res++] = (p & 0xFF00) >> 8;
					}
				}
			}
			if (pi1 >= 0 && pi1 <= 7 && (pi2 == 12 || pi2 == 13)) {
				if (pi2 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0x40 | (pi1 << 3) | 0x06;
				int p = getIndexData(p2);
				buf[res++] = p;
			}
			if (((pi2 >= 0 && pi2 <= 7) || (pi2 == -1 || pi2 == 8 || pi2 == 9)) && (pi1 == 12 || pi1 == 13)) {
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				if ((pi2 >= 0 && pi2 <= 7)) buf[res++] = 0x70 | pi2;
				if (pi2 == -1 || pi2 == 8 || pi2 == 9) buf[res++] = 0x36;
				int p = getIndexData(p1);
				buf[res++] = p;
				if (pi2 == -1 || pi2 == 8 || pi2 == 9) {
					int p = getHexData(p2);
					buf[res++] = p;
				}
			}
			if (pi1 == 7 && (pi2 == 30 || pi2 == 31)) {
				buf[res++] = 0x0A | ((pi2 - 30) << 4);
			}
			if (pi2 == 7 && (pi1 == 30 || pi1 == 31)) {
				buf[res++] = 0x02 | ((pi1 - 30) << 4);
			}
			if (pi1 == -1 && ((pi2 >= 7 && pi2 <= 11) || pi2 == 14 || pi2 == 15)) {
				int p = getMemoryData(p1);
				if (pi2 == 7) {
					buf[res++] = 0x32;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
				if (pi2 == 10) {
					buf[res++] = 0x22;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
				if (pi2 == 8 || pi2 == 9 || pi2 == 11) {
					buf[res++] = 0xED;
					buf[res++] = 0x40 | ((pi2 - 8) << 4) | 0x03;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
				if (pi2 == 14 || pi2 == 15) {
					if (pi2 == 14) buf[res++] = 0xDD; else buf[res++] = 0xFD;
					buf[res++] = 0x22;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
			}
			if (pi1 == 7 && (pi2 == 32 || pi2 == 33)) {
				buf[res++] = 0xED;
				if (pi2 == 32) buf[res++] = 0x57; else buf[res++] = 0x5F;
			}
			if (pi2 == 7 && (pi1 == 32 || pi1 == 33)) {
				buf[res++] = 0xED;
				if (pi1 == 32) buf[res++] = 0x47; else buf[res++] = 0x4F;
			}
			if (pi1 >= 8 && pi1 <= 11 && pi2 == -1) {
				if (p2[0] == '(') {
					int p = getMemoryData(p2);
					if (pi1 == 10) {
						buf[res++] = 0x2A;
						buf[res++] = p & 0xFF;
						buf[res++] = (p & 0xFF00) >> 8;
					}
					else {
						buf[res++] = 0xED;
						buf[res++] = 0x40 | ((pi1 - 8) << 4) | 0x0B;
						buf[res++] = p & 0xFF;
						buf[res++] = (p & 0xFF00) >> 8;
					}
				}
				else {
					int p = getHexData(p2);
					buf[res++] = ((pi1 - 8) << 4) | 0x01;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
			}
			if (pi1 == 11 && pi2 == 10) {
				buf[res++] = 0xF9;
			}
			if (pi1 == 11 && pi2 == 14) {
				buf[res++] = 0xDD;
				buf[res++] = 0xF9;
			}
			if (pi1 == 11 && pi2 == 15) {
				buf[res++] = 0xFD;
				buf[res++] = 0xF9;
			}
			if ((pi1 == 14 || pi1 == 15) && pi2 == -1) {
				if (pi1 == 14) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				if (p2[0] == '(') {
					int p = getMemoryData(p2);
					buf[res++] = 0x2A;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
				else {
					int p = getHexData(p2);
					buf[res++] = 0x21;
					buf[res++] = p & 0xFF;
					buf[res++] = (p & 0xFF00) >> 8;
				}
			}
		}
	}
	if (inst == "LDD") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xA8;
		}
	}
	if (inst == "LDDR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xB8;
		}
	}
	if (inst == "LDI") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xA0;
		}
	}
	if (inst == "LDIR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xB0;
		}
	}
	if (inst == "NEG") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0x44;
		}
	}
	if (inst == "NOP") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x00;
		}
	}
	if (inst == "OR") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xB0 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xB6;
				buf[res++] = p;
			}
			if (pi1 == -1 || pi1 == 8 || pi1 == 9) {
				int p = getHexData(p1);
				if (p >= 0 && p < 256) {
					buf[res++] = 0xF6;
					buf[res++] = p;
				}
			}
		}
	}
	if (inst == "OTDR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xBB;
		}
	}
	if (inst == "OTIR") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xB3;
		}
	}
	if (inst == "OUT") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi2 == 7 && pi1 == -1) {
				buf[res++] = 0xD3;
				buf[res++] = getMemoryData(p1);
			}
			if (pi2 >= 0 && pi2 <= 7 && pi1 == 27) {
				buf[res++] = 0xED;
				buf[res++] = 0x40 | (pi2 << 3) | 0x01;
			}
			if (p2 == "0" && pi1 == 27) {
				buf[res++] = 0xED;
				buf[res++] = 0x71;
			}
		}
	}
	if (inst == "OUTD") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xAB;
		}
	}
	if (inst == "OUTI") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0xA3;
		}
	}
	if (inst == "POP") {
		if (p1 != "" && p2 == "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 8 && pi1 <= 10 || pi1 == 25) {
				if (pi1 == 25) pi1 = 3; else pi1 -= 8;
				buf[res++] = 0xC0 | (pi1 << 4) | 0x01;
			}
			if (pi1 == 14 || pi1 == 15) {
				if (pi1 == 14) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xE1;
			}
		}
	}
	if (inst == "PUSH") {
		if (p1 != "" && p2 == "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 8 && pi1 <= 10 || pi1 == 25) {
				if (pi1 == 25) pi1 = 3; else pi1 -= 8;
				buf[res++] = 0xC0 | (pi1 << 4) | 0x05;
			}
			if (pi1 == 14 || pi1 == 15) {
				if (pi1 == 14) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xE5;
			}
		}
	}
	if (inst == "RES") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == -1) {
				int p = getHexData(p1);
				if (p >= 0 && p <= 7) {
					if (pi2 >= 0 && pi2 <= 7) {
						buf[res++] = 0xCB;
						buf[res++] = 0x80 | (p << 3) | pi2;
					}
					if (pi2 == 12 || pi2 == 13) {
						if (pi2 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
						buf[res++] = 0xCB;
						int pp = getIndexData(p2);
						buf[res++] = pp;
						buf[res++] = 0x80 | (p << 3) | 0x06;
					}
				}
			}
		}
	}
	if (inst == "RET") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if ((pi1 >= 16 && pi1 <= 23) || pi1 == 1) {
				if (pi1 == 1) pi1 = 19; else pi1 -= 16;
				buf[res++] = 0xC0 | (pi1 << 3);
			}
		}
		if (p1 == "" && p2 == "") {
			buf[res++] = 0xC9;
		}
	}
	if (inst == "RETI") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0x4D;
		}
	}
	if (inst == "RETN") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0x45;
		}
	}
	if (inst == "RL") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = 0x10 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x16;
			}
		}
	}
	if (inst == "RLA") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x17;
		}
	}
	if (inst == "RLC") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x06;
			}
		}
	}
	if (inst == "RLCA") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x07;
		}
	}
	if (inst == "RLD") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0x6F;
		}
	}
	if (inst == "RR") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = 0x18 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x1E;
			}
		}
	}
	if (inst == "RRA") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x1F;
		}
	}
	if (inst == "RRC") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = 0x08 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x0E;
			}
		}
	}
	if (inst == "RRCA") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x0F;
		}
	}
	if (inst == "RRD") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0xED;
			buf[res++] = 0x67;
		}
	}
	if (inst == "RST") {
		if (p1 != "" && p2 == "" && p3 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 == -1) {
				int p = getHexData(p1);
				if (p == 0 || p == 8 || p == 0x10 || p == 0x18 || p == 0x20 || p == 0x28 || p == 0x30 || p == 0x38) {
					buf[res++] = 0xC7 | p;
				}
			}
		}
	}
	if (inst == "SBC") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == 7 && (pi2 >= 0 && pi2 < 8)) {
				buf[res++] = 0x98 | pi2;
			}
			if (pi1 == 7 && (pi2 == -1 || pi2 == 8 || pi2 == 9)) {
				int p = getHexData(p2);
				if (p >= 0 && p <= 255) {
					buf[res++] = 0xDE;
					buf[res++] = p;
				}
			}
			if (pi1 == 7 && (pi2 == 12 || pi2 == 13)) {
				int p = getIndexData(p2);
				if (pi2 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0x9E;
				buf[res++] = p;
			}
			if (pi1 == 10 && pi2 >= 8 && pi2 <= 11) {
				buf[res++] = 0xED;
				buf[res++] = 0x40 | ((pi2 - 8) << 4) | 0x02;
			}
		}
	}
	if (inst == "SCF") {
		if (p1 == "" && p2 == "" && p3 == "") {
			buf[res++] = 0x37;
		}
	}
	if (inst == "SET") {
		if (p1 != "" && p2 != "") {
			int pi1 = getParamIndex(p1);
			int pi2 = getParamIndex(p2);
			if (pi1 == -1) {
				int p = getHexData(p1);
				if (p >= 0 && p <= 7) {
					if (pi2 >= 0 && pi2 <= 7) {
						buf[res++] = 0xCB;
						buf[res++] = 0xC0 | (p << 3) | pi2;
					}
					if (pi2 == 12 || pi2 == 13) {
						if (pi2 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
						buf[res++] = 0xCB;
						int pp = getIndexData(p2);
						buf[res++] = pp;
						buf[res++] = 0xC0 | (p << 3) | 0x06;
					}
				}
			}
		}
	}
	if (inst == "SLA") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = 0x20 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x26;
			}
		}
	}
	if (inst == "SLL") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = 0x30 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x36;
			}
		}
	}
	if (inst == "SRA") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = 0x28 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x2E;
			}
		}
	}
	if (inst == "SRL") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xCB;
				buf[res++] = 0x38 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xCB;
				buf[res++] = p;
				buf[res++] = 0x3E;
			}
		}
	}
	if (inst == "SUB") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0x90 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0x96;
				buf[res++] = p;
			}
			if (pi1 == -1 || pi1 == 8 || pi1 == 9) {
				int p = getHexData(p1);
				if (p >= 0 && p < 256) {
					buf[res++] = 0xD6;
					buf[res++] = p;
				}
			}
		}
	}
	if (inst == "XOR") {
		if (p1 != "" && p2 == "") {
			int pi1 = getParamIndex(p1);
			if (pi1 >= 0 && pi1 < 8) {
				buf[res++] = 0xA8 | pi1;
			}
			if (pi1 == 12 || pi1 == 13) {
				int p = getIndexData(p1);
				if (pi1 == 12) buf[res++] = 0xDD; else buf[res++] = 0xFD;
				buf[res++] = 0xAE;
				buf[res++] = p;
			}
			if (pi1 == -1 || pi1 == 8 || pi1 == 9) {
				int p = getHexData(p1);
				if (p >= 0 && p < 256) {
					buf[res++] = 0xEE;
					buf[res++] = p;
				}
			}
		}
	}
	
	for (int i=0;i<res;i++) memory->setDebugByte(dAddr+i, buf[i]);
}

void z80::setPort(uint16_t num, Ports* dev) {
	prt[num] = dev;
}

uint8_t z80::getPort(uint8_t num) {
	return ports_out[num];
}

uint16_t z80::getPort(uint16_t num) {
	return 0;
}