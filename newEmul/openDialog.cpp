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

WIN32_FIND_DATAA* getFilesList(string path, uint32_t *n) {
	WIN32_FIND_DATAA *fd;
	fd = new WIN32_FIND_DATAA[1024];
	int i = 0;
	HANDLE hFind = FindFirstFileA(path.c_str(), &fd[i]);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (fd[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				printf("  %s   <DIR>\n", fd[i].cFileName);
			}
			else {
				printf("  %s   %d bytes\n", fd[i].cFileName, fd[i].nFileSizeLow);
			}
			i++;
		} while (FindNextFileA(hFind, &fd[i]));
		FindClose(hFind);
	}
	*n = i;
	sort
	return fd;
}


OpenDialog::OpenDialog(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev) : Window(name, x, y, w, h, ev) {
	diskNames = getDisks();
	path = GetThisPath();
	printf("%s\n", path.c_str());
	uint32_t n = 0;
	WIN32_FIND_DATAA* files = getFilesList(path + "\\*.*", &n);

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
		flEvents[i] = addEvent(fl, 1, 15, 45 + (20 * i), 590, 20, 1, i);
		flEvents1[i] = addEvent(fl, 5, 15, 45 + (20 * i), 590, 20, 1, i);
	}
	numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 340, 0, 0);
	numEventHexViewerClickUp = addEvent(fl, 2, 590, 20, 20, 340, 0, 0);
	numEventHexViewerScrol = addEvent(fl, 4, 591, 40, 16, 300, 0, 0);

	updateWindow();

}

OpenDialog::~OpenDialog() {
	printf("Destroy OpenDialog window\n");
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
										flEvents[i] = addEvent(fl, 1, 15, 45 + (20 * i), 590, 20, 1, i);
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
									flEvents[i] = addEvent(fl, 1, 15, 45 + (20 * i), 590, 20, 1, i);
								}

								printf("Selected %s\n", selDisk.c_str());

								uint32_t k = selDisk.length();
								path = selDisk.substr(k-3, 2);
								printf("new path: %s\n", path.c_str());

								uint32_t n = 0;
								//delete[] files;
								files = getFilesList(path + "\\*.*", &n);
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
									flEvents[i] = addEvent(fl, 1, 15, 45 + (20 * i), 590, 20, 1, i);
									flEvents1[i] = addEvent(fl, 5, 15, 45 + (20 * i), 590, 20, 1, i);
								}
								numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 340, 0, 0);
								numEventHexViewerClickUp = addEvent(fl, 2, 590, 20, 20, 340, 0, 0);
								numEventHexViewerScrol = addEvent(fl, 4, 591, 40, 16, 300, 0, 0);

							}
							break;
						}
						if (winEvents[i].guiElement == fl) {
							winEvents[i].param1 = x;
							winEvents[i].param2 = y;
							winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
						}
					}
					if (evType == 4) {
						if (winEvents[i].guiElement == disks) {
							disks->OnMove(winEvents[i].param1, winEvents[i].param2);
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
									files = getFilesList(path + "\\*.*", &n);
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
										flEvents[i] = addEvent(fl, 1, 15, 45 + (20 * i), 590, 20, 1, i);
										flEvents1[i] = addEvent(fl, 5, 15, 45 + (20 * i), 590, 20, 1, i);
									}
									numEventHexViewerClick = addEvent(fl, 1, 590, 40, 20, 340, 0, 0);
									numEventHexViewerClickUp = addEvent(fl, 2, 590, 20, 20, 340, 0, 0);
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