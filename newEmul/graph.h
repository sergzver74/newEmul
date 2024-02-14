/**
*  @headerfile	graph.h
*/

#include <stdint.h>

#ifndef __GRAPH_H__
#define __GRAPH_H__

// colors
#define cBLACK          Graph::RGBtoColor(0x00,0x00,0x00)          ///< черный цвет
#define cBLUE	        Graph::RGBtoColor(0x00,0x00,0x7F)          ///< синий цвет
#define cLIGHTBLUE      Graph::RGBtoColor(0x00,0x00,0xFF)          ///< светло-синий цвет
#define cGREEN          Graph::RGBtoColor(0x00,0x7F,0x00)          ///< зеленый цвет
#define cLIGHTGREEN     Graph::RGBtoColor(0x00,0xFF,0x00)          ///< светло-зеленый цвет
#define cRED            Graph::RGBtoColor(0x7F,0x00,0x00)          ///< красный цвет
#define cLIGHTRED       Graph::RGBtoColor(0xFF,0x00,0x00)          ///< светло-красный цвет
#define cDARKGRAY       Graph::RGBtoColor(0x18,0x1C,0x18)          ///< серый цвет
#define cGRAY           Graph::RGBtoColor(0x1C,0x3C,0x38)          ///< темно-серый цвет
#define cLIGHTGRAY      Graph::RGBtoColor(0x78,0x7C,0x78)          ///< светло-серый цвет
#define cWHITE          Graph::RGBtoColor(0xFF,0xFF,0xFF)          ///< белый цвет
#define cCYAN           Graph::RGBtoColor(0x00,0xFF,0xFF)          ///< бирюзовый цвет
#define cYELLOW         Graph::RGBtoColor(0xFF,0xFF,0x00)          ///< желтый цвет
#define cLIGHTMAGENTA   Graph::RGBtoColor(0xFF,0x00,0xFF)          ///< светло-фиолетовый цвет
#define cMAGENTA        Graph::RGBtoColor(0x7F,0x00,0x7F)          ///< фиолетовый цвет
#define cBROWN          Graph::RGBtoColor(0x7F,0x7F,0x00)          ///< коричневый цвет
#define cORANGE         Graph::RGBtoColor(0xFF,0x7F,0x00)          ///< оранжевый цвет
#define cWINGRAY        Graph::RGBtoColor(0x80,0x80,0x80)          ///< темно-серый цвет
#define cWINLIGHTGRAY   Graph::RGBtoColor(0xD4,0xD0,0xC8)          ///< светло-серый цвет
#define cWINDARKGRAY    Graph::RGBtoColor(0x40,0x40,0x40)          ///< серый цвет

// orientation
#define oVERTICAL       0           ///< вертикальная ориентация
#define oHORIZONTAL     1           ///< горизонтальная ориентация

#define arcRIGHTDOWN    0           ///< скругление правого нижнего угла
#define arcRIGHTUP      1           ///< скругление правого верхнего угла
#define arcLEFTDOWN     2           ///< скругление левого нижнего угла
#define arcLEFTUP       3           ///< скругление левого верхнего угла

/*

typedef void (*PP)( int, int, unsigned int );
typedef unsigned int (*GP)(int x, int y);
typedef void (*CLSCR)();
typedef unsigned int (*RGBTOCOLOR)(unsigned int r, unsigned int g, unsigned int b);
typedef void (*COLORTORGB)(unsigned int col, unsigned int* r, unsigned int* g, unsigned int* b);
typedef void (*GBAR)(int x1, int y1, int x2, int y2);
typedef void (*GBARGR)(int x1, int y1, int x2, int y2, int orient, unsigned int col1, unsigned int col2);
typedef void (*FGRADIENT)(int xn,int yn, int x, int y, int x1, int y1, int orient, unsigned int col1, unsigned int col2);
typedef unsigned int* (*GIMG)(int x1,int y1,int x2,int y2);
typedef void (*PIMG)(int x1, int y1, unsigned int* imb);
*/

typedef struct screeninfo {
	uint16_t xres;
	uint16_t yres;
	uint8_t bits_per_pixel;
};

