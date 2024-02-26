#ifndef __KBDV6C_H__
#define __KBDV6C_H__

#include <stdint.h>
#include "ports.h"
#include "wav.h"
#include "graph.h"

class Keyboard : public Ports {
public:
	Keyboard(WAV *wav);
	~Keyboard() {};

	void setGraphContext(Graph* gr);
	void setPortData(uint16_t portNum, uint16_t data);
	uint16_t getPortData(uint16_t portNum);
	tPorts getPorts();

	uint8_t keyDown(uint8_t key);
	uint8_t keyUp(uint8_t key);

private:
	
	const int VectorKeyboard[8][8] =	{{ 0x2C, 0x35, 0x30, 0x31, 0x2F, 0x1D, 0x1C, 0x1B },
										 { 0x1A, 0x19, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13 },
										 { 0x12, 0x11, 0x10, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B },
										 { 0x0A, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x34 },
										 { 0x38, 0x37, 0x2E, 0x36, 0x33, 0x2D, 0x26, 0x25 },
										 { 0x24, 0x23, 0x22, 0x21, 0x20, 0x1F, 0x1E, 0x27 },
										 { 0x3E, 0x3D, 0x3C, 0x3B, 0x3A, 0x29, 0x4C, 0x49 },
										 { 0x51, 0x4F, 0x52, 0x50, 0x2A, 0x28, 0x58, 0x2B }};
	
	
	//const int VectorKeyboard[8][8] = { {32, 226, 221, 220, 219, 90, 89, 88 },
	//								{87, 86,  85,  84,  83,  82, 81, 80 },
	//								{79, 78,  77,  76,  75,  74, 73, 72 },
	//								{71, 70,  69,  68,  67,  66, 65, 189},
	//								{191,190, 187, 188, 186, 222,57, 56 },
	//								{55, 54,  53,  52,  51,  50, 49, 48 },
	//								{116,115, 114, 113, 112, 27, 35, 36 },
	//								{40, 39,  38,  37,  8,   13, 45, 9  } };
	
	
	
	uint8_t kp02[256];
	uint8_t kp03[256];
	bool blk;
	bool vvod;
	bool sbr;

	bool ss;
	bool us;
	bool ruslat;

	uint8_t port01;
	uint8_t port02;
	uint8_t port03;
	uint8_t p02;
	uint8_t p03;
	WAV* wavPlayer;
	Graph* gContext;

};


#endif // !__KBDV6C_H__

