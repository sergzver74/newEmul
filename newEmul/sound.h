#ifndef __SOUND_H__
#define __SOUND_H__

#include <stdio.h>
#include <stdlib.h>
#include <Sdl.h>

#include "resampler.h"
#include "k580vi53.h"

#define BUFFERCOUNT	8
#define BUFFERSIZE	8192

class sound {
public:
	sound(uint32_t clock, K580VI53 *vi);
	~sound();
	void pause(bool pause);
	void sample(float sampl);
	void soundSteps(int nclk1m5, int tapeout, int tapein);
private:
	bool isInit;
	SDL_AudioDeviceID dev;
	uint16_t frameSize;
	static void callbackAudio(void* userData, uint8_t* stream, int len);
	float soundBuffer[BUFFERCOUNT][BUFFERSIZE];
	uint32_t sampleRate;
	uint32_t soundAccuTop;

	//std::atomic_int wrptr;
	uint32_t soundFrameSize;
	uint32_t currentSamplePtr;
	uint32_t dataBbuf;
	uint32_t playBuf;
	uint32_t curClocks = 0;
	float lastSample = 0;
	uint16_t clockPerSample = 0;
	K580VI53* vi53;

	Resampler resampler;
};


#endif // !__SOUND_H__

