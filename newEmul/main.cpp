#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#include <SDL.h>
#include <functional>
#include "vector06c.h"
#include "graph.h"
#include "fonts.h"
#include "gui.h"
#include "menu.h"
#include "doevents.h"
#include "debug.h"
#include "openDialog.h"
#include "wav.h"
#include "vkeyboard.h"
#include "vkeyb.h"
#include "config.h"
#include "breakpoints.h"

//long long clockCount = 0;

//void t3Mhz() {
//	clockCount++;
//}

//void newCPU(CPU *cp) {
//	cp->reset();
//}

#define MAXWINDOWS	5

bool exitProgramm = false;
SDL_Window* subWindow;
SDL_Renderer* subRenderer;
Debug* dbgWindow = NULL;
OpenDialog* openDlg = NULL;
VKEYBOARD* virtualKeyboard = NULL;
Window *wins[MAXWINDOWS];
Machine *globalMachine;
Uint32 myEventType = 0;
uint32_t mutex = 0;
SDL_Surface* surf = NULL;
WAV* wav;
Breakpoints* breakPoints = NULL;

void updateMainWindow(SDL_Renderer* renderer, SDL_Surface* surface) {
	while(mutex) {}
	mutex++;
	
	uint32_t* ss = (uint32_t*)surf->pixels;
	uint32_t* ds = (uint32_t*)surface->pixels;
	for (int i = 0; i < 100000; i++) if (*(ss + i)) *(ds + i) = *(ss + i);
	
	SDL_Texture* tex = NULL;
	tex = SDL_CreateTextureFromSurface(renderer, surface);
	if (tex) {
		if ((SDL_RenderCopy(renderer, tex, NULL, NULL)) < 0) {
			printf("Error render copy, %p\n", tex);
			printf("%s\n", SDL_GetError());
			exit(-1);
		}
		SDL_DestroyTexture(tex);
		tex = NULL;

		SDL_RenderPresent(renderer);
	}
	mutex--;
}

void openRom() {
	printf("Selected OpenROM\n");

	uint32_t openWindowWidth = 620;
	uint32_t openWindowHeight = 500;
	openDlg = new OpenDialog("Open ROM", 200, 200, openWindowWidth, openWindowHeight, myEventType, "*.rom");
	wins[2] = openDlg;
}

void openWav() {
	printf("Selected openWav\n");
	uint32_t openWindowWidth = 620;
	uint32_t openWindowHeight = 500;
	openDlg = new OpenDialog("Open WAV", 200, 200, openWindowWidth, openWindowHeight, myEventType, "*.wav");
	wins[2] = openDlg;
}

void exitRun() {
	printf("Selected exitRun\n");
	exitProgramm = true;
}

void debugger() {
	printf("Selected debugger\n");
	uint32_t debudWindowWidth = 1024;
	uint32_t debudWindowHeight = 768;
	dbgWindow = new Debug(globalMachine, "Debug", 200, 200, debudWindowWidth, debudWindowHeight, myEventType, breakPoints);
	wins[1] = dbgWindow;
}

void vkeyboard() {
	printf("Selected virtual keyboard\n");
	vKeybParams kp = globalMachine->getVirtualKeyboardParams();
	virtualKeyboard = new VKEYBOARD(globalMachine, "Virtual keyboard", 200, 200, kp.winWidth, kp.winHeigth, myEventType, kp);
	wins[3] = virtualKeyboard;
}

void options() {
	printf("Selected options\n");
}

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

