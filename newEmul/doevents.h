#ifndef __DOEVENTS_H__
#define __DOEVENTS_H__

#include <stdint.h>
#include <functional>
#include "gui.h"

#define MOUSELEFTBUTTONDOWN		1
#define MOUSELEFTBUTTONUP		2
#define MOUSEMOVE				3
#define MOUSERIGHTBUTTONDOWN	4
#define MOUSERIGHTBUTTONUP		5

void initEvents();
uint16_t  addEvent(GUI* guiElement, uint8_t evType, uint16_t xn, uint16_t yn, uint16_t dx, uint16_t dy, uint32_t param1, uint32_t param2, std::function<void()> callback);
void deleteEvent(uint16_t n);
bool checkDoEvent(uint8_t evType, uint16_t x, uint16_t y);
void cleanUse(uint16_t n);
void setUse(uint16_t n);

#endif // !__DOEVENTS_H__