/**
@struct tFBSave
@brief Структура параметров графической системы.
*/
struct tFBSave {
	unsigned short* fbSave;     ///< указатель на реальный фреймбуффер
	int xres;                   ///< разрешение по X
	int yres;                   ///< разрешение по Y
	int maxx;                   ///< максимальная координата X
	int maxy;                   ///< максимальная координата Y
	int minx;                   ///< мнимальная координата X
	int miny;                   ///< мнимальная координата Y
	char saved;                 ///< флаг присутствия сохраненной конфигурации
};

class Graph {
private:
	screeninfo vinfo; ///< структура для работы с фреймбуфером
	int minx;                       ///< Минимальная координата области вывода изображения по оси X. Точки изображения меньше ее выводиться не будут.
	int miny;                       ///< Минимальная координата области вывода изображения по оси Y. Точки изображения меньше ее выводиться не будут.
	int maxx;                       ///< Максимальная координата области вывода изображения по оси X. Точки изображения больше ее выводиться не будут.
	int maxy;                       ///< Максимальная координата области вывода изображения по оси Y. Точки изображения больше ее выводиться не будут.
	int fbfd0;                      ///< дискриптор файла фреймбуфера 0
	int fbfd2;                      ///< дискриптор файла фреймбуфера 2
	int fbsize;                     ///< Размер фреймбуфера
	unsigned int cur_color;         ///< текущий цвет рисования
	unsigned int fill_color;        ///< текущий цвет заливки
	unsigned short* fbbuf;          ///< Указатель на текущий фреймбуфер
	unsigned int* fbbuf1;           ///< Указатель на 1 фреймбуфер
	char alpha;                     ///< текущий уровень прозрачности
	int pms;                        ///< текущий режим вывода сохраненной области
	unsigned int pic;             ///< текущий цвет маски
	bool grinit;                    ///< флаг инициализации фреймбуффера
	bool maxis;                     ///< флаг переноса системы координат
	int movex;                      ///< Смещение по оси X
	int movey;                      ///< Смещение по оси Y
	tFBSave fbSave;                 ///< сохраненные параметры графического буфера

	//unsigned int convertColor32to16(unsigned int c);

public:
	Graph(void* fbPointer, uint16_t width, uint16_t heigth, uint8_t bps);
	Graph() {};
	~Graph();
	void moveAxis(int x, int y);
	int getmaxx();
	int getmaxy();
	static unsigned int RGBtoColor(unsigned int r, unsigned int g, unsigned int b);
	static void ColortoRGB(unsigned int col, unsigned int* r, unsigned int* g, unsigned int* b);
	void SetColor(unsigned int col);
	void SetFillColor(unsigned int col);
	unsigned int GetColor();
	void putpixel(int x, int y, unsigned int col);
	unsigned int getpixel(int x, int y);
	void line(int x1, int y1, int x2, int y2);
	void rectangle(int x1, int y1, int x2, int y2);
	void bar(int x1, int y1, int x2, int y2);
	void Circle(int x0, int y0, int radius);
	void arc(int x0, int y0, int radius, int part);
	void bargradient(int x1, int y1, int x2, int y2, int orient, unsigned int col1, unsigned int col2);
	void fillgradient(int xn, int yn, int x, int y, int x1, int y1, int orient, unsigned int col1, unsigned int col2);
	void roundrect(int x, int y, int x1, int y1, int radius);
	void roundrectgradient(int x, int y, int x1, int y1, int radius, int orient, unsigned int col1, unsigned int col2);
	void circlegradient(int x, int y, int radius, int orient, unsigned int col1, unsigned int col2);
	void setAlpha(char a);
	void setPutImageStyle(int type, unsigned int col);
	unsigned int* getImage(int x1, int y1, int x2, int y2);
	void putImage(int x1, int y1, unsigned int* imb);
	void freeImage(unsigned int* imb);
	//int initgr(void* fbPointer, uint16_t width, uint16_t heigth, uint8_t bps);
	void setOutputContext(int x, int y, int x1, int y1);
	void clearScreen();
	void restoreDefaultContext();
	void getOutputContext(int* x, int* y, int* mx, int* my);
	void setVirtualScreen(unsigned short* scr, int width, int height);
	void restoreOriginalScreen();
	//void closegr();
};

#endif
