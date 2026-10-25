#include "radio86rk.h"
#include <Windows.h>
#include "hex.h"

#include "radio86rk.h"


Radio86rk::Radio86rk() {

}

Radio86rk::Radio86rk(SDL_Renderer* rendr, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, WAV* wav, PROFILE prof, Breakpoints* bp) {

}

Radio86rk::~Radio86rk() {

}

void Radio86rk::timer() {

}

CPU* Radio86rk::getCPU() {
	return cpu;
}

void Radio86rk::start(uint8_t mode) {

}

void Radio86rk::stop() {

}

bool Radio86rk::getStatus() {
	return enabled;
}

void Radio86rk::recalculateToVector() {

}

Memory* Radio86rk::getMemory() {
	return mem;
}

vKeybParams Radio86rk::getVirtualKeyboardParams() {
	vKeybParams keyb;

	return keyb;
}

bool Radio86rk::udateNeeded() {
	return debugUpdateNeeded;
}

void Radio86rk::updated() {

}

void Radio86rk::keyDown(uint32_t keyCode) {

}

void Radio86rk::keyUp(uint32_t keyCode) {
}

void Radio86rk::loadProgramToMemory(std::string url, bool addr0) {
	FILE* f;
	f = fopen(url.c_str(), "rb");
	if (f) {
		uint8_t* buf = new uint8_t[65536];

		int res = fread(buf, 1, 65536, f);
		bool oldEnabled = enabled;
		enabled = false;
		mem->setMemory(buf, addr0 ? 0 : 0x100, res);
		mem->changeROMStatus(false);
		cpu->reset();
		enabled = oldEnabled;

		delete[] buf;
		fclose(f);
	}
}

void Radio86rk::loadDataToMemory(std::string url, int tp) {
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