

#include "vector06c.h"
#include <Windows.h>
#include "hex.h"

void Vector06c::recalculateToVector() {
	if (commandTicksCount == 4) return;
	if (commandTicksCount == 5) {
		commandTicksCount = 8;
		return;
	}
	if (commandTicksCount == 7) {
		commandTicksCount = 8;
		return;
	}
	if (commandTicksCount == 10) {
		commandTicksCount = 12;
		return;
	}
	if (commandTicksCount == 11) {
		commandTicksCount = 16;
		return;
	}
	if (commandTicksCount == 13) {
		commandTicksCount = 16;
		return;
	}
	if (commandTicksCount == 16) {
		commandTicksCount = 20;
		return;
	}
	if (commandTicksCount == 17) {
		commandTicksCount = 24;
		return;
	}
	if (commandTicksCount == 18) {
		commandTicksCount = 24;
		return;
	}
	printf("error timings for vector06c: %d\n", commandTicksCount);
	exit(0);
}

void Vector06c::timer() {
	
	if (enabled) {
		//if (clockCount % 2 == 0) {
		//	timervi53->step();
		//}
		if (clockCount % 4 == 0) {
			display->syncDisplay();
		}
		clockCount++;
		if (!cycle) {
			cycle = true;
			commandTicksCount = cpu->execute();
			recalculateToVector();
		}
		if (cycle) {
			//if ((tickCount & 0x03) == 0) {
			//	display->syncDisplay();
			//}
			tickCount++;
			if (tickCount == commandTicksCount) {
				wavPlayer->playSample(commandTicksCount);
				snd->soundSteps(commandTicksCount, keyboard->getTapeOut(),wavPlayer->getCurrentSample());
				tickCount = 0;
				cycle = false;
			}
		}
	}
}

void Vector06c::trace() {

	tickCount = 0;
	do {
		do {
			if ((tickCount % 4 == 0) == 0) {
				display->syncDisplay();
			}
			if (!cycle) {
				cycle = true;
				commandTicksCount = cpu->execute();
				recalculateToVector();
			}
			if (cycle) {
				tickCount++;
				if (tickCount == commandTicksCount) {
					tickCount = 0;
					cycle = false;
				}
			}
		} while (tickCount);
	} while (cpu->isHalted());
}

void Vector06c::step() {
	uint16_t curaddr = cpu->getPC();
	uint8_t iCode;
	mem->getByte(curaddr, &iCode);
	switch (iCode)
	{
	case 0xCD:
	case 0xDD:
	case 0xED:
	case 0xFD:
	case 0xC4:
	case 0xCC:
	case 0xD4:
	case 0xDC:
	case 0xE4:
	case 0xEC:
	case 0xF4:
	case 0xFC: {
		do {
			trace();
			printf("Cur addr: %04X, start addr: %04X\n", cpu->getPC(), curaddr);
			if ((cpu->getPC() - curaddr) == 3) break;
		} while (1);
		break;
	}
	default:
		trace();
	}
}

void Vector06c::stepTo(std::string addr) {
	uint16_t stopAddr = hexToDec(addr);
	printf("Run to addr: %04X(%d)\n", stopAddr, stopAddr);
	do {
		trace();
	} while (cpu->getPC() != stopAddr);
}

Vector06c::Vector06c() {

}

Vector06c::Vector06c(SDL_Renderer* rendr, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, WAV* wav, PROFILE prof)
{
	renderer = rendr;
	enabled = false;
	wavPlayer = wav;
	clock = new Timer(3000000);
	if (prof.ramdisk) {
		kvazidisk = new kvaz();
	}
	else kvazidisk = NULL;
	mem = new Memory(65536, 5, 0, prof.ROMPath, kvazidisk);
	cpu = new i8080(mem);
	uint8_t* vMem = NULL;
	vMem = mem->getVideoMemoryPointer();
	if (prof.ramdisk) {
		tPorts kvazPrt = kvazidisk->getPorts();
		for (int i = 0; i < kvazPrt.count; i++) {
			cpu->setPort(kvazPrt.ports[i], kvazidisk);
		}
	}
	keyboard = new Keyboard(wav);
	display = new VectorDisplay(renderer, vMem, cpu, callback, keyboard);
	tPorts prts = display->getPorts();
	for (int i = 0; i < prts.count; i++) {
		cpu->setPort(prts.ports[i], display);
	}
	tPorts krts = keyboard->getPorts();
	for (int i = 0; i < krts.count; i++) {
		cpu->setPort(krts.ports[i], keyboard);
	}
	timervi53 = new K580VI53();
	tPorts trts = timervi53->getPorts();
	for (int i = 0; i < trts.count; i++) {
		cpu->setPort(trts.ports[i], timervi53);
	}
	snd = new sound(3000000, timervi53);
	cpu->reset();
	clockCount = 0;
	tickCount = 0;
	commandTicksCount = 0;
	cycle = false;
	clock->start(this, true);
	snd->pause(false);
}

