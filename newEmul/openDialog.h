#ifndef __OPENDIALOG_H__
#define __OPENDIALOG_H__

#include <stdint.h>
#include <string>
#include "window.h"


class OpenDialog : public Window {
public:
	OpenDialog(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev, std::string msk);
	virtual ~OpenDialog();
	bool eventManager(SDL_Event event);
	uint8_t getresultCode();
	string getResult();


private:

	tLabel* folderText;
	tLabel* nameText;
	tEdit* fileName;
	tComboBox* mask;
	tButton* open;
	tButton* cancel;
	tComboBox* disks;
	tFileList* fl;
	uint16_t flEvents[16];
	uint16_t flEvents1[16];
	string* diskNames;
	bool selected;
	string fullPath;
	string path;
	WIN32_FIND_DATAA* files;
	string currentMask;
	string* masks;
	uint16_t openClickUpEvent;
	uint16_t openClickEvent;
	uint16_t cancelClickUpEvent;
	uint16_t cancelClickEvent;
	uint8_t openResultCode;

	uint16_t moveEvents[16];
	uint16_t clickEvents[16];
	uint16_t numEventHexViewerClick;
	uint16_t numEventHexViewerClickUp;
	uint16_t numEventHexViewerScrol;

};


#endif // !__OPENDIALOG_H__

