#include "vkeyb.h"

VKEYBOARD::VKEYBOARD(Machine* m, std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev, vKeybParams kp) : Window(name, x, y, w, h, ev) {
	
	machine = m;
	keybParams = kp;

	for (int i = 0; i < keybParams.keyCount; i++) {
		key[i] = new tButton(gContext, fContext);
		key[i]->create(keybParams.keys[i].x, keybParams.keys[i].y, keybParams.keys[i].width, keybParams.keys[i].heigth, keybParams.keys[i].cap);
		key[i]->Visibled(true);
		addEvent(key[i], 1, keybParams.keys[i].x, keybParams.keys[i].y, keybParams.keys[i].width, keybParams.keys[i].heigth, keybParams.keys[i].code, i);
		addEvent(key[i], 2, keybParams.keys[i].x, keybParams.keys[i].y, keybParams.keys[i].width, keybParams.keys[i].heigth, keybParams.keys[i].code, i);
	}
	for (int i = 0; i < keybParams.keyCount; i++) {
		keyState[i] = 0;
	}

	updateWindow();
}

VKEYBOARD::~VKEYBOARD() {
	machine = NULL;

	for (int i = 0; i < keybParams.keyCount; i++) {
		delete key[i];
	}
}

bool VKEYBOARD::eventManager(SDL_Event event) {

	//printf("Receive event to windonID=%d event type=%d\n", winID, event.type);

	uint32_t evType = 0;
	if (event.type == SDL_MOUSEBUTTONDOWN) evType = 1;
	if (event.type == SDL_MOUSEBUTTONUP) evType = 2;
	if (event.type == SDL_KEYDOWN) evType = 3;
	if (event.type == SDL_MOUSEMOTION) evType = 4;
	if (event.type == userEventType) evType = userEventType;
	if (event.type == SDL_MOUSEBUTTONUP && event.button.clicks == 2) evType = 5;

	int i = 0;
	for (i = 0; i < 512; i++) {
		if (winEvents[i].guiElement != NULL) {
			if (winEvents[i].evType == evType) {
				int32_t x, y;
				uint32_t bs = SDL_GetMouseState(&x, &y);
				if (x >= winEvents[i].x && x <= winEvents[i].x1 && y >= winEvents[i].y && y <= winEvents[i].y1) {
					if (evType == 1) {
						if (bs == 1) {
							winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
							keyState[winEvents[i].param2] = 1;
							printf("pressed key number: %d code: %02X\n", winEvents[i].param2, winEvents[i].param1);
							machine->keyDown(winEvents[i].param1);
						}
						if (bs == 4) {
							if (keyState[winEvents[i].param2] == 0) {
								winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
								keyState[winEvents[i].param2] = 4;
								printf("pressed key number: %d code: %02X\n", winEvents[i].param2, winEvents[i].param1);
								machine->keyDown(winEvents[i].param1);
							}
							else {
								if (keyState[winEvents[i].param2] == 4) {
									winEvents[i].guiElement->OnClickUp(winEvents[i].param1, winEvents[i].param2);
									keyState[winEvents[i].param2] = 0;
									printf("unpressed key number: %d code: %02X\n", winEvents[i].param2, winEvents[i].param1);
									machine->keyUp(winEvents[i].param1);
								}
							}
						}
					}
					if (evType == 2) {
						if (keyState[winEvents[i].param2] == 1) {
							winEvents[i].guiElement->OnClickUp(winEvents[i].param1, winEvents[i].param2);
							keyState[winEvents[i].param2] = 0;
							printf("unpressed key number: %d code: %02X\n", winEvents[i].param2, winEvents[i].param1);
							machine->keyUp(winEvents[i].param1);
						}
					}

					updateWindow();

					return true;
				}
			}
		}
		else break;
	}

	return false;
}