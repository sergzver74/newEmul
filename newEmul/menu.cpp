#include "menu.h"

menu::menu() {
	mx = 0;
	my = 0;
	mx1 = 0;
	my1 = 0;
	mainItemsCount = 0;
	for (int i = 0; i < 16; i++) mainButtonDownEvent[i] = 0;
	for (int i = 0; i < 16; i++) mainMouseMoveEvent[i] = 0;
	for (int i = 0; i < 16; i++) subItemsCount[i] = 0;
	for (int i = 0; i < 16; i++) mainItems[i] = NULL;
	for (int i = 0; i < 16; i++) mainMenuItems[i] = "";
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) subMenuItems[i][j] = "";
	}
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++)  callbacks[i][j] = NULL;
	}
	visible = false;
	backImage = NULL;
	backSelectImage = NULL;
	backSubImage = NULL;
	mainMenuOpen = false;
	curOpenMainMenu = 0;
}

menu::menu(Graph* gc, Font* fc) {
	mx = 0;
	my = 0;
	mx1 = 0;
	my1 = 0;
	mainItemsCount = 0;
	for (int i = 0; i < 16; i++) mainButtonDownEvent[i] = 0;
	for (int i = 0; i < 16; i++) mainMouseMoveEvent[i] = 0;
	for (int i = 0; i < 16; i++) subItemsCount[i] = 0;
	for (int i = 0; i < 16; i++) mainItems[i] = NULL;
	for (int i = 0; i < 16; i++) mainMenuItems[i] = "";
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) subMenuItems[i][j] = "";
	}
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++)  callbacks[i][j] = NULL;
	}
	visible = false;
	backImage = NULL;
	backSelectImage = NULL;
	backSubImage = NULL;
	mainMenuOpen = false;
	curOpenMainMenu = 0;
	grContext = gc;
	fontContext = fc;
}

menu::~menu() {
	for (int i = 0; i < 16; i++) {
		if (mainItems[i]) {
			deleteEvent(mainButtonDownEvent[i]);
			deleteEvent(mainMouseMoveEvent[i]);
			mainItems[i]->Destroy();
			mainItems[i] = NULL;
		}
	}
	if (backSubImage) {
		grContext->putImage(mx, my, backSubImage);
		grContext->freeImage(backSubImage);
		backSubImage = NULL;
	}
	if (backSelectImage) {
		grContext->putImage(mx, my, backSelectImage);
		grContext->freeImage(backSelectImage);
		backSelectImage = NULL;
	}
	if (backImage != NULL) {
		grContext->putImage(mx, my, backImage);
		grContext->freeImage(backImage);
		backImage = NULL;
	}
	mx = 0;
	my = 0;
	mx1 = 0;
	my1 = 0;
	mainItemsCount = 0;
	for (int i = 0; i < 16; i++) mainButtonDownEvent[i] = 0;
	for (int i = 0; i < 16; i++) mainMouseMoveEvent[i] = 0;
	for (int i = 0; i < 16; i++) subItemsCount[i] = 0;
	for (int i = 0; i < 16; i++) mainMenuItems[i] = "";
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j<16; j++) subMenuItems[i][j] = "";
	}
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++)  callbacks[i][j] = NULL;
	}
	visible = false;
	mainMenuOpen = false;
	curOpenMainMenu = 0;
}

void menu::create(uint16_t x, int16_t y, int16_t width, int16_t heigth) {
	mx = x;
	my = y;
	mx1 = x + width;
	my1 = y + heigth;
}

void menu::addMainMenuItem(std::string name) {
	mainMenuItems[mainItemsCount++] = name;
}

void menu::addSubMenuItem(uint8_t mainMenuItemID, std::string name, std::function<void()> callback) {
	subMenuItems[mainMenuItemID][subItemsCount[mainMenuItemID]] = name;
	callbacks[mainMenuItemID][subItemsCount[mainMenuItemID]] = callback;
	subItemsCount[mainMenuItemID]++;
}

