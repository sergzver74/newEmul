#include "k1818vg93.h"

using namespace std;

K1818VG93::K1818VG93(string fn) {
	fddName = fn;
	imageLen = 0;
	image = NULL;
	regData = 0;
	regSect = 0;
	regTrack = 0;
	regStatus = 0;
	regCommand = 0;
	curTrack = 0;
	curSect = 0;
	regSelect = 0;
	stepDirection = false;
	readMode = false;
	writeMode = false;
	if (fn != "") {
		if (loadImage(fn)) FDDPresent = true; else FDDPresent = false;
	}
	else FDDPresent = false;
	readAddressMode = false;
	for (int i = 0; i < 6; i++) readAddressBuf[i] = 0;
	fddNum = 0;
}

K1818VG93::~K1818VG93() {
	if (image) {
		delete[] image;
		image = NULL;
	}
	imageLen = 0;
	FDDPresent = false;
	fddName = "";
	regData = 0;
	regSect = 0;
	regTrack = 0;
	regStatus = 0;
	regCommand = 0;
	curSect = 0;
	curTrack = 0;
	regSelect = 0;
	stepDirection = false;
	readMode = false;
	writeMode = false;
	readAddressMode = false;
	for (int i = 0; i < 6; i++) readAddressBuf[i] = 0;
	fddNum = 0;
}

bool K1818VG93::loadImage(std::string fn) {
	
	bool res = false;
	
	if (image) {
		delete[] image;
		image = NULL;
	}
	imageLen = 0;

	FILE* f = fopen(fn.c_str(), "rb");
	if (f) {
		fseek(f, 0, SEEK_END);
		uint32_t fileLen = (uint32_t)ftell(f);
		fseek(f, 0, SEEK_SET);
		image = new uint8_t[fileLen];
		imageLen = fread(image, 1, fileLen, f);
		if (imageLen == fileLen) res = true;
	
		if (image[0x1F] == 0xFE)
		{
			if (image[0x0A] == 0) sectorLength = 128;
			if (image[0x0A] == 1) sectorLength = 256;
			if (image[0x0A] == 2) sectorLength = 512;
			if (image[0x0A] == 3) sectorLength = 1024;
			inSide = image[0x0B];
			curReadPointer = 0;
			curCount = 0;
			curTrack = 0;
		}
		else
		{
			sectorLength = 1024;
			inSide = 1;
			curReadPointer = 0;
			curCount = 0;
			curTrack = 0;
		}
	
	
	}

	return res;
}

void K1818VG93::ready(bool en) {
	if (!en) regStatus &= 0x7F; else  regStatus |= 0x80;
}

void K1818VG93::writeProtected(bool en) {
	if (!en) regStatus &= 0xBF; else regStatus |= 0x40;
}

void K1818VG93::writeError(bool en) {
	if (!en) regStatus &= 0xDF; else regStatus |= 0x20;
}

void K1818VG93::search(bool en) {
	if (!en) regStatus &= 0xEF; else regStatus |= 0x10;
}

void K1818VG93::crcEroor(bool en) {
	if (!en) regStatus &= 0xF7; else regStatus |= 0x08;
}

void K1818VG93::dataLost(bool en) {
	if (!en) regStatus &= 0xFB; else regStatus |= 0x04;
}

void K1818VG93::impulse(bool en) {
	if (!en) regStatus &= 0xFD; else regStatus |= 0x02;
}

void K1818VG93::busy(bool en) {
	if (!en) regStatus &= 0xFE; else regStatus |= 0x01;
}

