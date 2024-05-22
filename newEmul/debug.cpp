#include "debug.h"

Debug::Debug(Machine* m, std::string name, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint32_t ev) : Window(name, x, y, w, h, ev) {

	computer = m;
	vcpu = m->getCPU();
	regs = vcpu->getRegisters();
	
	regText = new tLabel(gContext, fContext);
	regText->create(800, 20, 80, 20, cBLACK, "Регистры:");
	regText->setparam(2, 1, 0, 0, 0);
	regText->Visibled(true);
	disText = new tLabel(gContext, fContext);
	disText->create(620, 20, 150, 20, cBLACK, "Дисассемблер:");
	disText->setparam(2, 1, 0, 0, 0);
	disText->Visibled(true);

	for (int i = 0; i < 32; i++) {
		regNames[i] = NULL;
		regValues[i] = NULL;
	}
	for (int i = 0; i < 16; i++) {
		flagName[i] = NULL;
		flagValue[i] = NULL;
	}
	
	for (int i = 0; i < regs.count; i++) {
		regNames[i] = new tLabel(gContext, fContext);
		regNames[i]->create(800, 40 + 20*i, 60, 19, cBLACK, regs.registers[i].name);
		regNames[i]->setparam(0, 1, 0, 0, 0);
		regNames[i]->Visibled(true);
		regValues[i] = new tLabel(gContext, fContext);
		regValues[i]->create(870, 40 + 20 * i, 40, 19, cBLACK, regs.registers[i].strData);
		regValues[i]->setparam(0, 1, 0, 0, 0);
		regValues[i]->Visibled(true);
		addEvent(regValues[i], 5, 870, 40 + 20 * i, 40, 19, 1, i);
	}

	for (int i = 0; i < regs.flags.count; i++) {
		flagName[i] = new tLabel(gContext, fContext);
		flagName[i]->create(800 + 20 *i, 40 + regs.count * 20, 20, 19, cBLACK, regs.flags.flags[i].name);
		flagName[i]->setparam(0, 1, 0, 0, 0);
		flagName[i]->Visibled(true);
		flagValue[i] = new tLabel(gContext, fContext);
		flagValue[i]->create(800 + 20 * i, 60 + regs.count * 20, 20, 19, cBLACK, regs.flags.flags[i].strData);
		flagValue[i]->setparam(0, 1, 0, 0, 0);
		flagValue[i]->Visibled(true);
		addEvent(flagValue[i], 5, 800 + 20 * i, 60 + regs.count * 20, 20, 19, 2, i);
	}

	uint16_t addr = vcpu->getPC();
	disasmText = new tLabel(gContext, fContext);
	disasmText->create(620, 40, 180, 350, cBLACK, vcpu->disAsm(&addr, DISASSEMBLERROWS));
	disasmText->setparam(0, 1, 0, 0, 0);
	disasmText->Visibled(true);
	for (int i = 0; i < DISASSEMBLERROWS; i++) {
		addEvent(disasmText, 5, 620, 40 + 16*i, 180, 16, 3, i);
	}

	btnTrace = new tButton(gContext, fContext);
	btnTrace->create(800, 600, 60, 20, "Trace");
	btnTrace->Visibled(true);
	addEvent(btnTrace, 1, 800, 600, 60, 20, 0, 0);
	addEvent(btnTrace, 2, 800, 600, 60, 20, 0, 0);

	btnStep = new tButton(gContext, fContext);
	btnStep->create(800, 630, 60, 20, "Step");
	btnStep->Visibled(true);
	addEvent(btnStep, 1, 800, 630, 60, 20, 0, 0);
	addEvent(btnStep, 2, 800, 630, 60, 20, 0, 0);

	btnRunTo = new tButton(gContext, fContext);
	btnRunTo->create(800, 660, 60, 25, "RunTo");
	btnRunTo->Visibled(true);
	addEvent(btnRunTo, 1, 800, 660, 60, 25, 0, 0);
	addEvent(btnRunTo, 2, 800, 660, 60, 25, 0, 0);

	btnStop = new tButton(gContext, fContext);
	btnStop->create(800, 690, 60, 20, "Stop");
	btnStop->Visibled(true);
	addEvent(btnStop, 1, 800, 690, 60, 20, 0, 0);
	addEvent(btnStop, 2, 800, 690, 60, 20, 0, 0);

	btnStart = new tButton(gContext, fContext);
	btnStart->create(800, 720, 60, 20, "Start");
	btnStart->Visibled(true);
	addEvent(btnStart, 1, 800, 720, 60, 20, 0, 0);
	addEvent(btnStart, 2, 800, 720, 60, 20, 0, 0);

	edtAddr = new tEdit(gContext, fContext, winID, userEventType);
	edtAddr->create(870, 660, 60, 25, 2, 1, "");
	edtAddr->Visibled(true);
	edtAddr->setMaxSymbols(4);
	//addEvent(edtAddr, 1, 800, 660, 60, 20, 0, 0);
	addEvent(edtAddr, 2, 870, 660, 60, 25, 0, 0);
	addEvent(edtAddr, 3, 0, 0, 0, 0, 0, 0);
	addEvent(edtAddr, userEventType, 0, 0, 0, 0, 0, 0);

	hexViewer = new tHexViewer(gContext, fContext, computer->getMemory(), true, winID);
	hexViewer->create(10, 20);
	hexViewer->Visibled(true);
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			addEvent(hexViewer, 5, 10 + 58 + (24 * j), 20 + (20 * i) + 10, 20, 20, 4, i << 8 | j);
		}
	}
	numEventHexViewerClick = addEvent(hexViewer, 1, 10, 20, 600, 340, 0, 0);
	numEventHexViewerClickUp = addEvent(hexViewer, 2, 10, 20, 600, 340, 0, 0);
	numEventHexViewerScrol = addEvent(hexViewer, 4, 591, 40, 16, 300, 0, 0);

	btnROM = new tButton(gContext, fContext);
	btnROM->create(200, 370, 60, 25, "ROM");
	btnROM->Visibled(true);
	addEvent(btnROM, 1, 200, 370, 60, 25, 0, 0);
	addEvent(btnROM, 2, 200, 370, 60, 25, 0, 0);

	btnRAM = new tButton(gContext, fContext);
	btnRAM->create(300, 370, 60, 25, "RAM");
	btnRAM->Visibled(true);
	addEvent(btnRAM, 1, 300, 370, 60, 25, 0, 0);
	addEvent(btnRAM, 2, 300, 370, 60, 25, 0, 0);

	tempEdit = NULL;

	updateWindow();
}

