#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdint.h>
#include <SDL.h>
#include "window.h"
#include "graph.h"
#include "fonts.h"
#include "gui.h"
#include "machine.h"
#include "emutypes.h"
#include "hex.h"
#include "breakpoints.h"

#define DISASSEMBLERROWS	20

class Debug: public Window {
private:
	tLabel *regText;
	tLabel* disText;
	Registers regs;
	CPU* vcpu;
	Machine* computer;
	tLabel* regNames[32];
	tLabel* regValues[32];
	tLabel* flagName[16];
	tLabel* flagValue[16];
	tLabel* disasmText;

	tButton* btnTrace;
	tButton* btnStep;
	tButton* btnRunTo;
	tButton* btnStop;
	tButton* btnStart;
	tEdit* edtAddr;
	tHexViewer *hexViewer;
	tButton* btnROM;
	tButton* btnRAM;

	tButton* btnKvaz1;
	tButton* btnKvaz2;
	tButton* btnKvaz3;
	tButton* btnKvaz4;

	uint16_t numEventHexViewerClick;
	uint16_t numEventHexViewerClickUp;
	uint16_t numEventHexViewerScrol;

	tEdit* tempEdit;
	uint16_t tempEditKeyEventNom;
	uint16_t tempEditCursorEventNum;

	Breakpoints* breakPoints;
	tBreakPointSet* breakPointSet;

	void updateData();

public:
	Debug(Machine *m, std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev, Breakpoints* bp);
	virtual ~Debug();
	bool eventManager(SDL_Event event);
	void updateDataNeeded();

};

#endif // !__DEBUG_H__
