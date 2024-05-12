#ifndef __VKEYB_H__
#define __VKEYB_H__

#include <stdint.h>
#include <string>
#include <SDL.h>

#include "vkeyboard.h"
#include "window.h"
#include "graph.h"
#include "fonts.h"
#include "gui.h"
#include "machine.h"
#include "vkeyboard.h"

class VKEYBOARD : public  Window {
private:
	Machine* machine;
	vKeybParams keybParams;

	tButton* key[128];
	uint8_t keyState[128];

public:
	VKEYBOARD(Machine* m, std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev, vKeybParams kp);
	virtual ~VKEYBOARD();
	bool eventManager(SDL_Event event);
};

#endif // !__VKEYB_H__
