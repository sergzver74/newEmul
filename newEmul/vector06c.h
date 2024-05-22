#ifndef __VECTOR_06C_H__
#define __VECTOR_06C_H__

#include <SDL.h>

#include "cpu.h"
#include "i8080.h"
#include "timer.h"
#include "memory.h"
#include "machine.h"
#include "vecrordisplay.h"
#include "kbdv6c.h"
#include "wav.h"
#include "sound.h"
#include "k580vi53.h"
#include "k1818vg93.h"
#include "kvaz.h"
#include "vkeyboard.h"
#include "config.h"

class Vector06c : public Machine
{
public:
	Vector06c();
	Vector06c(SDL_Renderer* rendr, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, WAV* wav, PROFILE prof);
	~Vector06c();

	void timer();
	CPU* getCPU();

	void start(uint8_t mode);
	void stop();
	bool getStatus();
	void recalculateToVector();
	
	//void trace();
	//void step();
	void stepTo(std::string addr);
	Memory* getMemory();
	void keyDown(uint32_t keyCode);
	void keyUp(uint32_t keyCode);
	void loadProgramToMemory(std::string url, bool addr0);
	void loadDataToMemory(std::string url, int tp);
	vKeybParams getVirtualKeyboardParams();
	bool udateNeeded();
	void updated();

private:

	Timer *clock;
	Memory *mem;
	i8080 *cpu;
	VectorDisplay *display;
	Keyboard* keyboard;
	sound* snd;
	WAV* wavPlayer;
	K580VI53* timervi53;
	kvaz* kvazidisk;
	K1818VG93 *fddController;

	SDL_Renderer* renderer;
	uint64_t clockCount;
	uint64_t tickCount;
	uint8_t commandTicksCount;
	bool enabled;
	bool cycle;
	bool INTE;
	uint8_t debugMode;
	uint16_t stopAddr;
	bool stopNeeded;
	bool debugUpdateNeeded;


};

#endif // !__VECTOR_06C_H__

