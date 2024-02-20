

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
		clockCount++;
		if (!cycle) {
			cycle = true;
			commandTicksCount = cpu->execute();
			recalculateToVector();
		}
		if (cycle) {
			if ((tickCount & 0x03) == 0) {
				display->syncDisplay();
			}
			tickCount++;
			if (tickCount == commandTicksCount) {
				wavPlayer->playSample(commandTicksCount);
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
			if (!cycle) {
				cycle = true;
				commandTicksCount = cpu->execute();
				recalculateToVector();
			}
			if (cycle) {
				if ((tickCount & 0x03) == 0) {
					display->syncDisplay();
				}
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
	do {
		trace();
	} while ((curaddr > cpu->getPC() && curaddr <= cpu->getPC() + 3));
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

Vector06c::Vector06c(SDL_Renderer* rendr, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, WAV* wav)
{
	renderer = rendr;
	enabled = false;
	wavPlayer = wav;
	clock = new Timer(3000000);
	mem = new Memory(65536, 5, 0, "d:\\boots.bin");
	cpu = new i8080(mem);
	uint8_t* vMem = NULL;
	vMem = mem->getVideoMemoryPointer();
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
	cpu->reset();
	clockCount = 0;
	tickCount = 0;
	commandTicksCount = 0;
	cycle = false;
	clock->start(this, true);
}

Vector06c::~Vector06c()
{
	tickCount = 0;
	commandTicksCount = 0;
	enabled = false;
	cycle = false;
	clock->stop();
	Sleep(1000);
	delete display;
	delete cpu;
	delete mem;
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
		mem->changeROMStatus(true);
		cpu->reset();
	}
}

void Vector06c::keyUp(uint32_t keyCode) {
	keyboard->keyUp(keyCode);
}
