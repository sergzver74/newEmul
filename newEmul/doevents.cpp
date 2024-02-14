#include "doevents.h"

typedef struct {
	GUI* guiElement;
	uint8_t evType;
	uint16_t x;
	uint16_t y;
	uint16_t x1;
	uint16_t y1;
	uint32_t param1;
	uint32_t param2;
	std::function<void()> callback;
	bool inUse;
} tEvents;

tEvents events[1024];

void initEvents() {
	for (int n = 0; n < 1024; n++) {
		events[n].guiElement = NULL;
		events[n].evType = 0;
		events[n].x = 0;
		events[n].y = 0;
		events[n].x1 = 0;
		events[n].y1 = 0;
		events[n].param1 = 0;
		events[n].param2 = 0;
		events[n].callback = NULL;
		events[n].inUse = false;
	}
}

uint16_t addEvent(GUI *guiElement, uint8_t evType, uint16_t xn, uint16_t yn, uint16_t dx, uint16_t dy, uint32_t param1, uint32_t param2, std::function<void()> callback) {
	for (int i = 0; i < 1024; i++) {
		if (events[i].guiElement == NULL) {
			events[i].guiElement = guiElement;
			events[i].evType = evType;
			events[i].x = xn;
			events[i].y = yn;
			events[i].x1 = xn + dx;
			events[i].y1 = yn + dy;
			events[i].param1 = param1;
			events[i].param2 = param2;
			events[i].callback = callback;
			events[i].inUse = false;
			return i;
		}
	}
	return 0;
}

void deleteEvent(uint16_t n) {
	events[n].guiElement = NULL;
	events[n].evType = 0;
	events[n].x = 0;
	events[n].y = 0;
	events[n].x1 =0;
	events[n].y1 = 0;
	events[n].param1 = 0;
	events[n].param2 = 0;
	events[n].callback = NULL;
	events[n].inUse = false;
}

bool cleanUse() {
	for (int i = 0; i < 1024; i++) {
		//if (events[i].guiElement != NULL) {
			if (events[i].inUse) {
				events[i].inUse = false;
				if (events[i].evType == MOUSEMOVE) events[i].guiElement->OnEndMove(events[i].param1, events[i].param2);
				return true;
			}
		//}
	}
	return false;
}

void cleanUse(uint16_t n) {
	events[n].inUse = false;
}

void setUse(uint16_t n) {
	events[n].inUse = true;
}

bool checkDoEvent(uint8_t evType, uint16_t x, uint16_t y) {
	int i = 0;
	for (i = 0; i < 1024; i++) {
		//if (events[i].guiElement != NULL) {
			if (events[i].evType == evType) {
				if (x >= events[i].x && x <= events[i].x1 && y >= events[i].y && y <= events[i].y1) {
					if (!events[i].inUse) {
						cleanUse();
						events[i].inUse = true;
						if (evType== MOUSEMOVE) events[i].guiElement->OnMove(events[i].param1, events[i].param2);
						if (evType == MOUSELEFTBUTTONDOWN) events[i].guiElement->OnClick(events[i].param1, events[i].param2);
						if (evType == MOUSELEFTBUTTONUP) events[i].guiElement->OnClickUp(events[i].param1, events[i].param2);
						return true;
					}
					else return false;
				}
			}
		//}
		//else break;
	}
	return cleanUse();
}