void menu::Visibled(bool vis) {
	if (vis != visible) {
		visible = vis;
		if (visible) {
			backImage = grContext->getImage(mx, my, mx1, my1);
			grContext->SetFillColor(cWINLIGHTGRAY);
			grContext->bar(mx, my, mx1, my1);
			grContext->SetColor(cWINGRAY);
			grContext->rectangle(mx + 2, my + 2, mx1 - 2, my1 - 2);
			grContext->SetColor(cWHITE);
			grContext->rectangle(mx + 3, my + 3, mx1 - 3, my + 3);
			grContext->rectangle(mx + 3, my + 3, mx + 3, my1 - 3);
			grContext->rectangle(mx + 2, my1 - 1, mx1 - 1, my1 - 1);
			grContext->rectangle(mx1 - 1, my + 1, mx1 - 1, my1 - 1);
			uint16_t stx, endx;
			stx = 24;
			for (int i = 0; i < mainItemsCount; i++) {
				mainItems[i] = new tLabel(grContext, fontContext);
				endx = tLabel::getLengthTextInPixels(fontContext, 2, 1, mainMenuItems[i]);
				mainItems[i]->create(stx, 5, endx, 20, cBLACK, mainMenuItems[i]);
				mainItems[i]->setparam(2, 1, 0, 0, 0);
				mainItems[i]->Visibled(true);
				mainButtonDownEvent[i] = addEvent(this, MOUSELEFTBUTTONDOWN, stx, 5, endx, 25, 1, i, NULL);
				mainMouseMoveEvent[i] = addEvent(this, MOUSEMOVE, stx, 5, endx, 25, 1, i, NULL);
				stx += (endx + 20);
			}
		}
		else {
			for (int i = 0; i < mainItemsCount; i++) {
				if (mainItems[i]) {
					deleteEvent(mainButtonDownEvent[i]);
					deleteEvent(mainMouseMoveEvent[i]);
					mainItems[i]->Destroy();
					mainItems[i] = NULL;
				}
			}
			if (backImage != NULL) {
				grContext->putImage(mx, my, backImage);
				grContext->freeImage(backImage);
				backImage = NULL;
			}
		}
	}
}

void menu::Destroy() {

}

void menu::Enabled(bool en) {

}

bool menu::getFocus() {
	return false;
}

void menu::OnKeyDown(uint32_t param1, uint32_t param2) {

}

std::string menu::getText() {
	return "";
}

void menu::getPositionAndSize(int* x, int* y, int* x1, int* y1) {
	*x = mx;
	*y = my;
	*x1 = mx1;
	*y1 = my1;
}

