#ifndef __VKEBOARD_H__
#define __VKEBOARD_H__

#include <stdint.h>
#include <string>

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t heigth;
	std::string cap;
	uint8_t code;
} KEY;

typedef struct {
	uint16_t winWidth;
	uint16_t winHeigth;
	uint8_t keyCount;
	KEY keys[128];
} vKeybParams;

#endif // !__VKEBOARD_H__