int main(int argc, char* args[]) {
	
	printf("Start\n");
	for (int i = 0; i < MAXWINDOWS; i++) wins[i] = NULL;
	
	config conf("emul.cnf");
	CONFIG cfg = conf.getConfig();
	
	exitProgramm = false;
	initEvents();

	SDL_Event event;
	SDL_Renderer* renderer;
	SDL_Window* window;
	int i;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);
	
	window = SDL_CreateWindow(cfg.profiles[cfg.numProfile].name.c_str(), 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	SDL_RenderClear(renderer);

	myEventType = SDL_RegisterEvents(1);
	printf("new Event type=%d\n", myEventType);

	SDL_Texture* tex = NULL;

	SDL_Rect destr;
	destr.x = 0;
	destr.y = 0;
	surf = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0, 0, 0, 0);

	Graph* gMain = new Graph((void*)surf->pixels, surf->w, surf->h, surf->format->BitsPerPixel);
	Font::fontinit();
	Font::registerNewFont((char*)"sserifer.fon");
	Font* fMain = new Font(gMain);
	
	menu *mainMenu = new menu(gMain, fMain);
	mainMenu->create(0, 0, 639, 25);
	mainMenu->addMainMenuItem("File");
	mainMenu->addSubMenuItem(0, "Open ROM...", openRom);
	mainMenu->addSubMenuItem(0, "Open WAVE...", openWav);
	mainMenu->addSubMenuItem(0, "Exit", exitRun);
	mainMenu->addMainMenuItem("Debug");
	mainMenu->addSubMenuItem(1, "Debugger", debugger);
	mainMenu->addSubMenuItem(1, "Keyboard", vkeyboard);
	mainMenu->addMainMenuItem("Settings");
	mainMenu->addSubMenuItem(2, "Options", options);
	mainMenu->Visibled(true);
	
	updateMainWindow(renderer, surf);

	breakPoints = new Breakpoints();

	if (cfg.profiles[cfg.numProfile].profileName == "vector06c") {
		wav = new WAV(3000000);

		Vector06c* vector;
		vector = new Vector06c(renderer, updateMainWindow, wav, cfg.profiles[cfg.numProfile], breakPoints);
		globalMachine = vector;
		if (!cfg.debugMode) globalMachine->start(0);
	}
	
	bool updated = false;
	bool eventRes = false;
	while (1) {
		if (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_KEYDOWN) {
				printf("Pressed key %02X, at window: %d, code: %02X\n", event.key.keysym.sym, event.window.windowID, event.key.keysym.scancode);
				if (event.window.windowID == 1) {
					globalMachine->keyDown(event.key.keysym.scancode);
				}
			}
			if (event.type == SDL_KEYUP) {
				printf("Unpressed key %02X, at window: %d, code: %02X\n", event.key.keysym.sym, event.window.windowID, event.key.keysym.scancode);
				if (event.window.windowID == 1) {
					globalMachine->keyUp(event.key.keysym.scancode);
				}
			}
			if (event.window.windowID == 1) {
				if (event.type == SDL_QUIT)	break;
				if (event.type == SDL_MOUSEMOTION) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					eventRes = checkDoEvent(MOUSEMOVE, x, y);
					//printf("(%d,%d) ", x, y);
				}
				if (event.type == SDL_MOUSEBUTTONDOWN) {
					if (event.button.button == SDL_BUTTON_LEFT) {
						int x, y;
						SDL_GetMouseState(&x, &y);
						//printf("Left button down on (%d,%d) ", x, y);
						eventRes = checkDoEvent(MOUSELEFTBUTTONDOWN, x, y);
					}
				}
				if (event.type == SDL_MOUSEBUTTONUP) {
					if (event.button.button == SDL_BUTTON_LEFT) {
						int x, y;
						SDL_GetMouseState(&x, &y);
						//printf("Left button down on (%d,%d) ", x, y);
						eventRes = checkDoEvent(MOUSELEFTBUTTONUP, x, y);
					}
				}
			}
			else {
				if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP || event.button.clicks == 2 || event.type == myEventType || event.type == SDL_KEYDOWN || event.type == SDL_MOUSEMOTION) {
					for (int i = 1; i < MAXWINDOWS; i++) {
						if (wins[i]) {
							if (event.window.windowID == wins[i]->getWindowID()) {
								eventRes = wins[i]->eventManager(event);
								if (eventRes) {
									if (i == 2) {
										uint8_t res = openDlg->getresultCode();
										if (res == 2) {
											delete wins[i];
											wins[i] = NULL;
										}
										if (res == 1) {
											string fileName = openDlg->getResult();
											delete wins[i];
											wins[i] = NULL;

											string ext = fileName.substr(fileName.length() - 3, 3);
											printf("Extension=%s\n", ext.c_str());
											if (ext == "wav") {

												printf("Open file: %s\n", fileName.c_str());
												wav->loadDataFromWave(fileName);
											}
											if (ext == "rom" || ext == "r0m") {
												bool addr0 = false;
												if (ext == "r0m") addr0 = true;
												globalMachine->loadProgramToMemory(fileName, addr0);
											}
											if (ext == "bas" || ext == "cas" || ext == "BAS" || ext == "CAS") {
												printf("Opening BASIC file\n");
												globalMachine->loadDataToMemory(fileName, 1);
											}

										}
									}
								}
								eventRes = false;
								break;
							}
						}
					}
				}
			}
			if (event.type == SDL_WINDOWEVENT) {
				if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
					printf("Window %d closed\n", event.window.windowID);
					if (event.window.windowID != 1) {
						for (int i = 1; i < MAXWINDOWS; i++) {
							if (wins[i] != NULL) {
								if (event.window.windowID == wins[i]->getWindowID()) {
									delete wins[i];
									wins[i] = NULL;
									break;
								}
							}
							//SDL_DestroyWindow(SDL_GetWindowFromID(event.window.windowID));
						}
					}
					else break;
				}
			}
		}
		if (wins[1]) {
			if (globalMachine->udateNeeded()) {
					dbgWindow->updateDataNeeded();
					globalMachine->updated();
			}
		}
		if (eventRes) {
			eventRes = false;
			
			if (!globalMachine->getStatus()) updateMainWindow(renderer, surf);
		}
		if (exitProgramm) break;
	}

	delete globalMachine;
	delete wav;
	delete breakPoints;

	printf("\n");

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	printf("End prog\n");

	return EXIT_SUCCESS;
}