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
#include "kvaz.h"
#include "vkeyboard.h"

class Vector06c : public Machine
{
public:
	Vector06c();
	Vector06c(SDL_Renderer* rendr, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, WAV* wav, bool kvazPresent);
	~Vector06c();

	void timer();
	CPU* getCPU();

	void start();
	void stop();
	bool getStatus();
	void recalculateToVector();
	
	void trace();
	void step();
	void stepTo(std::string addr);
	Memory* getMemory();
	void keyDown(uint32_t keyCode);
	void keyUp(uint32_t keyCode);
	void loadProgramToMemory(std::string url, bool addr0);
	void loadDataToMemory(std::string url, int tp);
	vKeybParams getVirtualKeyboardParams();

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

	SDL_Renderer* renderer;
	uint64_t clockCount;
	uint64_t tickCount;
	uint8_t commandTicksCount;
	bool enabled;
	bool cycle;
	bool INTE;


};

#endif // !__VECTOR_06C_H__