void menu::OnMove(uint32_t param1, uint32_t param2) {
	if (param1 == 1) {
		if (!mainMenuOpen) {
			//printf("On move to MAIN menu item %s\n", mainMenuItems[param2].c_str());
			uint16_t sx = mainItems[param2]->GetX() - 10;
			uint16_t sx1 = mainItems[param2]->GetX1() + 10;
			if (!backSelectImage) backSelectImage = grContext->getImage(sx, 5, sx1, 21);
			grContext->SetColor(cWHITE);
			grContext->line(sx, 5, sx1 - 1, 5);
			grContext->line(sx, 5, sx, 20);
			grContext->SetColor(cWINGRAY);
			grContext->line(sx, 21, sx1, 21);
			grContext->line(sx1, 5, sx1, 21);
		}
		else {
			for (int i = 0; i < subItemsCount[curOpenMainMenu]; i++) {
				deleteEvent(subButtonUpEvent[i]);
				deleteEvent(subMouseMoveEvent[i]);
				subItems[i]->Destroy();
			}
			uint16_t sx = mainItems[curOpenMainMenu]->GetX() - 10;
			uint16_t sx1 = mainItems[curOpenMainMenu]->GetX1() + 10;
			if (backSubImage != NULL) {
				grContext->putImage(sx, 22, backSubImage);
				grContext->freeImage(backSubImage);
				backSubImage = NULL;
			}
			if (backSelectImage != NULL) {
				grContext->putImage(sx, 5, backSelectImage);
				grContext->freeImage(backSelectImage);
				backSelectImage = NULL;
			}

			sx = mainItems[param2]->GetX() - 10;
			sx1 = mainItems[param2]->GetX1() + 10;
			backSelectImage = grContext->getImage(sx, 5, sx1, 21);
			grContext->SetColor(cWINGRAY);
			grContext->line(sx, 5, sx1 - 1, 5);
			grContext->line(sx, 5, sx, 20);
			grContext->SetColor(cWHITE);
			grContext->line(sx, 21, sx1, 21);
			grContext->line(sx1, 5, sx1, 21);
			backSubImage = grContext->getImage(sx, 22, sx + 190, 32 + subItemsCount[param2] * 25);
			grContext->SetFillColor(cWINLIGHTGRAY);
			grContext->bar(sx, 22, sx + 190, 32 + subItemsCount[param2] * 25);
			grContext->SetColor(cWINDARKGRAY);
			grContext->line(sx + 190, 22, sx + 190, 32 + subItemsCount[param2] * 25);
			grContext->line(sx, 32 + subItemsCount[param2] * 25, sx + 190, 32 + subItemsCount[param2] * 25);
			grContext->SetColor(cWHITE);
			grContext->line(sx + 1, 23, sx + 189, 23);
			grContext->line(sx + 1, 23, sx + 1, 31 + subItemsCount[param2] * 25);
			grContext->SetColor(cWINGRAY);
			grContext->line(sx + 189, 23, sx + 189, 31 + subItemsCount[param2] * 25);
			grContext->line(sx + 1, 31 + subItemsCount[param2] * 25, sx + 189, 31 + subItemsCount[param2] * 25);

			uint16_t sty, endy;
			sty = 30;
			for (int i = 0; i < subItemsCount[param2]; i++) {
				subItems[i] = new tLabel(grContext, fontContext);
				subItems[i]->create(sx + 4, sty, 182, 25, cBLACK, subMenuItems[param2][i]);
				subItems[i]->setparam(2, 1, 0, 0, alVCENTER);
				subItems[i]->Visibled(true);
				subButtonUpEvent[i] = addEvent(this, MOUSELEFTBUTTONUP, sx + 4, sty, 182, 25, 2, i, callbacks[param2][i]);
				subMouseMoveEvent[i] = addEvent(this, MOUSEMOVE, sx + 4, sty, 182, 25, 2, i, NULL);
				sty += 25;
			}
			cleanUse(mainButtonDownEvent[param2]);
			setUse(mainMouseMoveEvent[param2]);
			curOpenMainMenu = param2;
		}
	}
	else {
		//printf("On move to sub menu item %s\n", subMenuItems[curOpenMainMenu][param2].c_str());
		subItems[param2]->setBkColor(0x0A246A);
		subItems[param2]->changeColor(cWHITE);
		curOpenSubMenu = param2;
	}
}

void menu::OnEndMove(uint32_t param1, uint32_t param2) {
	if (param1 == 1) {
		if (!mainMenuOpen) {
			//printf("On End move to MAIN menu item %s\n", mainMenuItems[param2].c_str());
			uint16_t sx = mainItems[param2]->GetX() - 10;
			if (backSelectImage != NULL) {
				grContext->putImage(sx, 5, backSelectImage);
				grContext->freeImage(backSelectImage);
				backSelectImage = NULL;
			}
		}
	}
	else {
		subItems[curOpenSubMenu]->setBkColor(cWINLIGHTGRAY);
		subItems[curOpenSubMenu]->changeColor(cBLACK);
	}
}

