#include "window.h"

Window::Window(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t eventType) {
	width = w;
	height = h;

	window = SDL_CreateWindow(name.c_str(), x, y, width, height, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);

	bps = surface->format->BitsPerPixel;
	winID = SDL_GetWindowID(window);

	gContext = new Graph((void*)surface->pixels, width, height, bps);
	fContext = new Font(gContext);
	gContext->SetFillColor(cWINLIGHTGRAY);
	gContext->bar(0, 0, width - 1, height - 1);

	for (int n = 0; n < 512; n++) {
		winEvents[n].guiElement = NULL;
		winEvents[n].evType = 0;
		winEvents[n].x = 0;
		winEvents[n].y = 0;
		winEvents[n].x1 = 0;
		winEvents[n].y1 = 0;
		winEvents[n].param1 = 0;
		winEvents[n].param2 = 0;
		winEvents[n].inUse = false;
	}
	userEventType = eventType;

	/*
	tex = SDL_CreateTextureFromSurface(renderer, surface);

	if ((SDL_RenderCopy(renderer, tex, NULL, NULL)) < 0) {
		printf("%s\n", SDL_GetError());
		exit(-1);
	}
	SDL_DestroyTexture(tex);
	tex = NULL;

	SDL_RenderPresent(renderer);
	*/
}

Window::~Window() {
	delete fContext;
	delete gContext;
	if (tex) {
		SDL_DestroyTexture(tex);
		tex = NULL;
	}
	if (surface) {
		SDL_FreeSurface(surface);
		surface = NULL;
	}
	if (renderer) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window) {
		SDL_DestroyWindow(window);
		window = NULL;
	}
	width = 0;
	height = 0;
	bps = 0;
	winID = 0;
	for (int n = 0; n < 512; n++) {
		winEvents[n].guiElement = NULL;
		winEvents[n].evType = 0;
		winEvents[n].x = 0;
		winEvents[n].y = 0;
		winEvents[n].x1 = 0;
		winEvents[n].y1 = 0;
		winEvents[n].param1 = 0;
		winEvents[n].param2 = 0;
		winEvents[n].inUse = false;
	}
}

uint32_t Window::getWindowID() {
	return winID;
}

void Window::updateWindow() {
	tex = SDL_CreateTextureFromSurface(renderer, surface);
	if (tex) {
		if ((SDL_RenderCopy(renderer, tex, NULL, NULL)) < 0) {
			printf("Error render copy, %p\n", tex);
			printf("%s\n", SDL_GetError());
			exit(-1);
		}
		SDL_DestroyTexture(tex);
		tex = NULL;

		SDL_RenderPresent(renderer);
	}
}

uint16_t Window::addEvent(GUI* guiElement, uint32_t evType, uint16_t xn, uint16_t yn, uint16_t dx, uint16_t dy, uint32_t param1, uint32_t param2) {
	for (int i = 0; i < 512; i++) {
		if (winEvents[i].guiElement == NULL) {
			winEvents[i].guiElement = guiElement;
			winEvents[i].evType = evType;
			winEvents[i].x = xn;
			winEvents[i].y = yn;
			winEvents[i].x1 = xn + dx;
			winEvents[i].y1 = yn + dy;
			winEvents[i].param1 = param1;
			winEvents[i].param2 = param2;
			winEvents[i].inUse = false;
			return i;
		}
	}
	return 0;
}

void Window::deleteEvent(uint16_t n) {
	winEvents[n].guiElement = NULL;
	winEvents[n].evType = 0;
	winEvents[n].x = 0;
	winEvents[n].y = 0;
	winEvents[n].x1 = 0;
	winEvents[n].y1 = 0;
	winEvents[n].param1 = 0;
	winEvents[n].param2 = 0;
	winEvents[n].inUse = false;
}