#ifndef __WAV_H__
#define __WAV_H__

#include <string>
#include <stdint.h>

class WAV {
public:
	
	
	WAV();
	~WAV();
	void loadDataFromWave(std::string name);


private:
	
	typedef struct {
		uint32_t chunkId;
		uint32_t chunkSize;
		uint32_t format;
		uint32_t subchunk1Id;
		uint32_t subchunk1Size;
		uint16_t audioFormat;
		uint16_t numChannels;
		uint32_t sampleRate;
		uint32_t byteRate;
		uint16_t blockAlign;
		uint16_t bitsPerSample;
		uint32_t subchunk2Id;
		uint32_t subchunk2Size;
	} WAVHeader;
	
	bool isLoaded;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate;
	uint16_t blockAlign;
	uint16_t bitsPerSample;

};

#endif // !__WAV_H__