void menu::OnClick(uint32_t param1, uint32_t param2) {
	//printf("left button down  on menu %d item %s\n",param1, mainMenuItems[param2].c_str());
	if (param1 == 1) {
		if (!mainMenuOpen) {
			uint16_t sx = mainItems[param2]->GetX() - 10;
			if (backSelectImage != NULL) {
				grContext->putImage(sx, 5, backSelectImage);
				grContext->freeImage(backSelectImage);
				backSelectImage = NULL;
			}
			mainMenuOpen = true;
			uint16_t sx1 = mainItems[param2]->GetX1() + 10;
			backSelectImage = grContext->getImage(sx, 5, sx1, 21);
			grContext->SetColor(cWINGRAY);
			grContext->line(sx, 5, sx1 - 1, 5);
			grContext->line(sx, 5, sx, 20);
			grContext->SetColor(cWHITE);
			grContext->line(sx, 21, sx1, 21);
			grContext->line(sx1, 5, sx1, 21);
			backSubImage = grContext->getImage(sx, 22, sx+190, 32 + subItemsCount[param2] * 25);
			grContext->SetFillColor(cWINLIGHTGRAY);
			grContext->bar(sx, 22, sx + 190, 32 + subItemsCount[param2] * 25);
			grContext->SetColor(cWINDARKGRAY);
			grContext->line(sx + 190, 22, sx + 190, 32 + subItemsCount[param2] * 25);
			grContext->line(sx, 32 + subItemsCount[param2] * 25, sx + 190, 32 + subItemsCount[param2] * 25);
			grContext->SetColor(cWHITE);
			grContext->line(sx+1, 23, sx + 189, 23);
			grContext->line(sx + 1, 23, sx + 1, 31 + subItemsCount[param2] * 25);
			grContext->SetColor(cWINGRAY);
			grContext->line(sx + 189, 23, sx + 189, 31 + subItemsCount[param2] * 25);
			grContext->line(sx+1, 31 + subItemsCount[param2] * 25, sx + 189, 31 + subItemsCount[param2] * 25);


			uint16_t sty, endy;
			sty = 30;
			for (int i = 0; i < subItemsCount[param2]; i++) {
				subItems[i] = new tLabel(grContext, fontContext);
				subItems[i]->create(sx+4, sty, 182, 25, cBLACK, subMenuItems[param2][i]);
				subItems[i]->setparam(2, 1, 0, 0, alVCENTER);
				subItems[i]->Visibled(true);
				subButtonUpEvent[i] = addEvent(this, MOUSELEFTBUTTONUP, sx + 4, sty, 182, 25, 2, i, callbacks[param2][i]);
				subMouseMoveEvent[i] = addEvent(this, MOUSEMOVE, sx + 4, sty, 182, 25, 2, i, NULL);
				sty += 25;
			}
			cleanUse(mainButtonDownEvent[param2]);
			setUse(mainMouseMoveEvent[param2]);
			curOpenMainMenu = param2;
		}
		else {
			mainMenuOpen = false;
			for (int i = 0; i < subItemsCount[param2]; i++) {
				deleteEvent(subButtonUpEvent[i]);
				deleteEvent(subMouseMoveEvent[i]);
				subItems[i]->Destroy();
			}
			uint16_t sx = mainItems[param2]->GetX() - 10;
			uint16_t sx1 = mainItems[param2]->GetX1() + 10;
			if (backSubImage != NULL) {
				grContext->putImage(sx, 22, backSubImage);
				grContext->freeImage(backSubImage);
				backSubImage = NULL;
			}
			if (backSelectImage != NULL) {
				grContext->putImage(sx, 5, backSelectImage);
				grContext->freeImage(backSelectImage);
				backSelectImage = NULL;
			}
			backSelectImage = grContext->getImage(sx, 5, sx1, 21);
			grContext->SetColor(cWHITE);
			grContext->line(sx, 5, sx1 - 1, 5);
			grContext->line(sx, 5, sx, 20);
			grContext->SetColor(cWINGRAY);
			grContext->line(sx, 21, sx1, 21);
			grContext->line(sx1, 5, sx1, 21);
			cleanUse(mainButtonDownEvent[param2]);
			setUse(mainMouseMoveEvent[param2]);
		}
	}
}

void menu::OnClickUp(uint32_t param1, uint32_t param2) {
	//printf("left button up on menu %d item %s\n", param1, subMenuItems[curOpenMainMenu][param2].c_str());
	std::function<void()> curCallback = callbacks[curOpenMainMenu][param2];
	for (int i = 0; i < subItemsCount[curOpenMainMenu]; i++) {
		deleteEvent(subButtonUpEvent[i]);
		deleteEvent(subMouseMoveEvent[i]);
		subItems[i]->Destroy();
	}
	uint16_t sx = mainItems[curOpenMainMenu]->GetX() - 10;
	uint16_t sx1 = mainItems[curOpenMainMenu]->GetX1() + 10;
	if (backSubImage != NULL) {
		grContext->putImage(sx, 22, backSubImage);
		grContext->freeImage(backSubImage);
		backSubImage = NULL;
	}
	if (backSelectImage != NULL) {
		grContext->putImage(sx, 5, backSelectImage);
		grContext->freeImage(backSelectImage);
		backSelectImage = NULL;
	}
	mainMenuOpen = false;
	curCallback();
}