Vector06c::~Vector06c()
{
	snd->pause(true);
	tickCount = 0;
	commandTicksCount = 0;
	enabled = false;
	cycle = false;
	clock->stop();
	Sleep(1000);
	delete snd;
	delete timervi53;
	delete display;
	delete cpu;
	delete mem;
	delete kvazidisk;
	delete clock;
	wavPlayer = NULL;
}

void Vector06c::start() {
	enabled = true;
}

void Vector06c::stop() {
	enabled = false;
}

bool Vector06c::getStatus() {
	return enabled;
}

CPU* Vector06c::getCPU() {
	return cpu;
}

Memory* Vector06c::getMemory() {
	return mem;
}

void Vector06c::keyDown(uint32_t keyCode) {
	uint8_t res = keyboard->keyDown(keyCode);
	if (res == 1) {
		mem->changeROMStatus(false);
		cpu->reset();
	}
	if (res == 2) {
		mem->resetMainMemory();
		mem->changeROMStatus(true);
		cpu->reset();
	}
}

void Vector06c::keyUp(uint32_t keyCode) {
	keyboard->keyUp(keyCode);
}

void Vector06c::loadProgramToMemory(std::string url, bool addr0) {
	FILE* f;
	f = fopen(url.c_str(), "rb");
	if (f) {
		uint8_t* buf = new uint8_t[65536];

		int res = fread(buf, 1, 65536, f);
		bool oldEnabled = enabled;
		enabled = false;
		mem->setMemory(buf, addr0 ? 0: 0x100, res);
		mem->changeROMStatus(false);
		cpu->reset();
		enabled = oldEnabled;

		delete[] buf;
		fclose(f);
	}
}

void Vector06c::loadDataToMemory(std::string url, int tp) {
	FILE* f;
	f = fopen(url.c_str(), "rb");
	if (f) {
		uint8_t* buf = new uint8_t[65536];

		int res = fread(buf, 1, 65536, f);

		if (tp == 1) {
			mem->setMemory(buf, 0x4300, res);
		}

		delete[] buf;
		fclose(f);
	}

}

