#include "tapeplayer.h"

#include "resource.cpp"

TapePlayer::TapePlayer(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev) : Window(name, x, y, w, h, ev) {

	openbmp = new tBitmap;
	openbmp->loadBitmapFromMemory(openImage);
	openImageData = openbmp->getBMPImage();
	open = new tButton(gContext, fContext);
	open->create(10, 10, 67, 31, "");
	open->setImage(5, 5, openImageData, 0xFFFFFFFF);
	open->Visibled(true);
	addEvent(open, 1, 10, 10, 67, 31, 0, 0);
	addEvent(open, 2, 10, 10, 67, 31, 0, 0);
	
	recbmp = new tBitmap;
	recbmp->loadBitmapFromMemory(recImage);
	recImageData = recbmp->getBMPImage();
	rec = new tButton(gContext, fContext);
	rec->create(190, 20, 60, 64, "");
	rec->setImage(5, 5, recImageData, 0xFFFFFFFF);
	rec->Visibled(true);
	addEvent(rec, 1, 190, 20, 60, 64, 0, 0);
	addEvent(rec, 2, 190, 20, 60, 64, 0, 0);
	
	rewbmp = new tBitmap;
	rewbmp->loadBitmapFromMemory(rewImage);
	rewImageData = rewbmp->getBMPImage();
	rew = new tButton(gContext, fContext);
	rew->create(250, 20, 60, 64, "");
	rew->setImage(5, 5, rewImageData, 0xFFFFFFFF);
	rew->Visibled(true);
	addEvent(rew, 1, 250, 20, 60, 64, 0, 0);
	addEvent(rew, 2, 250, 20, 60, 64, 0, 0);

	playbmp = new tBitmap;
	playbmp->loadBitmapFromMemory(playImage);
	playImageData = playbmp->getBMPImage();
	play = new tButton(gContext, fContext);
	play->create(310, 20, 60, 64, "");
	play->setImage(5, 5, playImageData, 0xFFFFFFFF);
	play->Visibled(true);
	addEvent(play, 1, 310, 20, 60, 64, 0, 0);
	addEvent(play, 2, 310, 20, 60, 64, 0, 0);

	nextbmp = new tBitmap;
	nextbmp->loadBitmapFromMemory(nextImage);
	nextImageData = nextbmp->getBMPImage();
	next = new tButton(gContext, fContext);
	next->create(370, 20, 60, 64, "");
	next->setImage(5, 5, nextImageData, 0xFFFFFFFF);
	next->Visibled(true);
	addEvent(next, 1, 370, 20, 60, 64, 0, 0);
	addEvent(next, 2, 370, 20, 60, 64, 0, 0);

	
	stopbmp = new tBitmap;
	stopbmp->loadBitmapFromMemory(stopImage);
	stopImageData = stopbmp->getBMPImage();
	stop = new tButton(gContext, fContext);
	stop->create(310, 20, 60, 64, "");
	stop->setImage(5, 5, stopImageData, 0xFFFFFFFF);
	addEvent(stop, 1, 310, 20, 60, 64, 0, 0);
	addEvent(stop, 2, 310, 20, 60, 64, 0, 0);
	



	updateWindow();

}

TapePlayer::~TapePlayer() {

	stop->Destroy();
	delete stop;
	free(stopImageData);
	delete stopbmp;
	
	next->Destroy();
	delete next;
	free(nextImageData);
	delete nextbmp;
	
	play->Destroy();
	delete play;
	free(playImageData);
	delete playbmp;
	
	rew->Destroy();
	delete rew;
	free(rewImageData);
	delete rewbmp;
	
	rec->Destroy();
	delete rec;
	free(recImageData);
	delete recbmp;

	open->Destroy();
	delete open;
	free(openImageData);
	delete openbmp;
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
					if (evType == 1) {
						winEvents[i].param1 = x;
						winEvents[i].param2 = y;
						winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
					}
					if (evType == 2) {
						winEvents[i].param1 = x;
						winEvents[i].param2 = y;
						winEvents[i].guiElement->OnClickUp(winEvents[i].param1, winEvents[i].param2);
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