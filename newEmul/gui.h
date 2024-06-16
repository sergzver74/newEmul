#ifndef __GUI_H__
#define __GUI_H__

#include <stdint.h>
#include <string>
#include <thread>
#include <windows.h>
#include <SDL.h>
#include "graph.h"
#include "fonts.h"
#include "memory.h"
#include "breakpoints.h"
#include "machine.h"


#define bCLOSEBUTTON		0   ///< вид кнопки закрыть
#define bROUNDBUTTON1       1   ///< первый вид кнопки
#define bROUNDBUTTON2       2   ///< второй вид кнопки

#define tNORMAL             0   ///< обычный шрифт
#define tBOLD               1   ///< жирный шрифт
#define tSHADOW             2   ///< шрифт с тенью
#define tBOLDSHADOW         3   ///< жирный шрифт с тенью

#define tBkRect             0   ///< пр€моугольна€ кнопка
#define tBkRoundRect        1   ///< пр€моугольна€ кнопка со скругленными углами
#define tBkCircle           2   ///< кругла€ кнопка

#define tButtonUp           1
#define tButtonDown         2

class GUI
{
public:
	GUI() {};
	~GUI() {};
	//virtual void create(uint16_t x, uint16_t y, uint16_t width, uint16_t height) = 0;
	virtual void Destroy() = 0;
	virtual void Visibled(bool vis) = 0;
	virtual void Enabled(bool en) = 0;
    virtual void OnClick(uint32_t param1, uint32_t param2) = 0;
    virtual void OnClickUp(uint32_t param1, uint32_t param2) = 0;
    virtual void OnMove(uint32_t param1, uint32_t param2) = 0;
    virtual void OnEndMove(uint32_t param1, uint32_t param2) = 0;
    virtual void OnKeyDown(uint32_t param1, uint32_t param2) = 0;
    virtual bool getFocus() = 0;
    virtual std::string getText() = 0;
    virtual void getPositionAndSize(int* x, int* y, int* x1, int* y1) = 0;
};


class tLabel : public GUI {
private:
    int lx;                     ///< координата X начала области вывода
    int ly;                     ///< координата Y начала области вывода
    int lx1;                    ///< координата X окончани€ области вывода
    int ly1;                    ///< координата Y окончани€ области вывода
    int tp;                     ///< тип вывода текста определ€ющий жирное начертание и тень
    int fid;                    ///< номер используемого шрифта
    int sz;                     ///< масштабирование шрифта
    char lhl;                   ///< выравнивание по горизонтали
    char lvl;                    ///< выравнивание по вертикали
    char bktp;                  ///< тип области на которой будет выводитс€ текст
    unsigned int shcl;        ///< цвет тени
    unsigned int lcl;         ///< цвет текста
    unsigned int* lbl;        ///< сохраненна€ область экрана под текстом
    unsigned int bkcolor;     ///< цвет подложки
    bool bkexists;              ///< наличие подложки
    void outText();
    std::string txt;
    bool visible;
    bool enable;
    Graph* grContext;
    Font* fontContext;
public:
    //tLabel() {};
    ~tLabel();
    tLabel(Graph* gc, Font* fc);
    void create(int x, int y, int dx, int dy, unsigned int col, std::string text);
    void setparam(int fontid, int size, int type, char halign, char valign);
    void setShadowColor(unsigned int col);
    void setColor(unsigned int col);
    void setBkColor(unsigned int color);
    void setBkType(char type);
    void changecaption(std::string text);
    void changeColor(uint32_t col);
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    uint32_t getColor();
    std::string getText();
    uint16_t GetX();
    uint16_t GetX1();
    uint16_t GetY();
    uint16_t GetY1();
    void Destroy();
    void Visibled(bool vis);
    void Enabled(bool en);
    bool getFocus();
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    static int getLengthTextInPixels(Font* fc, int fontid, int size, std::string text);
};

class tButton : public GUI {
private:
    
    int bx;                     ///< координата X начала области вывода
    int by;                     ///< координата Y начала области вывода
    int bx1;                    ///< координата X окончани€ области вывода
    int by1;                    ///< координата Y окончани€ области вывода
    int dx;
    int dy;
    std::string txt;
    uint8_t btp;
    bool visible;
    bool enable;
    unsigned int* lbl;
    tLabel* caption;

