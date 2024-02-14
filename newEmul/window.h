#ifndef __WINDOW_H__
#define __WINDOW_H__

#include <stdint.h>
#include <string>
#include <SDL.h>
#include "window.h"
#include "gui.h"
#include "graph.h"
#include "fonts.h"

class Window {
private:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
	SDL_Texture* tex;
	uint16_t width;
	uint16_t height;
	uint16_t bps;

protected:
	
	typedef struct {
		GUI* guiElement;
		uint32_t evType;
		uint16_t x;
		uint16_t y;
		uint16_t x1;
		uint16_t y1;
		uint32_t param1;
		uint32_t param2;
		bool inUse;
	} tWinEvents;

	
	Graph* gContext;
	Font* fContext;
	uint32_t winID;
	uint32_t userEventType;
	tWinEvents winEvents[512];

	uint16_t addEvent(GUI* guiElement, uint32_t evType, uint16_t xn, uint16_t yn, uint16_t dx, uint16_t dy, uint32_t param1, uint32_t param2);
	void deleteEvent(uint16_t n);


public:

	Window(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t eventType);
	virtual ~Window();
	virtual bool eventManager(SDL_Event event) = 0;


	void updateWindow();
	uint32_t getWindowID();
};


#endif // !__WINDOW_H__
