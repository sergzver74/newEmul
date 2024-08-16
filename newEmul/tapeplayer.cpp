#include "tapeplayer.h"

#include "resource.cpp"

TapePlayer::TapePlayer(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev) : Window(name, x, y, w, h, ev) {

	playbmp = new tBitmap;
	playbmp->loadBitmapFromMemory(playImage);
	unsigned int* bmpdata = playbmp->getBMPImage();
	gContext->putImage(100, 100, bmpdata);

	updateWindow();
	free(bmpdata);

}

TapePlayer::~TapePlayer() {
	delete playbmp;
}

bool TapePlayer::eventManager(SDL_Event event) {

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


					updateWindow();

					return true;
				}
			}
		}
		else break;
	}

	return false;
}