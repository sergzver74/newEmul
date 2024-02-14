#ifndef __MENU_H__
#define __MENU_H__

#include <stdint.h>
#include <string>
#include <functional>
#include "graph.h"
#include "gui.h"
#include "fonts.h"
#include "doevents.h"


class menu : GUI {
public:
	menu();
	~menu();
	menu(Graph* gc, Font* fc);
	void create(uint16_t x, int16_t y, int16_t width, int16_t heigth);
	void addMainMenuItem(std::string name);
	void addSubMenuItem(uint8_t mainMenuItemID, std::string name, std::function<void()> callback);
	void Visibled(bool vis);
	void OnClick(uint32_t param1, uint32_t param2);
	void OnClickUp(uint32_t param1, uint32_t param2);

	std::string getText();
	void getPositionAndSize(int* x, int* y, int* x1, int* y1);
	void Destroy();
	void Enabled(bool en);
	bool getFocus();
	void OnKeyDown(uint32_t param1, uint32_t param2);
	void OnMove(uint32_t param1, uint32_t param2);
	void OnEndMove(uint32_t param1, uint32_t param2);

private:
	int16_t mx, my, mx1, my1;
	std::string mainMenuItems[16];
	std::string subMenuItems[16][16];
	std::function<void()> callbacks[16][16];
	uint8_t mainItemsCount;
	uint8_t curOpenMainMenu;
	uint8_t curOpenSubMenu;
	uint8_t subItemsCount[16];
	bool visible;
	bool mainMenuOpen;
	unsigned int* backImage;
	unsigned int* backSelectImage;
	unsigned int* backSubImage;
	tLabel* mainItems[16];
	tLabel* subItems[16];
	uint16_t mainButtonDownEvent[16];
	uint16_t mainMouseMoveEvent[16];
	uint16_t subButtonUpEvent[16];
	uint16_t subMouseMoveEvent[16];
	Graph* grContext;
	Font* fontContext;
};


#endif
