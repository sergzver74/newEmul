#ifndef __KBDV6C_H__
#define __KBDV6C_H__

#include <stdint.h>
#include "ports.h"

class Keyboard : public Ports {
public:
	Keyboard();
	~Keyboard() {};

	void setPortData(uint16_t portNum, uint16_t data);
	uint16_t getPortData(uint16_t portNum);
	tPorts getPorts();

	uint8_t keyDown(uint8_t key);
	uint8_t keyUp(uint8_t key);

private:
	
	const int VectorKeyboard[8][8] = { {32, 226, 221, 220, 219, 90, 89, 88 },
									{87, 86,  85,  84,  83,  82, 81, 80 },
									{79, 78,  77,  76,  75,  74, 73, 72 },
									{71, 70,  69,  68,  67,  66, 65, 189},
									{191,190, 187, 188, 186, 222,57, 56 },
									{55, 54,  53,  52,  51,  50, 49, 48 },
									{116,115, 114, 113, 112, 27, 35, 36 },
									{40, 39,  38,  37,  8,   13, 45, 9  } };
	
	uint8_t kp02[256];
	uint8_t kp03[256];
	bool blk;
	bool vvod;
	bool sbr;

	uint8_t port01;
	uint8_t port02;
	uint8_t port03;
	uint8_t p02;
	uint8_t p03;

};


#endif // !__KBDV6C_H__