vKeybParams Vector06c::getVirtualKeyboardParams() {
	vKeybParams keyb;
	keyb.winWidth = 870;
	keyb.winHeigth = 270;
	
	keyb.keyCount = 70;
	keyb.keys[0].x = 10;
	keyb.keys[0].y = 10;
	keyb.keys[0].width = 49;
	keyb.keys[0].heigth = 49;
	keyb.keys[0].cap = ";\n+";
	keyb.keys[0].code = 0x33;

	keyb.keys[1].x = 60;
	keyb.keys[1].y = 10;
	keyb.keys[1].width = 49;
	keyb.keys[1].heigth = 49;
	keyb.keys[1].cap = "1\n!";
	keyb.keys[1].code = 0x1E;

	keyb.keys[2].x = 110;
	keyb.keys[2].y = 10;
	keyb.keys[2].width = 49;
	keyb.keys[2].heigth = 49;
	keyb.keys[2].cap = "2\n\"";
	keyb.keys[2].code = 0x1F;
	
	keyb.keys[3].x = 160;
	keyb.keys[3].y = 10;
	keyb.keys[3].width = 49;
	keyb.keys[3].heigth = 49;
	keyb.keys[3].cap = "3\n#";
	keyb.keys[3].code = 0x20;

	keyb.keys[4].x = 210;
	keyb.keys[4].y = 10;
	keyb.keys[4].width = 49;
	keyb.keys[4].heigth = 49;
	keyb.keys[4].cap = "4\n¤";
	keyb.keys[4].code = 0x21;

	keyb.keys[5].x = 260;
	keyb.keys[5].y = 10;
	keyb.keys[5].width = 49;
	keyb.keys[5].heigth = 49;
	keyb.keys[5].cap = "5\n%";
	keyb.keys[5].code = 0x22;

	keyb.keys[6].x = 310;
	keyb.keys[6].y = 10;
	keyb.keys[6].width = 49;
	keyb.keys[6].heigth = 49;
	keyb.keys[6].cap = "6\n&";
	keyb.keys[6].code = 0x23;

	keyb.keys[7].x = 360;
	keyb.keys[7].y = 10;
	keyb.keys[7].width = 49;
	keyb.keys[7].heigth = 49;
	keyb.keys[7].cap = "7\n'";
	keyb.keys[7].code = 0x24;

	keyb.keys[8].x = 410;
	keyb.keys[8].y = 10;
	keyb.keys[8].width = 49;
	keyb.keys[8].heigth = 49;
	keyb.keys[8].cap = "8\n(";
	keyb.keys[8].code = 0x25;

	keyb.keys[9].x = 460;
	keyb.keys[9].y = 10;
	keyb.keys[9].width = 49;
	keyb.keys[9].heigth = 49;
	keyb.keys[9].cap = "9\n)";
	keyb.keys[9].code = 0x26;

	keyb.keys[10].x = 510;
	keyb.keys[10].y = 10;
	keyb.keys[10].width = 49;
	keyb.keys[10].heigth = 49;
	keyb.keys[10].cap = "0\n ";
	keyb.keys[10].code = 0x27;

	keyb.keys[11].x = 560;
	keyb.keys[11].y = 10;
	keyb.keys[11].width = 49;
	keyb.keys[11].heigth = 49;
	keyb.keys[11].cap = "-\n=";
	keyb.keys[11].code = 0x2D;

	keyb.keys[12].x = 610;
	keyb.keys[12].y = 10;
	keyb.keys[12].width = 49;
	keyb.keys[12].heigth = 49;
	keyb.keys[12].cap = "/\n?";
	keyb.keys[12].code = 0x38;
	
	keyb.keys[13].x = 35;
	keyb.keys[13].y = 60;
	keyb.keys[13].width = 49;
	keyb.keys[13].heigth = 49;
	keyb.keys[13].cap = "É\nJ";
	keyb.keys[13].code = 0x0D;

	keyb.keys[14].x = 85;
	keyb.keys[14].y = 60;
	keyb.keys[14].width = 49;
	keyb.keys[14].heigth = 49;
	keyb.keys[14].cap = "Ö\nC";
	keyb.keys[14].code = 0x06;

	keyb.keys[15].x = 135;
	keyb.keys[15].y = 60;
	keyb.keys[15].width = 49;
	keyb.keys[15].heigth = 49;
	keyb.keys[15].cap = "Ó\nU";
	keyb.keys[15].code = 0x18;

	keyb.keys[16].x = 185;
	keyb.keys[16].y = 60;
	keyb.keys[16].width = 49;
	keyb.keys[16].heigth = 49;
	keyb.keys[16].cap = "Ê\nK";
	keyb.keys[16].code = 0x0E;

	keyb.keys[17].x = 235;
	keyb.keys[17].y = 60;
	keyb.keys[17].width = 49;
	keyb.keys[17].heigth = 49;
	keyb.keys[17].cap = "Å\nE";
	keyb.keys[17].code = 0x08;

	keyb.keys[18].x = 285;
	keyb.keys[18].y = 60;
	keyb.keys[18].width = 49;
	keyb.keys[18].heigth = 49;
	keyb.keys[18].cap = "Í\nN";
	keyb.keys[18].code = 0x11;

	keyb.keys[19].x = 335;
	keyb.keys[19].y = 60;
	keyb.keys[19].width = 49;
	keyb.keys[19].heigth = 49;
	keyb.keys[19].cap = "Ã\nG";
	keyb.keys[19].code = 0x0A;

	keyb.keys[20].x = 385;
	keyb.keys[20].y = 60;
	keyb.keys[20].width = 49;
	keyb.keys[20].heigth = 49;
	keyb.keys[20].cap = "Ø\n[";
	keyb.keys[20].code = 0x2F;

	keyb.keys[21].x = 435;
	keyb.keys[21].y = 60;
	keyb.keys[21].width = 49;
	keyb.keys[21].heigth = 49;
	keyb.keys[21].cap = "Ù\n]";
	keyb.keys[21].code = 0x30;

	keyb.keys[22].x = 485;
	keyb.keys[22].y = 60;
	keyb.keys[22].width = 49;
	keyb.keys[22].heigth = 49;
	keyb.keys[22].cap = "Ç\nZ";
	keyb.keys[22].code = 0x1D;

	keyb.keys[23].x = 535;
	keyb.keys[23].y = 60;
	keyb.keys[23].width = 49;
	keyb.keys[23].heigth = 49;
	keyb.keys[23].cap = "Õ\nH";
	keyb.keys[23].code = 0x0B;

	keyb.keys[24].x = 585;
	keyb.keys[24].y = 60;
	keyb.keys[24].width = 49;
	keyb.keys[24].heigth = 49;
	keyb.keys[24].cap = ":\n*";
	keyb.keys[24].code = 0x33;

	keyb.keys[25].x = 10;
	keyb.keys[25].y = 110;
	keyb.keys[25].width = 49;
	keyb.keys[25].heigth = 49;
	keyb.keys[25].cap = "ÓÑ";
	keyb.keys[25].code = 0xE0;

	keyb.keys[26].x = 60;
	keyb.keys[26].y = 110;
	keyb.keys[26].width = 49;
	keyb.keys[26].heigth = 49;
	keyb.keys[26].cap = "Ô\nF";
	keyb.keys[26].code = 0x09;

	keyb.keys[27].x = 110;
	keyb.keys[27].y = 110;
	keyb.keys[27].width = 49;
	keyb.keys[27].heigth = 49;
	keyb.keys[27].cap = "Û\nY";
	keyb.keys[27].code = 0x1C;

	keyb.keys[28].x = 160;
	keyb.keys[28].y = 110;
	keyb.keys[28].width = 49;
	keyb.keys[28].heigth = 49;
	keyb.keys[28].cap = "Â\nW";
	keyb.keys[28].code = 0x1A;

	keyb.keys[29].x = 210;
	keyb.keys[29].y = 110;
	keyb.keys[29].width = 49;
	keyb.keys[29].heigth = 49;
	keyb.keys[29].cap = "À\nA";
	keyb.keys[29].code = 0x04;

	keyb.keys[30].x = 260;
	keyb.keys[30].y = 110;
	keyb.keys[30].width = 49;
	keyb.keys[30].heigth = 49;
	keyb.keys[30].cap = "Ï\nP";
	keyb.keys[30].code = 0x13;

	keyb.keys[31].x = 310;
	keyb.keys[31].y = 110;
	keyb.keys[31].width = 49;
	keyb.keys[31].heigth = 49;
	keyb.keys[31].cap = "Ð\nR";
	keyb.keys[31].code = 0x15;

	keyb.keys[32].x = 360;
	keyb.keys[32].y = 110;
	keyb.keys[32].width = 49;
	keyb.keys[32].heigth = 49;
	keyb.keys[32].cap = "Î\nO";
	keyb.keys[32].code = 0x12;

	keyb.keys[33].x = 410;
	keyb.keys[33].y = 110;
	keyb.keys[33].width = 49;
	keyb.keys[33].heigth = 49;
	keyb.keys[33].cap = "Ë\nL";
	keyb.keys[33].code = 0x0F;

	keyb.keys[34].x = 460;
	keyb.keys[34].y = 110;
	keyb.keys[34].width = 49;
	keyb.keys[34].heigth = 49;
	keyb.keys[34].cap = "Ä\nD";
	keyb.keys[34].code = 0x07;

	keyb.keys[35].x = 510;
	keyb.keys[35].y = 110;
	keyb.keys[35].width = 49;
	keyb.keys[35].heigth = 49;
	keyb.keys[35].cap = "Æ\nV";
	keyb.keys[35].code = 0x19;

	keyb.keys[36].x = 560;
	keyb.keys[36].y = 110;
	keyb.keys[36].width = 49;
	keyb.keys[36].heigth = 49;
	keyb.keys[36].cap = "Ý\n\\";
	keyb.keys[36].code = 0x31;

	keyb.keys[37].x = 610;
	keyb.keys[37].y = 110;
	keyb.keys[37].width = 49;
	keyb.keys[37].heigth = 49;
	keyb.keys[37].cap = ".\n>";
	keyb.keys[37].code = 0x37;

	keyb.keys[38].x = 10;
	keyb.keys[38].y = 160;
	keyb.keys[38].width = 74;
	keyb.keys[38].heigth = 49;
	keyb.keys[38].cap = "ÑÑ";
	keyb.keys[38].code = 0xE1;

	keyb.keys[39].x = 85;
	keyb.keys[39].y = 160;
	keyb.keys[39].width = 49;
	keyb.keys[39].heigth = 49;
	keyb.keys[39].cap = "ß\nQ";
	keyb.keys[39].code = 0x14;

	keyb.keys[40].x = 135;
	keyb.keys[40].y = 160;
	keyb.keys[40].width = 49;
	keyb.keys[40].heigth = 49;
	keyb.keys[40].cap = "×\n^";// ` (òèëüäà)
	keyb.keys[40].code = 0x35;

	keyb.keys[41].x = 185;
	keyb.keys[41].y = 160;
	keyb.keys[41].width = 49;
	keyb.keys[41].heigth = 49;
	keyb.keys[41].cap = "Ñ\nS";
	keyb.keys[41].code = 0x16;

	keyb.keys[42].x = 235;
	keyb.keys[42].y = 160;
	keyb.keys[42].width = 49;
	keyb.keys[42].heigth = 49;
	keyb.keys[42].cap = "Ì\nM";
	keyb.keys[42].code = 0x10;

	keyb.keys[43].x = 285;
	keyb.keys[43].y = 160;
	keyb.keys[43].width = 49;
	keyb.keys[43].heigth = 49;
	keyb.keys[43].cap = "È\nI";
	keyb.keys[43].code = 0x0C;

	keyb.keys[44].x = 335;
	keyb.keys[44].y = 160;
	keyb.keys[44].width = 49;
	keyb.keys[44].heigth = 49;
	keyb.keys[44].cap = "Ò\nT";
	keyb.keys[44].code = 0x17;

	keyb.keys[45].x = 385;
	keyb.keys[45].y = 160;
	keyb.keys[45].width = 49;
	keyb.keys[45].heigth = 49;
	keyb.keys[45].cap = "Ü\nX";
	keyb.keys[45].code = 0x1B;

	keyb.keys[46].x = 435;
	keyb.keys[46].y = 160;
	keyb.keys[46].width = 49;
	keyb.keys[46].heigth = 49;
	keyb.keys[46].cap = "Á\nB";
	keyb.keys[46].code = 0x05;

	keyb.keys[47].x = 485;
	keyb.keys[47].y = 160;
	keyb.keys[47].width = 49;
	keyb.keys[47].heigth = 49;
	keyb.keys[47].cap = "Þ\n@"; //' (Ý)
	keyb.keys[47].code = 0x34;

	keyb.keys[48].x = 535;
	keyb.keys[48].y = 160;
	keyb.keys[48].width = 49;
	keyb.keys[48].heigth = 49;
	keyb.keys[48].cap = ",\n<";
	keyb.keys[48].code = 0x36;

	keyb.keys[49].x = 585;
	keyb.keys[49].y = 160;
	keyb.keys[49].width = 74;
	keyb.keys[49].heigth = 49;
	keyb.keys[49].cap = "ÂÊ";
	keyb.keys[49].code = 0x28;

	keyb.keys[50].x = 10;
	keyb.keys[50].y = 210;
	keyb.keys[50].width = 74;
	keyb.keys[50].heigth = 49;
	keyb.keys[50].cap = "ÐÓÑ\nËÀÒ";
	keyb.keys[50].code = 0x39;

	keyb.keys[51].x = 85;
	keyb.keys[51].y = 210;
	keyb.keys[51].width = 74;
	keyb.keys[51].heigth = 49;
	keyb.keys[51].cap = "ÒÀÁ";
	keyb.keys[51].code = 0x2B;

	keyb.keys[52].x = 160;
	keyb.keys[52].y = 210;
	keyb.keys[52].width = 349;
	keyb.keys[52].heigth = 49;
	keyb.keys[52].cap = "";
	keyb.keys[52].code = 0x2C;

	keyb.keys[53].x = 510;
	keyb.keys[53].y = 210;
	keyb.keys[53].width = 74;
	keyb.keys[53].heigth = 49;
	keyb.keys[53].cap = "ÏÑ";
	keyb.keys[53].code = 0x58; // alt enter (on num pad keyboard)

	keyb.keys[54].x = 585;
	keyb.keys[54].y = 210;
	keyb.keys[54].width = 74;
	keyb.keys[54].heigth = 49;
	keyb.keys[54].cap = "ÇÁ\n_";
	keyb.keys[54].code = 0x2A;

	keyb.keys[55].x = 710;
	keyb.keys[55].y = 10;
	keyb.keys[55].width = 49;
	keyb.keys[55].heigth = 49;
	keyb.keys[55].cap = "ÂÂÎÄ";
	keyb.keys[55].code = 0x3F;

	keyb.keys[56].x = 760;
	keyb.keys[56].y = 10;
	keyb.keys[56].width = 49;
	keyb.keys[56].heigth = 49;
	keyb.keys[56].cap = "ÁËÊ";
	keyb.keys[56].code = 0x40;

	keyb.keys[57].x = 810;
	keyb.keys[57].y = 10;
	keyb.keys[57].width = 49;
	keyb.keys[57].heigth = 49;
	keyb.keys[57].cap = "ÑÁÐ";
	keyb.keys[57].code = 0x41;

	keyb.keys[58].x = 710;
	keyb.keys[58].y = 60;
	keyb.keys[58].width = 49;
	keyb.keys[58].heigth = 49;
	keyb.keys[58].cap = "F1";
	keyb.keys[58].code = 0x3A;

	keyb.keys[59].x = 760;
	keyb.keys[59].y = 60;
	keyb.keys[59].width = 49;
	keyb.keys[59].heigth = 49;
	keyb.keys[59].cap = "F2";
	keyb.keys[59].code = 0x3B;

	keyb.keys[60].x = 810;
	keyb.keys[60].y = 60;
	keyb.keys[60].width = 49;
	keyb.keys[60].heigth = 49;
	keyb.keys[60].cap = "F3";
	keyb.keys[60].code = 0x3C;

	keyb.keys[61].x = 710;
	keyb.keys[61].y = 110;
	keyb.keys[61].width = 49;
	keyb.keys[61].heigth = 49;
	keyb.keys[61].cap = "F4";
	keyb.keys[61].code = 0x3D;

	keyb.keys[62].x = 760;
	keyb.keys[62].y = 110;
	keyb.keys[62].width = 49;
	keyb.keys[62].heigth = 49;
	keyb.keys[62].cap = "F5";
	keyb.keys[62].code = 0x3E;

	keyb.keys[63].x = 810;
	keyb.keys[63].y = 110;
	keyb.keys[63].width = 49;
	keyb.keys[63].heigth = 49;
	keyb.keys[63].cap = "ÀÐ2";
	keyb.keys[63].code = 0x29;

	keyb.keys[64].x = 710;
	keyb.keys[64].y = 160;
	keyb.keys[64].width = 49;
	keyb.keys[64].heigth = 49;
	keyb.keys[64].cap = "^\\";
	keyb.keys[64].code = 0x49;//ins

	keyb.keys[65].x = 760;
	keyb.keys[65].y = 160;
	keyb.keys[65].width = 49;
	keyb.keys[65].heigth = 49;
	keyb.keys[65].cap = "^\n|";
	keyb.keys[65].code = 0x52;

	keyb.keys[66].x = 810;
	keyb.keys[66].y = 160;
	keyb.keys[66].width = 49;
	keyb.keys[66].heigth = 49;
	keyb.keys[66].cap = "ÑÒÐ";
	keyb.keys[66].code = 0x4C;//del

	keyb.keys[67].x = 710;
	keyb.keys[67].y = 210;
	keyb.keys[67].width = 49;
	keyb.keys[67].heigth = 49;
	keyb.keys[67].cap = "<-";
	keyb.keys[67].code = 0x50;

	keyb.keys[68].x = 760;
	keyb.keys[68].y = 210;
	keyb.keys[68].width = 49;
	keyb.keys[68].heigth = 49;
	keyb.keys[68].cap = "|\n^";
	keyb.keys[68].code = 0x51;

	keyb.keys[69].x = 810;
	keyb.keys[69].y = 210;
	keyb.keys[69].width = 49;
	keyb.keys[69].heigth = 49;
	keyb.keys[69].cap = "->";
	keyb.keys[69].code = 0x4F;

	return keyb;
}