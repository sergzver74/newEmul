#ifndef __K1818VG93_H__
#define __K1818VG93_H__

#include <stdint.h>
#include <string>

#include "ports.h"

class K1818VG93 : public Ports {
public:
	K1818VG93(std::string nm);
	~K1818VG93();
	void setPortData(uint16_t portNum, uint16_t data);
	uint16_t getPortData(uint16_t portNum);
	tPorts getPorts();
	void reset();
private:
	std::string fddName;
	bool FDDPresent;
	bool loadImage(std::string fn);
	void ready(bool en);
	void writeProtected(bool en);
	void writeError(bool en);
	void search(bool en);
	void crcEroor(bool en);
	void dataLost(bool en);
	void impulse(bool en);
	void busy(bool en);
	
	uint32_t imageLen;
	uint8_t* image;

	uint8_t regData;
	uint8_t regSect;
	uint8_t regTrack;
	uint8_t regStatus;
	uint8_t regCommand;
	uint8_t regSelect;

	uint8_t curSect;
	uint8_t curTrack;
	uint8_t fddSide;
	bool stepDirection;

	uint16_t sectorLength;
	uint8_t inSide;
	uint32_t curReadPointer;
	uint32_t curCount;
	bool readMode;
	bool writeMode;


};

#endif // !__K1818VG93_H__
