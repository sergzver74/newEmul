#include "openDialog.h"
#include "fs.h"

OpenDialog::OpenDialog(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev, std::string msk) : Window(name, x, y, w, h, ev) {
	
	masks = new string[3];
	masks[0] = msk;
	masks[1] = "*.*";
	masks[2] = "";
	currentMask = masks[0];

	
	diskNames = getDisks();
	path = GetThisPath();
	printf("%s\n", path.c_str());
	uint32_t n = 0;
	WIN32_FIND_DATAA* files = getFilesList(path, &n, currentMask);

	string dsk = "";
	dsk += path[0];
	dsk += path[1];

	uint16_t indx = 0;
	while (diskNames[indx] != "") {
		string dsk2 = "";
		dsk2 += diskNames[indx][diskNames[indx].length()-3];
		dsk2 += diskNames[indx][diskNames[indx].length() - 2];
		if (dsk == dsk2) break;
		indx++;
	}

	selected = false;
	fullPath = "";

	folderText = new tLabel(gContext, fContext);
	folderText->create(10, 5, 50, 20, cBLACK, "Диск:");
	folderText->setparam(2, 1, 0, 0, 0);
	folderText->Visibled(true);

	disks = new tComboBox(gContext, fContext, ev);
	disks->create(50, 5, 250, 22, 2, 1, diskNames, indx);
	disks->Visibled(true);
	addEvent(disks, 1, 50, 5, 250, 22, 0, 0);

	for (int i = 0; i < 16; i++) moveEvents[i] = 0;
	for (int i = 0; i < 16; i++) clickEvents[i] = 0;

	fl = new tFileList(gContext, fContext, files, n, winID);
	fl->create(10, 40);
	fl->Visibled(true);

	for (int i = 0; i < 16; i++) {
		flEvents[i] = addEvent(fl, 1, 15, 65 + (20 * i), 570, 20, 1, i);
		flEvents1[i] = addEvent(fl, 5, 15, 65 + (20 * i), 570, 20, 1, i);
	}
	numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 350, 0, 0);
	numEventHexViewerClickUp = addEvent(fl, 2, 590, 40, 20, 350, 0, 0);
	numEventHexViewerScrol = addEvent(fl, 4, 591, 40, 16, 310, 0, 0);

	nameText = new tLabel(gContext, fContext);
	nameText->create(10, 400, 80, 20, cBLACK, "Имя файла:");
	nameText->setparam(2, 1, 0, 0, 0);
	nameText->Visibled(true);

	fileName = new tEdit(gContext, fContext, winID, userEventType);
	fileName->create(90, 400, 350, 20, 2, 1, "");
	fileName->Visibled(true);

	addEvent(fileName, 2, 90, 400, 350, 20, 0, 0);
	addEvent(fileName, 3, 0, 0, 0, 0, 0, 0);
	addEvent(fileName, userEventType, 0, 0, 0, 0, 0, 0);

	mask = new tComboBox(gContext, fContext, ev);
	mask->create(460, 400, 150, 22, 2, 1, masks, 0);
	mask->Visibled(true);
	addEvent(mask, 1, 460, 400, 150, 22, 0, 0);

	open = new tButton(gContext, fContext);
	open->create(440, 450, 80, 25, "Открыть");
	open->Visibled(true);
	openClickEvent = addEvent(open, 1, 440, 450, 80, 25, 0, 0);
	openClickUpEvent = addEvent(open, 2, 440, 450, 80, 25, 0, 0);

	cancel = new tButton(gContext, fContext);
	cancel->create(530, 450, 80, 25, "Отмена");
	cancel->Visibled(true);
	cancelClickEvent = addEvent(cancel, 1, 530, 450, 80, 25, 0, 0);
	cancelClickUpEvent = addEvent(cancel, 2, 530, 450, 80, 25, 0, 0);

	updateWindow();

}

OpenDialog::~OpenDialog() {
	printf("Destroy OpenDialog window\n");
	delete mask;
	delete fileName;
	delete nameText;
	delete fl;
	delete disks;
	delete folderText;
	delete[] diskNames;
	for (int i = 0; i < 16; i++) moveEvents[i] = 0;
	for (int i = 0; i < 16; i++) clickEvents[i] = 0;
}

