#ifndef __OPENDIALOG_H__
#define __OPENDIALOG_H__

#include <stdint.h>
#include <string>
#include "window.h"


class OpenDialog : public Window {
public:
	OpenDialog(std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev);
	virtual ~OpenDialog();
	bool eventManager(SDL_Event event);


private:

	string* getDisks();
	string GetThisPath();

	tLabel* folderText;
	tComboBox* disks;
	tFileList* fl;
	string* diskNames;

	uint16_t moveEvents[16];
	uint16_t clickEvents[16];
	uint16_t numEventHexViewerClick;
	uint16_t numEventHexViewerClickUp;
	uint16_t numEventHexViewerScrol;

};


#endif // !__OPENDIALOG_H__

