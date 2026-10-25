#ifndef __RADIO86RK
#define __RADIO86RK

#include <SDL.h>

#include "cpu.h"
#include "i8080.h"
#include "timer.h"
#include "memory.h"
#include "machine.h"
#include "wav.h"
#include "config.h"
#include "breakpoints.h"

class Radio86rk : public Machine
{
public:
	Radio86rk();
	Radio86rk(SDL_Renderer* rendr, std::function<void(SDL_Renderer* renderer, SDL_Surface* surface)> callback, WAV* wav, PROFILE prof, Breakpoints* bp);
	~Radio86rk();

	void timer();
	CPU* getCPU();

	void start(uint8_t mode);
	void stop();
	bool getStatus();
	void recalculateToVector();

	Memory* getMemory();
	void keyDown(uint32_t keyCode);
	void keyUp(uint32_t keyCode);
	void loadProgramToMemory(std::string url, bool addr0);
	void loadDataToMemory(std::string url, int tp);
	vKeybParams getVirtualKeyboardParams();
	bool udateNeeded();
	void updated();

private:
	Timer* clock;
	Memory* mem;
	i8080* cpu;
	//Keyboard* keyboard;
	//sound* snd;
	WAV* wavPlayer;
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
	Breakpoints* breakPoints;
};

#endif // !__RADIO86RK