    Graph* grContext;
    Font* fontContext;

public:
    //tButton() {};
    ~tButton() {};
    tButton(Graph* gc, Font* fc);
    void create(int x, int y, int dx, int dy, std::string text);
    void setSpecialButtonType(uint8_t tp);
    void changePosition(int x, int y);
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    void Visibled(bool vis);
    void Enabled(bool en);
    bool getFocus();
    std::string getText();
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    void Destroy();
};

class tCursor {
public:
    tCursor(Graph* gc, Font* fc, uint32_t winId, uint32_t evType);
    ~tCursor();
    void create(int x, int y, int fid, int sz);
    void Visibled(bool vis);
    void changePosition(int x, int y);

private:
    int cx, cy;
    int height;
    bool visible;
    int fontid;
    int size;
    uint32_t wid;
    uint32_t curEvent;
    unsigned int* lbl;

    Graph* grContext;
    Font* fontContext;

    void sendCursorEvent();
};

class tEdit : public GUI {
private:

    int ex;                     ///< координата X начала области вывода
    int ey;                     ///< координата Y начала области вывода
    int ex1;                    ///< координата X окончани€ области вывода
    int ey1;                    ///< координата Y окончани€ области вывода
    int dx;
    int dy;
    int fontid;
    int size;
    std::string txt;
    bool visible;
    bool enable;
    bool focus;
    unsigned int* lbl;
    tCursor* cursor;
    tLabel* editText;
    uint32_t cursorPosition;
    uint16_t maxSymbols;

    Graph* grContext;
    Font* fontContext;

public:
    //tButton() {};
    ~tEdit() {};
    tEdit(Graph* gc, Font* fc, uint32_t winId, uint32_t evType);
    void create(int x, int y, int dx, int dy, int fid, int sz, std::string text);
    void Visibled(bool vis);
    void Enabled(bool en);
    void setText(std::string txt);
    std::string getText();
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    void setMaxSymbols(uint16_t n);
    void lostFocus();
    void setFocus();
    bool getFocus();
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    void Destroy();

};

class tHexViewer : public GUI {
private:
    uint16_t hx;
    uint16_t hy;
    uint16_t hx1;
    uint16_t hy1;
    uint16_t dx;
    uint16_t dy;
    unsigned int* lbl;
    bool visible;
    bool enable;
    uint32_t win;
    uint32_t startMemAddr;
    uint32_t maxMemAddr;
    uint32_t curMemAddr;
    //bool isROM;
    uint8_t memType;
    uint8_t* memPointer;
    float scrollPosition;
    float scrollAdd;
    int16_t ln;

    tLabel* addrs[16];
    tLabel* datas[16][16];
    tLabel* stringDatas[16];

    tButton* bUp;
    tButton* bDown;
    tButton* bMiddle;

    Graph* grContext;
    Font* fontContext;
    //Memory* memory;
    Machine* computer;

    uint16_t mx;
    uint16_t my;
    bool scrollPressed;

    void update();

public:
    //tHexViewer(Graph* gc, Font* fc, Memory* mem, bool rom, uint32_t winId);
    tHexViewer(Graph* gc, Font* fc, Machine* mach, uint8_t mType, uint32_t winId);
    ~tHexViewer();
    void create(int x, int y, uint32_t cp, uint32_t sp);
    uint16_t getLebgth();
    void Visibled(bool vis);
    void updateData();
    void Enabled(bool en);
    //bool isROMStatus();
    uint8_t isMemoryType();
    uint32_t getCurrentPosition();
    uint32_t getScrollPosition();
    bool getFocus();
    std::string getTextElement(int i, int j);
    void setMemoryForTextElement(int i, int j, std::string sData);
    std::string getText();
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    void Destroy();
};

class tListBox : public GUI {
private:
    int ex;                     ///< координата X начала области вывода
    int ey;                     ///< координата Y начала области вывода
    int ex1;                    ///< координата X окончани€ области вывода
    int ey1;                    ///< координата Y окончани€ области вывода
    int dx;
    int dy;
    int fontid;
    int size;
    std::string* txt;
    bool visible;
    bool enable;
    bool focus;
    unsigned int* lbl;
    uint16_t index;
    tLabel* lst[16];
    uint8_t selected;

    uint8_t getCount();

