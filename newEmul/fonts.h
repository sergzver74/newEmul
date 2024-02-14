#ifndef __FONTS_H__
#define __FONTS_H__

#include "graph.h"

#define alHLEFT         0
#define alHCENTER       1
#define alHRIGHT        2
#define alVTOP          0
#define alVCENTER       1
#define alVBOTTOM       2

class Font {
private:

    unsigned short FontID;                  ///< Номер текущего шрифта

    int minx;                        ///< Минимальная координата области вывода изображения по оси X. Точки изображения меньше ее выводиться не будут.
    int miny;                        ///< Минимальная координата области вывода изображения по оси Y. Точки изображения меньше ее выводиться не будут.
    int maxx;                        ///< Максимальная координата области вывода изображения по оси X. Точки изображения больше ее выводиться не будут.
    int maxy;                        ///< Максимальная координата области вывода изображения по оси Y. Точки изображения больше ее выводиться не будут.
    int t_scale;                            ///< Коэффициент масштабирования
    int xt;                                 ///< Координата X начала вывода текста (левый верхний угол)
    int yt;                                 ///< Координата Y начала вывода текста (левый верхний угол)
    char t_halign;                          ///< Горизонтальное выравнивание
    char t_valign;                          ///< Вертикальное выравнивание
    unsigned int t_color;                 ///< цвет шрифта
    char isPhisical;

    Graph* grContext;

    static void LoadResourceFromNE(char* name);
    static void LoadResourceFromFNT(char* name);
    static void FontDir(char* name, int offs);
    void SetFontID(unsigned short id);
    void print1(unsigned char c);
    void print(unsigned char c);
    int getCount(char* c);

public:
    
    Font(Graph* gr);
	~Font();
	
	int length(char* c);
	void setPhisicalScreen();
	void restoreVirtualScreen();
	void printxy(int x, int y, char* text);
	int getLenTextInPixels(char* c, int size);
	void setFontParam(int fid, unsigned int col, int scale, char halign, char valign);
	int setFont(char* name, int sz, unsigned int col, char halign, char valign);
    int getFontHeight(int id, int size);
    static bool fontRegistred(char* name, int sz);
    static unsigned char registerNewFont(char* name);
    static void fontinit();
    static void closefont();

};

#endif
