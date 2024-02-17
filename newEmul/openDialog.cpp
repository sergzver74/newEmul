#include "openDialog.h"
#include <shlwapi.h>


#pragma comment(lib, "shlwapi.lib")

string* OpenDialog::getDisks() {
	
	string* names = new string[32];
	for (int i = 0; i < 32; i++) names[i] = "";
	
	uint32_t disks = GetLogicalDrives();
	printf("disks: %d\n", disks);

	int i = 0;

	char buf[255];
	int sz = GetLogicalDriveStringsA(sizeof(buf), buf);
	if (sz > 0)
	{
		char* p1 = buf;
		char* p2;

		char drive_label[256];
		char drive_label1[256];
		char drive_fat[30];
		DWORD drive_sn;
		DWORD drive_name_size = sizeof(drive_label);

		while (*p1 != '\0' && (p2 = strchr(p1, '\0')) != NULL)
		{

			bool Flag = GetVolumeInformationA(p1,
				drive_label,
				sizeof(drive_label),
				&drive_sn,
				&drive_name_size,
				NULL,
				drive_fat,
				sizeof(drive_fat)
			);
			if (Flag)
			{
				//if (drive_label[0]!=0) CharToOemBuffA(drive_label, drive_label1, 256); else CharToOemBuffA("Локальный диск", drive_label1, 256);

				//names[i] = string(drive_label1) + " (" + string(p1,2) + ")";
				if (drive_label[0] != 0) names[i] = string(drive_label) + " (" + string(p1, 2) + ")"; else names[i] = string("Локальный диск") + " (" + string(p1, 2) + ")";
				printf("%s\n", names[i].c_str());
				i++;
			}

			p1 = p2 + 1;
		}
	}
	return names;
}

string OpenDialog::GetThisPath()
{
	char dest[MAX_PATH];

	DWORD length = GetModuleFileNameA(NULL, dest, MAX_PATH);
	PathRemoveFileSpecA(dest);
	return string(dest);
}

void sort(WIN32_FIND_DATAA* fd, uint16_t n) {
	if (n > 1)
	{
		for (int i = 0; i < n - 1; i++) {
			int min = i;
			for (int j = i + 1; j < n; j++) {
				if (strcmp(fd[j].cFileName, fd[min].cFileName) < 0)
					min = j;
			}
			if (min != i)
			{
				WIN32_FIND_DATAA f = fd[i];
				fd[i] = fd[min];
				fd[min] = f;
			}
		}
	}
}

WIN32_FIND_DATAA* getFilesList(string path, uint32_t *n, string mask) {
	WIN32_FIND_DATAA f;
	WIN32_FIND_DATAA* fd;
	WIN32_FIND_DATAA* fd1;
	fd = new WIN32_FIND_DATAA[1024];
	fd1 = new WIN32_FIND_DATAA[1024];
	int i = 0;
	int k = 0;
	string nPath = path + "\\*.*";
	HANDLE hFind = FindFirstFileA(nPath.c_str(), &f);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (f.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				printf("  %s   <DIR>\n", f.cFileName);
				fd[i] = f;
				i++;
			}
			else {
				printf("  %s   %d bytes\n", f.cFileName, f.nFileSizeLow);
				string nm = string(f.cFileName);
				if (mask == "*.*") {
					fd1[k] = f;
					k++;
				}
				else {
					if (nm[nm.length() - 4] == mask[mask.length() - 4] && nm[nm.length() - 3] == mask[mask.length() - 3] && nm[nm.length() - 2] == mask[mask.length() - 2] && nm[nm.length() - 1] == mask[mask.length() - 1]) {
						fd1[k] = f;
						k++;
					}
				}
			}
		} while (FindNextFileA(hFind, &f));
		FindClose(hFind);
	}

	sort(fd, i);
	sort(fd1, k);

	WIN32_FIND_DATAA* fdNew;
	fdNew = new WIN32_FIND_DATAA[i + k];
	for (int l = 0; l < i; l++) fdNew[l] = fd[l];
	for (int l = 0; l < k; l++) fdNew[i + l] = fd1[l];
	delete[] fd1;
	delete[] fd;

	*n = i + k;
	return fdNew;
}


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