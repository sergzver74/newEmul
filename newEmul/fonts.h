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

    unsigned short FontID;                  ///< ����� �������� ������

    int minx;                        ///< ����������� ���������� ������� ������ ����������� �� ��� X. ����� ����������� ������ �� ���������� �� �����.
    int miny;                        ///< ����������� ���������� ������� ������ ����������� �� ��� Y. ����� ����������� ������ �� ���������� �� �����.
    int maxx;                        ///< ������������ ���������� ������� ������ ����������� �� ��� X. ����� ����������� ������ �� ���������� �� �����.
    int maxy;                        ///< ������������ ���������� ������� ������ ����������� �� ��� Y. ����� ����������� ������ �� ���������� �� �����.
    int t_scale;                            ///< ����������� ���������������
    int xt;                                 ///< ���������� X ������ ������ ������ (����� ������� ����)
    int yt;                                 ///< ���������� Y ������ ������ ������ (����� ������� ����)
    char t_halign;                          ///< �������������� ������������
    char t_valign;                          ///< ������������ ������������
    unsigned int t_color;                 ///< ���� ������
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