    Graph* grContext;
    Font* fontContext;

public:
    ~tListBox();
    tListBox(Graph* gc, Font* fc, uint32_t evType);
    void create(int x, int y, int dx, int dy, int fid, int sz, std::string* text);
    void Visibled(bool vis);
    void Enabled(bool en);

    bool getFocus();
    std::string getText();
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    void Destroy();
};

class tComboBox : public GUI {
private:
    int ex;                     ///< координата X начала области вывода
    int ey;                     ///< координата Y начала области вывода
    int ex1;                    ///< координата X окончани€ области вывода
    int ey1;                    ///< координата Y окончани€ области вывода
    int dx;
    int dy;
    int fontid;
    int size;
    std::string* txt;
    bool visible;
    bool enable;
    bool focus;
    unsigned int* lbl;
    uint16_t index;
    tLabel *selectText;
    tButton* bDown;
    tListBox* list;
    bool open;

    Graph* grContext;
    Font* fontContext;

public:
    ~tComboBox();
    tComboBox(Graph* gc, Font* fc, uint32_t evType);
    void create(int x, int y, int dx, int dy, int fid, int sz, std::string* text, uint16_t indx);
    void Visibled(bool vis);
    void Enabled(bool en);

    uint8_t getCount();
    bool isOpen();
    bool getFocus();
    std::string getText();
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    void Destroy();
};

class tFileList : public GUI {
private:
    uint16_t hx;
    uint16_t hy;
    uint16_t hx1;
    uint16_t hy1;
    uint16_t dx;
    uint16_t dy;
    unsigned int* lbl;
    bool visible;
    bool enable;
    uint32_t win;
    //uint32_t startMemAddr;
    //uint32_t maxMemAddr;
    //uint16_t curMemAddr;
    //uint8_t* memPointer;
    float scrollPosition;
    float scrollAdd;
    int16_t ln;
    uint8_t curSelect;
    uint16_t startSelect;

    tLabel* names[16];
    tLabel* sizeAneDir[16];
    tLabel* date[16];

    tButton* bName;
    tButton* bSize;
    tButton* bDate;

    tButton* bUp;
    tButton* bDown;
    tButton* bMiddle;

    Graph* grContext;
    Font* fontContext;
    WIN32_FIND_DATAA* files;
    uint32_t count;
    uint32_t selectedItem;

    uint16_t mx;
    uint16_t my;
    bool scrollPressed;

    void update();

public:
    tFileList(Graph* gc, Font* fc, WIN32_FIND_DATAA* fls, uint32_t n, uint32_t winId);
    ~tFileList();
    void create(int x, int y);
    void Visibled(bool vis);
    void updateData();
    void Enabled(bool en);
    bool getFocus();
    std::string getText();
    std::string getSelectedFileName();
    std::string getSelectedFileSize();
    uint16_t getSelectedIndex();
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnDblClick(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    void Destroy();
};

class tBreakPointSet : public GUI {
private:
    uint16_t hx;
    uint16_t hy;
    uint16_t hx1;
    uint16_t hy1;
    uint16_t dx;
    uint16_t dy;
    int fontid;
    int size;
    uint8_t count;
    unsigned int* lbl;
    bool visible;
    bool enable;

    Graph* grContext;
    Font* fontContext;
    Breakpoints* b;
    std::string asmTxt;

    tLabel* bp[20];

    std::string getString(uint8_t n);
    uint16_t getAddrFromASMString(uint8_t n);
    void checkExistsBreakpoints();

public:
    tBreakPointSet(Graph* gc, Font* fc, uint32_t n, Breakpoints* bp);
    ~tBreakPointSet();
    void create(int x, int y, int dx, int dy, int fid, int sz, std::string s);
    void Visibled(bool vis);
    void Enabled(bool en);
    void getPositionAndSize(int* x, int* y, int* x1, int* y1);
    void OnKeyDown(uint32_t param1, uint32_t param2);
    void OnDblClick(uint32_t param1, uint32_t param2);
    void OnClick(uint32_t param1, uint32_t param2);
    void OnClickUp(uint32_t param1, uint32_t param2);
    void OnMove(uint32_t param1, uint32_t param2);
    void OnEndMove(uint32_t param1, uint32_t param2);
    bool getFocus();
    std::string getText();
    
    void changeASMList(std::string txt);

    void Destroy();
};

#endif // !__GUI_H__
