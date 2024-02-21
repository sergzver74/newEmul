#include "wav.h"

static uint8_t* audio_pos;
static uint32_t audio_len;

static Uint32 wav_length;
static Uint8* wav_buffer;
static SDL_AudioSpec wav_spec;


void my_audio_callback(void* userdata, Uint8* stream, int len) {

	if (audio_len == 0) {
		SDL_CloseAudio();
		SDL_FreeWAV(wav_buffer);
		return;
	}

	len = (len > audio_len ? audio_len : len);
	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);

	audio_pos += len;
	audio_len -= len;
}

WAV::WAV(uint32_t clk) {
	clock = clk;
	isLoaded = false;
	sampleRate = 0;
	audioFormat = 0;
	numChannels = 0;
	byteRate = 0;
	blockAlign = 0;
	bitsPerSample = 0;
	data8 = NULL;
	data16 = NULL;
	dataLen = 0;
	curPlayedSample = 0;
	curClocks = 0;
}

WAV::~WAV() {
	clock = 0;
	isLoaded = false;
	sampleRate = 0;
	audioFormat = 0;
	numChannels = 0;
	byteRate = 0;
	blockAlign = 0;
	bitsPerSample = 0;
	if (data8) {
		delete[] data8;
		data8 = NULL;
	}
	if (data16) {
		delete[] data16;
		data16 = NULL;
	}
	dataLen = 0;
	curPlayedSample = 0;
	curClocks = 0;
}

void WAV::loadDataFromWave(std::string name) {

	FILE* fl;
	fopen_s(&fl, (char*)name.c_str(), "rb");
	if (fl)
	{
		unsigned char* buf;
		buf = new unsigned char[44];
		int num = fread(buf, 1, 44, fl);
		//fclose(fl);
		if (num == 44) {

			WAVHeader* header;
			header = (WAVHeader*)buf;
			if (header->chunkId == 0x46464952)
			{
				if (header->format == 0x45564157)
				{
					if (header->subchunk1Id == 0x20746d66)
					{
						sampleRate = header->sampleRate;
						audioFormat = header->audioFormat;
						numChannels = header->numChannels;
						byteRate = header->byteRate;
						blockAlign = header->blockAlign;
						bitsPerSample = header->bitsPerSample;

						printf("Open file:%s\n", name.c_str());
						printf("numChannels=%d\n", numChannels);
						printf("audioFormat=%d\n", audioFormat);
						printf("sampleRate=%d\n", sampleRate);
						printf("byteRate=%d\n", byteRate);
						printf("blockAlign=%d\n", blockAlign);
						printf("bitsPerSample=%d\n", bitsPerSample);
						clockPerSample = clock / sampleRate;

						if (SDL_LoadWAV(name.c_str(), &wav_spec, &wav_buffer, &wav_length) == NULL) {
							printf("SDL: Error loading wav\n");
						}

						wav_spec.callback = my_audio_callback;
						wav_spec.userdata = NULL;

						audio_pos = wav_buffer;
						audio_len = wav_length;

						if (SDL_OpenAudio(&wav_spec, NULL) < 0) {
							fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
							exit(-1);
						}

						if (header->subchunk2Id == 0x61746164) {
							isLoaded = false;
							if (data8) {
								delete[] data8;
								data8 = NULL;
							}
							if (data16) {
								delete[] data16;
								data16 = NULL;
							}
							dataLen = 0;
							if (bitsPerSample == 8) {
								uint8_t *tdata8 = new uint8_t[header->subchunk2Size];
								num = fread(tdata8, 1, header->subchunk2Size, fl);
								if (num == header->subchunk2Size) {
									if (numChannels == 1) {
										data8 = new uint8_t[num];
										for (int i = 0; i < num; i++) data8[i] = tdata8[i];
										dataLen = num;
										delete[] tdata8;
									}
									if (numChannels == 2) {
										data8 = new uint8_t[num / 2];
										uint32_t cnt = 0;
										for (int i = 0; i < num;) {
											data8[cnt++] = ((uint16_t)tdata8[i] + (uint16_t)tdata8[i + 1]) / 2;
											i += 2;
										}
										dataLen = cnt;
										delete[] tdata8;
									}
									isLoaded = true;
								} else printf("Error reading data\n");
							}
							if (bitsPerSample == 16) {
								uint16_t* tdata16 = new uint16_t[header->subchunk2Size / 2];
								int num = fread(tdata16, 1, header->subchunk2Size / 2, fl);
								if (num == header->subchunk2Size / 2) {
									if (numChannels == 1) {
										data16 = new uint16_t[num];
										for (int i = 0; i < num; i++) data16[i] = tdata16[i];
										dataLen = num;
										delete[] tdata16;
									}
									if (numChannels == 2) {
										data16 = new uint16_t[num / 2];
										uint32_t cnt = 0;
										for (int i = 0; i < num;) {
											data16[cnt++] = ((uint32_t)tdata16[i] + (uint32_t)tdata16[i + 1]) / 2;
											i += 2;
										}
										dataLen = cnt;
										delete[] tdata16;
									}
									isLoaded = true;
								}
								else printf("Error reading data\n");
							}
							SDL_PauseAudio(0);
						
						}
						else printf("This is not WAV file format\n");
					}
					else printf("This is not WAV file format\n");
				}
				else printf("This is not WAV file format\n");
			}
			else printf("This is not WAV file format\n");

		} else printf("Error reading header\n");


		delete[] buf;
		fclose(fl);
	}
}

void WAV::playSample(uint32_t commandTicksCount) {
	if (isLoaded) {
		if (curPlayedSample < dataLen) {
			curClocks += commandTicksCount;
			if (curClocks >= clockPerSample) {
				curClocks -= clockPerSample;
				curPlayedSample++;
			}
		}
		else {
			printf("Play END\n");
			isLoaded = false;
			curPlayedSample = 0;
			curClocks = 0;
			dataLen = 0;
			if (data8) {
				delete[] data8;
				data8 = NULL;
			}
			if (data16) {
				delete[] data16;
				data16 = NULL;
			}
		}
	}
}

bool WAV::getCurrentSample() {
	if (isLoaded) {
		if (bitsPerSample == 8) {
			if (data8[curPlayedSample] < 0x80) return true; else return false;
		}
		if (bitsPerSample == 16) {
			if (data16[curPlayedSample] < 0x8000) return true; else return false;
		}
	}
	else
	{
		return false;
	}
}