Debug::~Debug() {
	printf("Destroy DEBUG window\n");
	
	if (tempEdit != NULL) {
		delete tempEdit;
		tempEdit = NULL;
	}

	delete btnRAM;
	delete btnROM;
	delete hexViewer;
	delete edtAddr;
	delete btnRunTo;
	delete btnStep;
	delete btnTrace;
	
	delete disasmText;
	for (int i = 0; i < 16; i++) {
		if (flagName[i]) {
			delete flagName[i];
			flagName[i] = NULL;
		}
		if (flagValue[i]) {
			delete flagValue[i];
			flagValue[i] = NULL;
		}
	}
	for (int i = 0; i < 32; i++) {
		if (regNames[i]) {
			delete regNames[i];
			regNames[i] = NULL;
		}
		if (regValues[i]) {
			delete regValues[i];
			regValues[i] = NULL;
		}
	}
	delete disText;
	delete regText;
	vcpu = NULL;
	computer = NULL;

}

bool Debug::eventManager(SDL_Event event) {
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
				if (evType == userEventType) {
					updateWindow();
					return true;
				}
				else {
					if (evType == 3) {
						if (winEvents[i].guiElement->getFocus()) {
							if (event.key.keysym.sym == 0x0D) {

								if (winEvents[i].guiElement == tempEdit) {
									std::string tmpTxt = tempEdit->getText();
									tempEdit->lostFocus();
									tempEdit->Visibled(false);
									delete tempEdit;
									tempEdit = NULL;
									
									if (winEvents[i].param1 == 1) {
										regValues[winEvents[i].param2]->Visibled(true);
										vcpu->SetDebugValue(winEvents[i].param1, winEvents[i].param2, tmpTxt);
									}
									if (winEvents[i].param1 == 2) {
										flagValue[winEvents[i].param2]->Visibled(true);
										vcpu->SetDebugValue(winEvents[i].param1, winEvents[i].param2, tmpTxt);
									}
									if (winEvents[i].param1 == 3) {
										printf("edit text: %s\n", tmpTxt.c_str());
										std::string tt = disasmText->getText();
										std::string rowTxt = getRow(tt, winEvents[i].param2);
										std::string addr = rowTxt.substr(0, rowTxt.find(" "));
										printf("addr: %sh\n", addr.c_str());
										vcpu->SetAssemblerValue(addr, tmpTxt);

									}
									if (winEvents[i].param1 == 4) {
										printf("edit text: %s\n", tmpTxt.c_str());
										uint8_t j = winEvents[i].param2 & 0xFF;
										uint8_t l = (winEvents[i].param2 >> 8);
										hexViewer->setMemoryForTextElement(l, j, tmpTxt);
									}

									deleteEvent(tempEditCursorEventNum);
									deleteEvent(tempEditKeyEventNom);

									updateData();
								}

							}
							else {
								winEvents[i].guiElement->OnKeyDown(event.key.keysym.sym, 0);
							}
							

							updateWindow();

							return true;
						}
					}
					else {
						int32_t x, y;
						SDL_GetMouseState(&x, &y);
						if (x >= winEvents[i].x && x <= winEvents[i].x1 && y >= winEvents[i].y && y <= winEvents[i].y1) {
							if (evType == 1) {
								if (winEvents[i].guiElement == hexViewer) {
									winEvents[i].param1 = x;
									winEvents[i].param2 = y;
								}
								winEvents[i].guiElement->OnClick(winEvents[i].param1, winEvents[i].param2);
							}
							if (evType == 2) {
								if (winEvents[i].guiElement == hexViewer) {
									winEvents[i].param1 = x;
									winEvents[i].param2 = y;
								}
								winEvents[i].guiElement->OnClickUp(winEvents[i].param1, winEvents[i].param2);
								if (winEvents[i].guiElement == btnTrace) {
									//uint16_t addr = vcpu->getPC();
									edtAddr->lostFocus();
									computer->start(1);
									//updateData();
								}
								if (winEvents[i].guiElement == btnStep) {
									edtAddr->lostFocus();
									computer->start(2);
									//updateData();
								}
								if (winEvents[i].guiElement == btnRunTo) {
									edtAddr->lostFocus();
									computer->stepTo(edtAddr->getText());
									updateData();
								}
								if (winEvents[i].guiElement == btnStop) {
									edtAddr->lostFocus();
									computer->stop();
									//updateData();
								}
								if (winEvents[i].guiElement == btnStart) {
									edtAddr->lostFocus();
									computer->start(0);
									updateData();
								}
								if (winEvents[i].guiElement == btnROM) {
									edtAddr->lostFocus();
									if (!hexViewer->isROMStatus()) {
										delete hexViewer;
										hexViewer = NULL;
										hexViewer = new tHexViewer(gContext, fContext, computer->getMemory(), true, winID);
										hexViewer->create(10, 20);
										hexViewer->Visibled(true);
										winEvents[numEventHexViewerClick].guiElement = hexViewer;
										winEvents[numEventHexViewerClickUp].guiElement = hexViewer;
										winEvents[numEventHexViewerScrol].guiElement = hexViewer;
										updateData();
									}
								}
								if (winEvents[i].guiElement == btnRAM) {
									edtAddr->lostFocus();
									if (hexViewer->isROMStatus()) {
										delete hexViewer;
										hexViewer = NULL;
										hexViewer = new tHexViewer(gContext, fContext, computer->getMemory(), false, winID);
										hexViewer->create(10, 20);
										hexViewer->Visibled(true);
										winEvents[numEventHexViewerClick].guiElement = hexViewer;
										winEvents[numEventHexViewerClickUp].guiElement = hexViewer;
										winEvents[numEventHexViewerScrol].guiElement = hexViewer;
										updateData();
									}
								}
							}
							if (evType == 4) {
								if (winEvents[i].guiElement == hexViewer) {
									winEvents[i].param1 = x;
									winEvents[i].param2 = y;
								}
								winEvents[i].guiElement->OnMove(winEvents[i].param1, winEvents[i].param2);
							}
							if (evType == 5) {
								//printf("Double click at (%d,%d)\n", x, y);
								if (winEvents[i].guiElement != disasmText && winEvents[i].guiElement != hexViewer) {
									winEvents[i].guiElement->Visibled(false);
									std::string tmpTxt = winEvents[i].guiElement->getText();
									int tmx, tmpy, tmpx1, tmpy1;
									winEvents[i].guiElement->getPositionAndSize(&tmx, &tmpy, &tmpx1, &tmpy1);

									tempEdit = new tEdit(gContext, fContext, winID, userEventType);
									tempEdit->create(tmx, tmpy, tmpx1 - tmx, tmpy1 - tmpy, 2, 1, tmpTxt);
									if (winEvents[i].param1==1) tempEdit->setMaxSymbols(4); else tempEdit->setMaxSymbols(1);
									tempEdit->setFocus();
									tempEdit->Visibled(true);
									tempEditKeyEventNom = addEvent(tempEdit, 3, 0, 0, 0, 0, winEvents[i].param1, winEvents[i].param2);
									tempEditCursorEventNum = addEvent(tempEdit, userEventType, 0, 0, 0, 0, 0, 0);
									tempEdit->OnClickUp(0, 0);
								}
								else {
									if (winEvents[i].guiElement == disasmText) {
										std::string tmpTxt = disasmText->getText();
										std::string rowTxt = getRow(tmpTxt, winEvents[i].param2);
										printf("Selectes row: %s\n",rowTxt.c_str());
										std::string strData = rowTxt.substr(6, rowTxt.length() - 6);
										printf("Selectes row: %s\n", strData.c_str());
										tempEdit = new tEdit(gContext, fContext, winID, userEventType);
										tempEdit->create(winEvents[i].x + 48, winEvents[i].y - 5, winEvents[i].x1 - (winEvents[i].x + 48), winEvents[i].y1 - winEvents[i].y + 5, 0, 1, strData);
										tempEdit->setMaxSymbols(15);
										tempEdit->setFocus();
										tempEdit->Visibled(true);
										tempEditKeyEventNom = addEvent(tempEdit, 3, 0, 0, 0, 0, winEvents[i].param1, winEvents[i].param2);
										tempEditCursorEventNum = addEvent(tempEdit, userEventType, 0, 0, 0, 0, 0, 0);
										tempEdit->OnClickUp(0, 0);
									}
									if (winEvents[i].guiElement == hexViewer) {
										uint8_t j = winEvents[i].param2 & 0xFF;
										uint8_t l = (winEvents[i].param2 >> 8);
										std::string tmpTxt = hexViewer->getTextElement(l,j);
										printf("Selectes data: %s\n", tmpTxt.c_str());
										tempEdit = new tEdit(gContext, fContext, winID, userEventType);
										tempEdit->create(winEvents[i].x, winEvents[i].y, winEvents[i].x1 - winEvents[i].x, winEvents[i].y1 - winEvents[i].y, 0, 1, tmpTxt);
										tempEdit->setMaxSymbols(2);
										tempEdit->setFocus();
										tempEdit->Visibled(true);
										tempEditKeyEventNom = addEvent(tempEdit, 3, 0, 0, 0, 0, winEvents[i].param1, winEvents[i].param2);
										tempEditCursorEventNum = addEvent(tempEdit, userEventType, 0, 0, 0, 0, 0, 0);
										tempEdit->OnClickUp(0, 0);
									}
								}
							}

							updateWindow();

							return true;
						}
					}
				}
			}
		}
		else break;
	}
	//edtAddr->lostFocus();
	updateWindow();
	return false;
}

void Debug::updateData() {
	Registers nregs = vcpu->getRegisters();
	for (int i = 0; i < nregs.count; i++) {
		if (regs.registers[i].data == nregs.registers[i].data) {
			regValues[i]->changeColor(cBLACK);
			regValues[i]->changecaption(nregs.registers[i].strData);
		}
		else {
			regValues[i]->changeColor(cLIGHTRED);
			regValues[i]->changecaption(nregs.registers[i].strData);
		}
	}
	for (int i = 0; i < nregs.flags.count; i++) {
		if (regs.flags.flags[i].data == nregs.flags.flags[i].data) {
			flagValue[i]->changeColor(cBLACK);
			flagValue[i]->changecaption(nregs.flags.flags[i].strData);
		}
		else {
			flagValue[i]->changeColor(cLIGHTRED);
			flagValue[i]->changecaption(nregs.flags.flags[i].strData);
		}
	}
	regs = nregs;

	uint16_t addr = vcpu->getPC();
	disasmText->changecaption(vcpu->disAsm(&addr, 16));
	hexViewer->updateData();
}

void Debug::updateDataNeeded() {
	updateData();
	updateWindow();
}