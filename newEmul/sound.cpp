#include <string.h>
#include "sound.h"

sound::sound(uint32_t clock) {

    currentSamplePtr = 0;
    playBuf = 0;
    dataBbuf = 0;
    curClocks = 0;
    lastSample = 0;
    
    SDL_AudioSpec sdlSpec;
    SDL_AudioSpec realSdlSpec;
    SDL_memset(&sdlSpec, 0, sizeof(sdlSpec));
    sdlSpec.freq = 48000;
    sdlSpec.format = AUDIO_F32; // AUDIO_U16;
    sdlSpec.channels = 2;

    frameSize = sdlSpec.freq / 50;

    sdlSpec.samples = frameSize;
    sdlSpec.callback = sound::callbackAudio;
    sdlSpec.userdata = (void*)this;

    dev = SDL_OpenAudioDevice(NULL, 0, &sdlSpec, &realSdlSpec, SDL_AUDIO_ALLOW_FORMAT_CHANGE);
    if (dev == 0) {
        printf("Sound not initialized\n");
        isInit = false;
        return;
    }

    soundFrameSize = realSdlSpec.samples;
    sampleRate = realSdlSpec.freq;
    int cyclesInSeconds = 50 * 768 * 312 / 8;
    soundAccuTop = (int)(0.5 + 100.0 * cyclesInSeconds / this->sampleRate);

    clockPerSample = clock / sampleRate;

    resampler.set_passthrough(true);

    isInit = true;
}

sound::~sound() {
    isInit = false;
    SDL_CloseAudioDevice(dev);
}

void sound::callbackAudio(void* userData, uint8_t* stream, int len) {
    sound* dataSoundClass = (sound*)userData;
    float* myStream = (float*)stream;

    //printf("playBuf=%d, dataBbuf=%d, currentSamplePtr=%d, len=%d\n", dataSoundClass->playBuf, dataSoundClass->dataBbuf, dataSoundClass->currentSamplePtr, len);
    if (dataSoundClass->playBuf == dataSoundClass->dataBbuf) {
        memcpy(stream, dataSoundClass->soundBuffer[dataSoundClass->playBuf], dataSoundClass->currentSamplePtr * sizeof(float));
        for (int i = dataSoundClass->currentSamplePtr; i < dataSoundClass->soundFrameSize * 2; i++) {
            myStream[i] = dataSoundClass->lastSample;
        }
        dataSoundClass->currentSamplePtr = 0;
    }
    else
    {
        memcpy(stream, dataSoundClass->soundBuffer[dataSoundClass->playBuf], len);
        dataSoundClass->playBuf++;
        if (dataSoundClass->playBuf == BUFFERCOUNT) {
            dataSoundClass->playBuf = 0;
        }
    }

}

void sound::pause(bool pause) {
    if (isInit) {
        SDL_PauseAudioDevice(dev, (int)pause);
    }
    currentSamplePtr = 0;
    playBuf = 0;
    dataBbuf = 0;
}

void sound::sample(float sampl)
{
    if (isInit) {
        SDL_LockAudioDevice(dev);

        lastSample = sampl;
        soundBuffer[dataBbuf][currentSamplePtr++] = sampl;
        soundBuffer[dataBbuf][currentSamplePtr++] = sampl;
        if (currentSamplePtr >= soundFrameSize * 2) {
            currentSamplePtr = 0;
            dataBbuf++;
            //printf("Current databuf: %d\n", dataBbuf);
            if (dataBbuf == BUFFERCOUNT) {
                dataBbuf = 0;
            }
        }
        
        SDL_UnlockAudioDevice(dev);
    }
}

namespace std {
    float clamp(float v, float lo, float hi) {
        return v < lo ? lo : v > hi ? hi : v;
    }
}

void sound::soundSteps(int commandTicksCount, int tapeout, int tapein, int vi53)
{
    //if (tapeout != 0) printf("output %d\n", tapeout);
    /*
    int ech0 = Options.enable.timer_ch0,
        ech1 = Options.enable.timer_ch1,
        ech2 = Options.enable.timer_ch2,
        aych0 = Options.enable.ay_ch0,
        aych1 = Options.enable.ay_ch1,
        aych2 = Options.enable.ay_ch2;
    */
    
    //for (int clk = 0; clk < commandTicksCount; ++clk) {
        //float ay = this->aywrapper.step2(2, aych0, aych1, aych2);

        // timerwrapper does the stepping of 8253, it must always be called */
        /*
        float soundf = (this->timerwrapper.singlestep(ech0, ech1, ech2)) * Options.volume.timer
            + (tapeout + tapein) * Options.volume.beeper
            + Options.volume.covox * (covox / 256.0f)
            + Options.volume.ay * ay;
        */
        //if (tapein == 0) tapein = 1; else tapein = 0;
        float smpl = (tapeout + tapein) * 1.f + vi53 * 1.f;

        //smpl = this->resampler.sample(smpl);
        //if (resampler.egg) {
        //    resampler.egg = false;
        //    sample(std::clamp(smpl * 1.f, -1.f, 1.f));
        //}

        curClocks += commandTicksCount;
        if (curClocks >= clockPerSample) {
            sample(std::clamp(smpl * 1.f, -1.f, 1.f));
            curClocks -= clockPerSample;
        }


        //if (isInit) {
            //soundf = this->resampler.sample(soundf);

            //if (resampler.egg) {
            //    resampler.egg = false;
            //sample(smpl);
            //}
        //}
    //}
}