void K1818VG93::setPortData(uint16_t portNum, uint16_t data) {
	switch (portNum) {
	case 0x18:
		regData = data & 0xFF;
		if (writeMode) {
			curCount--;
			if (curCount == 0)
			{
				regStatus = 0;
				readMode = false;
			}
			image[curWritePointer++] = regData;
		}
		break;
	case 0x19:
		regSect = data & 0xFF;
		break;
	case 0x1A:
		regTrack = data & 0xFF;
		break;
	case 0x1B:
		{
			regCommand = data & 0xFF;
			if (FDDPresent) {
				if ((data & 0xFF) >> 4 == 0) {
					regData = 0;
					regSect = 0;
					regTrack = 0;
					regStatus = 0;
					regCommand = 0;
					curSect = 0;
					curTrack = 0;
					regSelect = 0;
				}
				if ((data & 0xFF) >> 4 == 1) {
					if (regTrack < 80) {
						search(false);
						curTrack = regData;
					}
					else {
						search(true);
						curTrack = 79;
					}
					regTrack = curTrack;
				}
				if ((data & 0xFF) >> 5 == 1) {
					if (!stepDirection) {
						curTrack++;
						if (curTrack < 80) {
							search(false);
						} else search(true);
					}
					else {
						if (curTrack > 0) {
							curTrack--;
							search(false);
						}
						else search(true);
					}
					if ((((data & 0xFF) >> 4) & 0x01) == 1) regTrack = curTrack;
				}
				if ((data & 0xFF) >> 5 == 2) {
					stepDirection = false;
					curTrack++;
					if (curTrack < 80) {
						search(false);
					}
					else search(true);
					if ((((data & 0xFF) >> 4) & 0x01) == 1) regTrack = curTrack;
				}
				if ((data & 0xFF) >> 5 == 3) {
					stepDirection = true;
					if (curTrack > 0) {
						curTrack--;
						search(false);
					}
					else search(true);
					if ((((data & 0xFF) >> 4) & 0x01) == 1) regTrack = curTrack;
				}
				if ((data & 0xFF) >> 5 == 4) {
					readMode = true;
					impulse(true);
					busy(true);
					curCount = sectorLength;
					curReadPointer = (regSect - 1) * sectorLength + inSide * (fddSide * 5120) + curTrack * 10240;
				}
				if ((data & 0xFF) >> 5 == 5) {
					writeMode = true;
					impulse(true);
					busy(true);
					curCount = sectorLength;
					curWritePointer = (regSect - 1) * sectorLength + inSide * (fddSide * 5120) + curTrack * 10240;
				}
				if ((data & 0xFF) >> 5 == 6) {

					readAddressMode = true;
					impulse(true);
					busy(true);
					curCount = 6;
					curReadPointer = 0;
					readAddressBuf[0] = fddSide;
					readAddressBuf[1] = curTrack;
					readAddressBuf[2] = regSect;
					readAddressBuf[3] = sectorLength;
					readAddressBuf[4] = 0;
					readAddressBuf[5] = 0;
				}
				if ((data & 0xFF) >> 4 == 14) {

					printf("This is READ TRACK COMMMAND\n");
					exit(1);

				}
				if ((data & 0xFF) >> 4 == 15) {

					printf("This is WRIET TRACK COMMAND\n");
					exit(1);

				}

				ready(false);
			} else ready(true);
		}
		break;
	case 0x1C:
		regSelect = data;
		if (FDDPresent)
		{
			fddNum = data & 0x03;
			fddSide = !((data >> 2) & 0x01);
			regStatus = 0;
		}
		else ready(true);
		break;
	}
}

uint16_t K1818VG93::getPortData(uint16_t portNum) {
	switch (portNum) {
	case 0x18:
		if (readMode) {
			curCount--;
			if (curCount == 0)
			{
				regStatus = 0;
				readMode = false;
			}
			return image[curReadPointer++];
		}
		if (readAddressMode) {
			curCount--;
			if (curCount == 0)
			{
				regStatus = 0;
				readAddressMode = false;
			}
			return readAddressBuf[curReadPointer++];
		}
		break;
	case 0x19:
		return regSect;
		break;
	case 0x1A:
		return regTrack;
		break;
	case 0x1B:
		return regStatus;
		break;
	case 0x1C:
		return regSelect;
		break;
	}

	return 0;
}

tPorts K1818VG93::getPorts() {
	ports.count = 5;
	ports.ports[0] = 0x18;
	ports.ports[1] = 0x19;
	ports.ports[2] = 0x1A;
	ports.ports[3] = 0x1B;
	ports.ports[4] = 0x1C;
	return ports;
}

void K1818VG93::reset() {

}