bool OpenDialog::eventManager(SDL_Event event) {

	uint32_t evType = 0;
	if (event.type == SDL_MOUSEBUTTONDOWN) evType = 1;
	if (event.type == SDL_MOUSEBUTTONUP) evType = 2;
	if (event.type == SDL_MOUSEMOTION) evType = 4;
	if (event.type == SDL_MOUSEBUTTONUP && event.button.clicks == 2) evType = 5;

	int i = 0;
	for (i = 0; i < 512; i++) {
		if (winEvents[i].guiElement != NULL) {
			if (winEvents[i].evType == evType) {

				int32_t x, y;
				SDL_GetMouseState(&x, &y);

				if (x >= winEvents[i].x && x <= winEvents[i].x1 && y >= winEvents[i].y && y <= winEvents[i].y1) {
					if (evType == 1) {
						if (winEvents[i].guiElement == disks) {
							disks->OnClick(winEvents[i].param1, winEvents[i].param2);
							if (winEvents[i].param1 == 0) {
								if (disks->isOpen()) {
									for (int i = 0; i < 16; i++) deleteEvent(flEvents[i]);
									for (int i = 0; i < disks->getCount(); i++) {
										moveEvents[i] = addEvent(disks, 4, 55, 28 + 10 + i * 25, 250, 25, 1, i);
										clickEvents[i] = addEvent(disks, 1, 55, 28 + 10 + i * 25, 250, 25, 1, i);
									}
								}
								else {
									for (int i = 0; i < disks->getCount(); i++) {
										deleteEvent(moveEvents[i]);
										deleteEvent(clickEvents[i]);
									}
									for (int i = 0; i < 16; i++) {
										flEvents[i] = addEvent(fl, 1, 15, 65 + (20 * i), 570, 20, 1, i);
									}
								}
							}
							if (winEvents[i].param1 == 1) {
								string selDisk = disks->getText();
								for (int i = 0; i < disks->getCount(); i++) {
									deleteEvent(moveEvents[i]);
									deleteEvent(clickEvents[i]);
								}
								for (int i = 0; i < 16; i++) {
									flEvents[i] = addEvent(fl, 1, 15, 65 + (20 * i), 570, 20, 1, i);
								}

								printf("Selected %s\n", selDisk.c_str());

								uint32_t k = selDisk.length();
								path = selDisk.substr(k-3, 2);
								printf("new path: %s\n", path.c_str());

								uint32_t n = 0;
								//delete[] files;
								files = getFilesList(path, &n, currentMask);
								for (int i = 0; i < 16; i++) {
									deleteEvent(flEvents[i]);
									deleteEvent(flEvents1[i]);
								}
								deleteEvent(numEventHexViewerClick);
								deleteEvent(numEventHexViewerClickUp);
								deleteEvent(numEventHexViewerScrol);
								fl->Visibled(false);
								delete fl;
								fl = new tFileList(gContext, fContext, files, n, winID);
								fl->create(10, 40);
								fl->Visibled(true);

								for (int i = 0; i < 16; i++) {
									flEvents[i] = addEvent(fl, 1, 15, 65 + (20 * i), 570, 20, 1, i);
									flEvents1[i] = addEvent(fl, 5, 15, 65 + (20 * i), 570, 20, 1, i);
								}
								numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 340, 0, 0);
								numEventHexViewerClickUp = addEvent(fl, 2, 590, 40, 20, 340, 0, 0);
								numEventHexViewerScrol = addEvent(fl, 4, 591, 40, 16, 300, 0, 0);

							}
							break;
						}
						if (winEvents[i].guiElement == mask) {
							mask->OnClick(winEvents[i].param1, winEvents[i].param2);
							if (winEvents[i].param1 == 0) {
								if (mask->isOpen()) {
									deleteEvent(openClickEvent);
									deleteEvent(openClickUpEvent);
									deleteEvent(cancelClickEvent);
									deleteEvent(cancelClickUpEvent);
									for (int i = 0; i < mask->getCount(); i++) {
										moveEvents[i] = addEvent(mask, 4, 460, 423 + 10 + i * 25, 150, 25, 1, i);
										clickEvents[i] = addEvent(mask, 1, 460, 423 + 10 + i * 25, 150, 25, 1, i);
									}
								}
								else {
									for (int i = 0; i < mask->getCount(); i++) {
										deleteEvent(moveEvents[i]);
										deleteEvent(clickEvents[i]);
									}
									openClickEvent = addEvent(open, 1, 440, 450, 80, 25, 0, 0);
									openClickUpEvent = addEvent(open, 2, 440, 450, 80, 25, 0, 0);
									cancelClickEvent = addEvent(cancel, 1, 530, 450, 80, 25, 0, 0);
									cancelClickUpEvent = addEvent(cancel, 2, 530, 450, 80, 25, 0, 0);
								}
							}
							if (winEvents[i].param1 == 1) {
								string selDisk = mask->getText();
								for (int i = 0; i < mask->getCount(); i++) {
									deleteEvent(moveEvents[i]);
									deleteEvent(clickEvents[i]);
								}
								openClickEvent = addEvent(open, 1, 440, 450, 80, 25, 0, 0);
								openClickUpEvent = addEvent(open, 2, 440, 450, 80, 25, 0, 0);
								cancelClickEvent = addEvent(cancel, 1, 530, 450, 80, 25, 0, 0);
								cancelClickUpEvent = addEvent(cancel, 2, 530, 450, 80, 25, 0, 0);

								printf("Selected %s\n", selDisk.c_str());
								currentMask = selDisk;
								printf("new path: %s\n", path.c_str());

								uint32_t n = 0;
								//delete[] files;
								files = getFilesList(path, &n, currentMask);
								for (int i = 0; i < 16; i++) {
									deleteEvent(flEvents[i]);
									deleteEvent(flEvents1[i]);
								}
								deleteEvent(numEventHexViewerClick);
								deleteEvent(numEventHexViewerClickUp);
								deleteEvent(numEventHexViewerScrol);
								fl->Visibled(false);
								delete fl;
								fl = new tFileList(gContext, fContext, files, n, winID);
								fl->create(10, 40);
								fl->Visibled(true);

								for (int i = 0; i < 16; i++) {
									flEvents[i] = addEvent(fl, 1, 15, 65 + (20 * i), 570, 20, 1, i);
									flEvents1[i] = addEvent(fl, 5, 15, 65 + (20 * i), 570, 20, 1, i);
								}
								numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 340, 0, 0);
								numEventHexViewerClickUp = addEvent(fl, 2, 590, 40, 20, 340, 0, 0);
								numEventHexViewerScrol = addEvent(fl, 4, 591, 40, 16, 300, 0, 0);

							}
							break;
						}
						if (winEvents[i].guiElement == fl) {
							winEvents[i].param1 = x;
							winEvents[i].param2 = y;
							winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
							string s = fl->getSelectedFileName();
							printf("Select: %s\n", s.c_str());
							if (s != "." && s != ".." && s!="") {
								fileName->setText(s);
							}
						}
						if (winEvents[i].guiElement == open) {
							winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
						}
						if (winEvents[i].guiElement == cancel) {
							winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
						}
					}
					if (evType == 2) {
						winEvents[i].param1 = x;
						winEvents[i].param2 = y;
						winEvents[i].guiElement->OnClickUp(winEvents[i].param1, winEvents[i].param2);
						if (winEvents[i].guiElement == open) {
							string selName = fl->getSelectedFileName();
							printf("Selected filename: %s\n", selName.c_str());
							if (selName == "") continue;
							string selDir = fl->getSelectedFileSize();
							printf("Selected filesize: %s\n", selDir.c_str());
							if (selDir != "DIR") {
								selected = true;
								fullPath = path + "\\" + selName;
								printf("Select file: %s\n", fullPath.c_str());
								openResultCode = 1;
								return true;
							}
							else {
								if (selName == "..") {
									int l = path.length() - 1;
									while (path[l] != '\\') {
										l--;
									}
									path.erase(l, path.length() - l);
									printf("New path: %s\n", path.c_str());

								}
								else
									if (selName != ".") {
										path += ("\\" + selName);
									}
								if (selName != ".") {
									uint32_t n = 0;
									//delete[] files;
									files = getFilesList(path, &n, currentMask);
									for (int i = 0; i < 16; i++) {
										deleteEvent(flEvents[i]);
										deleteEvent(flEvents1[i]);
									}
									deleteEvent(numEventHexViewerClick);
									deleteEvent(numEventHexViewerClickUp);
									deleteEvent(numEventHexViewerScrol);
									fl->Visibled(false);
									delete fl;
									fl = new tFileList(gContext, fContext, files, n, winID);
									fl->create(10, 40);
									fl->Visibled(true);

									for (int i = 0; i < 16; i++) {
										flEvents[i] = addEvent(fl, 1, 15, 65 + (20 * i), 570, 20, 1, i);
										flEvents1[i] = addEvent(fl, 5, 15, 65 + (20 * i), 570, 20, 1, i);
									}
									numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 340, 0, 0);
									numEventHexViewerClickUp = addEvent(fl, 2, 590, 40, 20, 340, 0, 0);
									numEventHexViewerScrol = addEvent(fl, 4, 591, 40, 16, 300, 0, 0);
								}
							}
						}
						if (winEvents[i].guiElement == cancel) {
							openResultCode = 2;
							return true;
						}
					}
					if (evType == 4) {
						if (winEvents[i].guiElement == disks) {
							disks->OnMove(winEvents[i].param1, winEvents[i].param2);
						}
						if (winEvents[i].guiElement == mask) {
							mask->OnMove(winEvents[i].param1, winEvents[i].param2);
						}
						if (winEvents[i].guiElement == fl) {
							winEvents[i].param1 = x;
							winEvents[i].param2 = y;
							winEvents[i].guiElement->OnMove(winEvents[i].param1, winEvents[i].param2);
						}
					}
					if (evType == 5) {
						if (winEvents[i].guiElement == fl) {
							string selName = fl->getSelectedFileName();
							printf("Selected filename: %s\n", selName.c_str());
							string selDir = fl->getSelectedFileSize(); 
							printf("Selected filesize: %s\n", selDir.c_str());
							if (selDir != "DIR") {
								selected = true;
								fullPath = path + "\\" + selName;
								printf("Select file: %s\n", fullPath.c_str());
								openResultCode = 1;
								return true;
							}
							else {
								if (selName == "..") {
									int l = path.length() - 1;
									while (path[l] != '\\') {
										l--;
									}
									path.erase(l, path.length() - l);
									printf("New path: %s\n", path.c_str());

								} else
								if (selName != ".") {
									path += ("\\" + selName);
								}
								if (selName != ".") {
									uint32_t n = 0;
									//delete[] files;
									files = getFilesList(path, &n, currentMask);
									for (int i = 0; i < 16; i++) {
										deleteEvent(flEvents[i]);
										deleteEvent(flEvents1[i]);
									}
									deleteEvent(numEventHexViewerClick);
									deleteEvent(numEventHexViewerClickUp);
									deleteEvent(numEventHexViewerScrol);
									fl->Visibled(false);
									delete fl;
									fl = new tFileList(gContext, fContext, files, n, winID);
									fl->create(10, 40);
									fl->Visibled(true);

									for (int i = 0; i < 16; i++) {
										flEvents[i] = addEvent(fl, 1, 15, 65 + (20 * i), 570, 20, 1, i);
										flEvents1[i] = addEvent(fl, 5, 15, 65 + (20 * i), 570, 20, 1, i);
									}
									numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 340, 0, 0);
									numEventHexViewerClickUp = addEvent(fl, 2, 590, 40, 20, 340, 0, 0);
									numEventHexViewerScrol = addEvent(fl, 4, 591, 40, 16, 300, 0, 0);
								}
							}
						}
					}
				}
			}
		}
	}


	updateWindow();
	return false;
}

uint8_t OpenDialog::getresultCode() {
	return openResultCode;
}

string OpenDialog::getResult() {
	return fullPath;
}