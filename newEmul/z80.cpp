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

#define SET(n) (F | (1 << n))
#define CLEAR(n) (F & (!(1 << n)))
#define FLAG(n) ((F & (1 << n)) >> n)


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
	ir.r16 = 0;
	ix = 0;
	iy = 0;
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
	reg.registers[reg.count].data = (uint32_t)ir.r16;
	reg.registers[reg.count++].strData = decToHexWord(ir.r16);
	reg.registers[reg.count].name = "IX";
	reg.registers[reg.count].data = (uint32_t)ix;
	reg.registers[reg.count++].strData = decToHexWord(ix);
	reg.registers[reg.count].name = "IY";
	reg.registers[reg.count].data = (uint32_t)iy;
	reg.registers[reg.count++].strData = decToHexWord(iy);
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
	ir.r16 = 0;
	ix = 0;
	iy = 0;
	currentBlock = 0;
	currentAF = 0;
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

uint8_t z80::bitOps() {
	return 0;
}
uint8_t z80::miscOps() {
	return 0;
}

uint8_t z80::ixOps() {
	return 0;
}

uint8_t z80::iyOps() {
	return 0;
}

uint8_t z80::execute() {
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
		mRes = memory->getByte(pc, &b1);
		B--;
		if (B == 0) {
			pc++;
			NumTicks = 8;
		}
		else {
			pc--;
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
		mRes = memory->getByte(pc, &b1);
		pc--;
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
		mRes = memory->getByte(pc, &b1);
		pc--;
		if (F & Zf == 0) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			pc += 2;
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
		mRes = memory->getByte(pc, &b1);
		pc--;
		if (F & Zf == Zf) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			pc += 2;
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
		F = (uint8_t)((F & (Sf | Zf | PVf | Cf) ^ Cf) | (((A = ~A) & (F3f | F5f)) | Hf | Nf));
		NumTicks = 4;
		break;
	case 0x30: // jr nc, d8
		mRes = memory->getByte(pc, &b1);
		pc--;
		if (F & Cf == 0) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			pc += 2;
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
		mRes = memory->getByte(pc, &b1);
		pc--;
		if (F & Cf == Cf) {
			pc += (int8_t)b1;
			NumTicks = 12;
		}
		else {
			pc += 2;
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
		D = E;
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
		uint8_t tmp;
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
		sp -= 2;;
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
		NumTicks = bitOps();
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
		if (prt[portAddr] != NULL) A = prt[portAddr]->getPortData(b1);
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
		NumTicks = ixOps();
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
		if (FLAG(fPV) == 1) pc = addr;
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
		NumTicks = iyOps();
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

std::string z80::bitOpsDasm(uint16_t* addr) {
	string s = "";

	return s;
}

std::string z80::miscOpsDasm(uint16_t* addr) {
	string s = "";

	return s;
}

std::string z80::ixOpsDasm(uint16_t* addr) {
	string s = "";

	return s;
}

std::string z80::iyOpsDasm(uint16_t* addr) {
	string s = "";

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
	case 0x2A: //{ ld (hl), addr }
		mRes = memory->getWord(*addr, &w1);
		*addr += 2;
		s = s + "LD (HL)," + decToHexWord(w1);
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
		s = s + bitOpsDasm(addr);
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
		s = s + ixOpsDasm(addr);
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
		s = s + iyOpsDasm(addr);
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

std::string z80::getParam(std::string s, int* n) {
	std::string res = "";
	int i = *n;
	if (i < s.length()) {
		while (s[i] == ' ' || s[i] == ',')
		{
			i++;
			if (i == s.length()) break;
		}
		while (i < s.length())
		{
			if ((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'A' && s[i] <= 'Z')) res += s[i]; else break;
			i++;
		}
	}
	*n = i;
	return res;
}

uint8_t z80::GetIdRegsToName(std::string n) {
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

uint8_t z80::GetIdRegsPairToName(std::string n) {
	if (n == "B") return 0x00;
	if (n == "D") return 0x10;
	if (n == "H") return 0x20;
	if (n == "SP") return 0x30;
	if (n == "PSW") return 0x30;
	return 255;
}

void z80::SetAssemblerValue(std::string addr, std::string value) {

	uint16_t dAddr = hexToDec(addr);
	printf("Use address: %d\n", dAddr);

	for (auto& c : value) c = toupper(c);
	printf("new value: %s\n", value.c_str());

	uint8_t paramCount = 1;
	for (int i = 0; i < value.length(); i++) if (value[i] == ' ' || value[i] == ',') paramCount++;
	printf("Params count: %d\n", paramCount);

	std::string cmd = "";
	int i = 0;
	while (i < value.length())
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

void z80::setPort(uint16_t num, Ports* dev) {
	prt[num] = dev;
}

uint8_t z80::getPort(uint8_t num) {
	return ports_out[num];
}

uint16_t z80::getPort(uint16_t num) {
	return 0;
}