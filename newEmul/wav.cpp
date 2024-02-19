#include "wav.h"

WAV::WAV() {
	isLoaded = false;
	sampleRate = 0;
	audioFormat = 0;
	numChannels = 0;
	byteRate = 0;
	blockAlign = 0;
	bitsPerSample = 0;
}

WAV::~WAV() {

}

void WAV::loadDataFromWave(std::string name) {

	FILE* fl;
	fopen_s(&fl, (char*)name.c_str(), "rb");
	if (fl)
	{
		unsigned char* buf;
		buf = new unsigned char[44];
		int num = fread(buf, 1, 44, fl);
		fclose(fl);

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


				}
				else printf("This is not WAV file format\n");
			}
			else printf("This is not WAV file format\n");
		}
		else printf("This is not WAV file format\n");




		delete[] buf;
	}
}