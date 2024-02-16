#include "gui.h"
#include "hex.h"

void tLabel::outText() {
    if (bkexists)
    {
        switch (bktp)
        {
        case tBkRect:
            grContext->SetFillColor(bkcolor);
            grContext->bar(lx,ly, lx1, ly1);
            break;
        case tBkRoundRect:
            grContext->SetColor(bkcolor);
            grContext->roundrectgradient(lx, ly, lx1, ly1, 10, oHORIZONTAL, bkcolor, bkcolor);
            break;
        case tBkCircle:

            break;
        default:
            break;
        }
    }
    grContext->setOutputContext(lx - 1, ly - 1, lx1, ly1);
    if (tp == tSHADOW)
    {
        fontContext->setFontParam(fid, shcl, sz, lhl, lvl);
        fontContext->printxy(lx + 1, ly + 1, (char*)txt.c_str());
    }
    if (tp == tBOLDSHADOW)
    {
        fontContext->setFontParam(fid, shcl, sz, lhl, lvl);
        fontContext->printxy(lx + 2, ly + 2, (char*)txt.c_str());
        fontContext->printxy(lx + 2, ly + 3, (char*)txt.c_str());
        fontContext->printxy(lx + 3, ly + 2, (char*)txt.c_str());
        fontContext->printxy(lx + 3, ly + 3, (char*)txt.c_str());

    }
    fontContext->setFontParam(fid, lcl, sz, lhl, lvl);
    fontContext->printxy(lx, ly, (char*)txt.c_str());
    if (tp == tBOLD || tp == tBOLDSHADOW)
    {
        fontContext->printxy(lx, ly + 1, (char*)txt.c_str());
        fontContext->printxy(lx + 1, ly, (char*)txt.c_str());
        fontContext->printxy(lx + 1, ly + 1, (char*)txt.c_str());
    }
    grContext->restoreDefaultContext();
}

void tLabel::create(int x, int y, int dx, int dy, unsigned int col, std::string text)
{
    lx = x;
    ly = y;
    lx1 = x + dx;
    ly1 = y + dy;
    fid = 0;
    sz = 1;
    tp = 0;
    lhl = 0;
    lvl = 0;
    lcl = col;
    bkexists = false;
    bkcolor = 0;
    shcl = 0;
    txt = text;
    visible = false;
    enable = true;
    lbl = NULL;
}

void tLabel::setparam(int fontid, int size, int type, char halign, char valign)
{
    fid = fontid;
    sz = size;
    tp = type;
    lhl = halign;
    lvl = valign;
}

void tLabel::setBkColor(unsigned int color)
{
    bkexists = true;
    bkcolor = color;
}

void tLabel::setBkType(char type)
{
    bktp = type;
}

void tLabel::setColor(unsigned int col)
{
    lcl = col;
}

void tLabel::setShadowColor(unsigned int col)
{
    shcl = col;
}

void tLabel::changeColor(uint32_t col) {
    if (lcl != col) {
        lcl = col;
        if (visible) {
            grContext->putImage(lx, ly, lbl);
            outText();
        }
    }
}

void tLabel::changecaption(std::string text)
{
    if (txt != text) {
        txt = text;
        if (visible) {
            grContext->putImage(lx, ly, lbl);
            outText();
        }
    }
}

void tLabel::Visibled(bool vis) {
    if (vis != visible) {
        visible = vis;
        if (!visible) {
            if (lbl != NULL) {
                grContext->putImage(lx, ly, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
        }
        else {
            lbl = grContext->getImage(lx, ly, lx1, ly1);
            outText();
        }
    }
}

std::string tLabel::getText() {
    return txt;
}

void tLabel::getPositionAndSize(int* x, int* y, int* x1, int* y1) {
    *x = lx;
    *y = ly;
    *x1 = lx1;
    *y1 = ly1;
}

uint32_t tLabel::getColor() {
    return lcl;
}

uint16_t tLabel::GetX() { return lx; }
uint16_t tLabel::GetX1() { return lx1; }
uint16_t tLabel::GetY() { return ly; }
uint16_t tLabel::GetY1() { return ly1; }

void tLabel::Enabled(bool en) {

}

void tLabel::Destroy()
{
    if (lbl != NULL)
    {
        grContext->putImage(lx, ly, lbl);
        grContext->freeImage(lbl);
        lbl = 0;
    }
    bkexists = false;
    bkcolor = 0;
    bktp = 0;
    fid = 0;
    sz = 0;
    tp = 0;
    shcl = 0;
    visible = false;
}

bool tLabel::getFocus() {
    return false;
}

void tLabel::OnKeyDown(uint32_t param1, uint32_t param2) {

}

void tLabel::OnClick(uint32_t param1, uint32_t param2) {

}

void tLabel::OnClickUp(uint32_t param1, uint32_t param2) {

}

void tLabel::OnMove(uint32_t param1, uint32_t param2) {

}

void tLabel::OnEndMove(uint32_t param1, uint32_t param2) {

}

int tLabel::getLengthTextInPixels(Font *fc, int fontid, int size, std::string text) {
    fc->setFontParam(fontid, 0, size, 0, 0);
    return fc->getLenTextInPixels((char*)text.c_str(), size);
}

tLabel::tLabel(Graph* gc, Font* fc) {
    grContext = gc;
    fontContext = fc;
}

tLabel::~tLabel() {
    if (lbl != NULL)
    {
        grContext->putImage(lx, ly, lbl);
        grContext->freeImage(lbl);
        lbl = 0;
    }
    bkexists = false;
    bkcolor = 0;
    bktp = 0;
    fid = 0;
    sz = 0;
    tp = 0;
    shcl = 0;
    visible = false;
}

tButton::tButton(Graph* gc, Font* fc) {
    grContext = gc;
    fontContext = fc;
}

void tButton::create(int x, int y, int dx, int dy, std::string text) {
    bx = x;
    by = y;
    bx1 = x + dx;
    by1 = y + dy;
    this->dx = dx;
    this->dy = dy;
    txt = text;
    btp = 0;
    visible = false;
    enable = true;
    lbl = NULL;
    caption = NULL;
}

void tButton::setSpecialButtonType(uint8_t tp) {
    btp = tp;
}

void tButton::changePosition(int x, int y) {
    if (visible) {
        Visibled(false);
        bx = x;
        by = y;
        bx1 = x + dx;
        by1 = y + dy;
        Visibled(true);
    }
    else {
        bx = x;
        by = y;
        bx1 = x + dx;
        by1 = y + dy;
    }
}

void tButton::getPositionAndSize(int* x, int* y, int* x1, int* y1) {
    *x = bx;
    *y = by;
    *x1 = bx1;
    *y1 = by1;
}

void tButton::Visibled(bool vis) {
    if (vis != visible) {
        visible = vis;
        if (visible) {
            lbl = grContext->getImage(bx, by, bx1, by1);
            grContext->SetFillColor(cWINLIGHTGRAY);
            grContext->bar(bx, by, bx1, by1);
            grContext->SetColor(cWHITE);
            grContext->line(bx + 1, by + 1, bx1 - 1, by + 1);
            grContext->line(bx+1, by+1, bx+1, by1-1);
            grContext->SetColor(cWINDARKGRAY);
            grContext->line(bx, by1, bx1, by1);
            grContext->line(bx1, by, bx1, by1);
            grContext->SetColor(cWINGRAY);
            grContext->line(bx+1, by1-1, bx1-1, by1-1);
            grContext->line(bx1 - 1, by + 1, bx1 - 1, by1 - 1);
            if (!btp) {
                caption = new tLabel(grContext, fontContext);
                caption->create(bx, by, dx, dy, cBLACK, txt);
                caption->setparam(2, 1, 0, alHCENTER, alVCENTER);
                caption->Visibled(true);
            }
            else {
                switch (btp) {
                case 1:
                {
                    grContext->SetColor(cBLACK);
                    for (int i = 0; i < 4; i++) {
                        grContext->line(bx + 4 + i, by + 9 - i, bx1 - 5 - i, by + 9 - i);
                    }
                }
                break;
                case 2:
                {
                    grContext->SetColor(cBLACK);
                    for (int i = 0; i < 4; i++) {
                        grContext->line(bx + 4 + i, by + 6 + i, bx1 - 5 - i, by + 6 + i);
                    }                
                }
                break;
                }
            }
        }
        else {
            if (!btp) {
                if (caption) {
                    caption->Destroy();
                    delete caption;
                    caption = NULL;
                }
            }
            if (lbl != NULL)
            {
                grContext->putImage(bx, by, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
        }
    }
}

void tButton::Enabled(bool en) {

}

std::string tButton::getText() {
    return txt;
}

bool tButton::getFocus() {
    return false;
}

void tButton::OnKeyDown(uint32_t param1, uint32_t param2) {

}

void tButton::OnClick(uint32_t param1, uint32_t param2) {
    if (visible) {
        if (!btp) {
            if (caption) {
                caption->Destroy();
                delete caption;
                caption = NULL;
            }
        }
        if (lbl != NULL)
        {
            grContext->putImage(bx, by, lbl);
        }
        grContext->SetColor(cBLACK);
        grContext->rectangle(bx, by, bx1, by1);
        grContext->SetColor(cWINGRAY);
        grContext->rectangle(bx + 1, by + 1, bx1 - 1, by1 - 1);
        if (!btp) {
            caption = new tLabel(grContext, fontContext);
            caption->create(bx + 2, by + 2, dx, dy, cBLACK, txt);
            caption->setparam(2, 1, 0, alHCENTER, alVCENTER);
            caption->Visibled(true);
        }
        else {
            switch (btp) {
            case 1:
            {
                grContext->SetColor(cBLACK);
                for (int i = 0; i < 4; i++) {
                    grContext->line(bx + 4 + i, by + 10 - i, bx1 - 5 - i, by + 10 - i);
                }
            }
            break;
            case 2:
            {
                grContext->SetColor(cBLACK);
                for (int i = 0; i < 4; i++) {
                    grContext->line(bx + 4 + i, by + 7 + i, bx1 - 5 - i, by + 7 + i);
                }
            }
            break;
            }
        }
    }

}

void tButton::OnClickUp(uint32_t param1, uint32_t param2) {
    if (visible) {
        if (!btp) {
            if (caption) {
                caption->Destroy();
                delete caption;
                caption = NULL;
            }
        }
        if (lbl != NULL)
        {
            grContext->putImage(bx, by, lbl);
        }
        lbl = grContext->getImage(bx, by, bx1, by1);
        grContext->SetFillColor(cWINLIGHTGRAY);
        grContext->bar(bx, by, bx1, by1);
        grContext->SetColor(cWHITE);
        grContext->line(bx + 1, by + 1, bx1 - 1, by + 1);
        grContext->line(bx + 1, by + 1, bx + 1, by1 - 1);
        grContext->SetColor(cWINDARKGRAY);
        grContext->line(bx, by1, bx1, by1);
        grContext->line(bx1, by, bx1, by1);
        grContext->SetColor(cWINGRAY);
        grContext->line(bx + 1, by1 - 1, bx1 - 1, by1 - 1);
        grContext->line(bx1 - 1, by + 1, bx1 - 1, by1 - 1);
        if (!btp) {
            caption = new tLabel(grContext, fontContext);
            caption->create(bx, by, dx, dy, cBLACK, txt);
            caption->setparam(2, 1, 0, alHCENTER, alVCENTER);
            caption->Visibled(true);
        }
        else {
            switch (btp) {
            case 1:
            {
                grContext->SetColor(cBLACK);
                for (int i = 0; i < 4; i++) {
                    grContext->line(bx + 4 + i, by + 9 - i, bx1 - 5 - i, by + 9 - i);
                }
            }
            break;
            case 2:
            {
                grContext->SetColor(cBLACK);
                for (int i = 0; i < 4; i++) {
                    grContext->line(bx + 4 + i, by + 6 + i, bx1 - 5 - i, by + 6 + i);
                }
            }
            break;
            }
        }
    }
}

void tButton::OnMove(uint32_t param1, uint32_t param2) {

}

void tButton::OnEndMove(uint32_t param1, uint32_t param2) {

}

void tButton::Destroy()
{
    if (caption) {
        caption->Destroy();
        delete caption;
        caption = NULL;
    }
    if (lbl != NULL)
    {
        grContext->putImage(bx, by, lbl);
        grContext->freeImage(lbl);
        lbl = NULL;
    }
    visible = false;
    enable = false;
    bx = 0;
    by = 0;
    bx1 = 0;
    by1 = 0;
    txt = "";
    btp = 0;
}

tCursor::tCursor(Graph* gc, Font* fc, uint32_t winId, uint32_t evType) {
    cx = 0;
    cy = 0;
    height = 0;
    visible = false;
    fontid = 0;
    size = 0;
    wid = winId;
    curEvent = evType;
    lbl = NULL;

    grContext = gc;
    fontContext = fc;
}
tCursor::~tCursor() {
    visible = false;
    Sleep(5);
    if (lbl) {
        grContext->putImage(cx, cy, lbl);
        grContext->freeImage(lbl);
        lbl = NULL;
    }
    cx = 0;
    cy = 0;
    height = 0;
    visible = false;
    fontid = 0;
    size = 0;
    wid = 0;
    curEvent = 0;

    grContext = NULL;
    fontContext = NULL;
}

void tCursor::create(int x, int y, int fid, int sz) {
    cx = x;
    cy = y;
    fontid = fid;
    size = sz;
    height = fontContext->getFontHeight(fontid, size);
}

void tCursor::sendCursorEvent() {
    SDL_Event event;
    SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
    event.type = curEvent;
    event.window.windowID = wid;
    event.user.code = 0;
    event.user.data1 = 0;
    event.user.data2 = 0;
    SDL_PushEvent(&event);
}

void tCursor::Visibled(bool vis) {
    if (visible != vis) {
        visible = vis;
        if (visible) {
            lbl = grContext->getImage(cx, cy, cx+1, cy + height);
            std::thread([=] {
                while (1)
                {
                    if (!visible) break;
                    grContext->SetColor(cBLACK);
                    grContext->line(cx, cy, cx, cy + height);
                    sendCursorEvent();
                    for (int i = 0; i < 250; i++) {
                        if (!visible) break;
                        Sleep(1);
                    }
                    if (lbl) {
                        grContext->putImage(cx, cy, lbl);
                        sendCursorEvent();
                    }
                    for (int i = 0; i < 250; i++) {
                        if (!visible) break;
                        Sleep(1);
                    }
                }
                }).detach();
        }
        else {
            Sleep(5);
            if (lbl) {
                grContext->putImage(cx, cy, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
            sendCursorEvent();
        }
    }
}

void tCursor::changePosition(int x, int y) {
    //Visibled(false);
    cx = x;
    cy = y;
    //Visibled(true);
}

tEdit::tEdit(Graph* gc, Font* fc, uint32_t winId, uint32_t evType) {
    grContext = gc;
    fontContext = fc;
    editText = new tLabel(gc, fc);
    cursor = new tCursor(gc, fc, winId, evType);
}

void tEdit::Destroy() {
    delete cursor;
    delete editText;
    if (lbl != NULL)
    {
        grContext->putImage(ex, ey, lbl);
        grContext->freeImage(lbl);
        lbl = NULL;
    }
    visible = false;
    enable = false;
    ex = 0;
    ey = 0;
    ex1 = 0;
    ey1 = 0;
    fontid = 0;
    size = 0;
    txt = "";
    focus = false;
    maxSymbols = 0;
 }

void tEdit::create(int x, int y, int dx, int dy, int fid, int sz, std::string text) {
    ex = x;
    ey = y;
    ex1 = x + dx;
    ey1 = y + dy;
    this->dx = dx;
    this->dy = dy;
    fontid = fid;
    size = sz;
    txt = text;
    visible = false;
    enable = true;
    lbl = NULL;
    focus = false;
    maxSymbols = 0xFFFF;
    editText->create(ex + 4, ey + 4, dx - 4, dy - 4, cBLACK, text);
    editText->setparam(fontid, size, 0, 0, 0);
    int cdx = tLabel::getLengthTextInPixels(fontContext, fontid, 1, text);
    cursor->create(ex + 4 + cdx, ey + 4, fontid, size);
    cursorPosition = txt.length();
}

void tEdit::Visibled(bool vis) {
    if (vis != visible) {
        visible = vis;
        if (visible) {
            lbl = grContext->getImage(ex, ey, ex1, ey1);
            grContext->SetFillColor(cWHITE);
            grContext->bar(ex, ey, ex1, ey1);
            grContext->SetColor(cWINGRAY);
            grContext->line(ex, ey, ex1 - 1, ey);
            grContext->line(ex, ey, ex, ey1 - 1);
            grContext->SetColor(cWINDARKGRAY);
            grContext->line(ex + 1, ey + 1, ex1 - 2, ey + 1);
            grContext->line(ex + 1, ey + 1, ex + 1, ey1 - 2);
            grContext->SetColor(cWINLIGHTGRAY);
            grContext->line(ex+1, ey1-1, ex1-1, ey1-1);
            grContext->line(ex1-1, ey+1, ex1-1, ey1-1);
            editText->Visibled(true);
        }
        else {
            if (lbl != NULL)
            {
                grContext->putImage(ex, ey, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
        }
    }
}

std::string tEdit::getText() {
    return txt;
}

void tEdit::setMaxSymbols(uint16_t n) {
    maxSymbols = n;
}


void tEdit::getPositionAndSize(int* x, int* y, int* x1, int* y1) {
    *x = ex;
    *y = ey;
    *x1 = ex1;
    *y1 = ey1;
}

void tEdit::lostFocus() {
    focus = false;
    cursor->Visibled(false);
}

void tEdit::setFocus() {
    focus = true;
}

bool tEdit::getFocus() {
    return focus;
}

void tEdit::Enabled(bool en) {

}

void tEdit::OnKeyDown(uint32_t param1, uint32_t param2) {
    printf("tEdit key pressed %c\n", param1);
    if (param1 >= 0x20 && param1 < 255 && param1 != 0x7F) {
        if (txt.length() < maxSymbols) {
            cursor->Visibled(false);
            int cdx = 0;
            if (cursorPosition == txt.length()) {
                txt = txt + (char)param1;
                cursorPosition = txt.length();
                cdx = tLabel::getLengthTextInPixels(fontContext, fontid, 1, txt);
            }
            else {
                txt.insert(cursorPosition, 1, (char)param1);
                cursorPosition++;
                std::string tempTXT = txt.substr(0, cursorPosition);
                cdx = tLabel::getLengthTextInPixels(fontContext, fontid, 1, tempTXT);
            }
            cursor->changePosition(ex + 4 + cdx, ey + 4);
            editText->changecaption(txt);
            cursor->Visibled(true);
        }
    }
    if (param1 == 0x08) {
        cursor->Visibled(false);
        int cdx = 0;
        if (!txt.empty() && cursorPosition == txt.length()) {
            txt.pop_back();
            cursorPosition = txt.length();
            cdx = tLabel::getLengthTextInPixels(fontContext, fontid, 1, txt);
        }
        else {
            if (cursorPosition > 0) {
                txt.erase(cursorPosition - 1, 1);
                cursorPosition--;
            }
            std::string tempTXT = txt.substr(0, cursorPosition);
            cdx = tLabel::getLengthTextInPixels(fontContext, fontid, 1, tempTXT);

        }
        cursor->changePosition(ex + 4 + cdx, ey + 4);
        editText->changecaption(txt);
        cursor->Visibled(true);
    }
    if (param1 == 0x7F) {
        if (cursorPosition < txt.length()) {
            txt.erase(cursorPosition, 1);
            cursor->Visibled(false);
            editText->changecaption(txt);
            cursor->Visibled(true);
        }
    }
    if (param1 == 0x40000050) { // left key
        printf("press left key\n");
        if (cursorPosition > 0) cursorPosition--;
        std::string tempTXT = txt.substr(0,cursorPosition);
        cursor->Visibled(false);
        int cdx = tLabel::getLengthTextInPixels(fontContext, fontid, 1, tempTXT);
        cursor->changePosition(ex + 4 + cdx, ey + 4);
        cursor->Visibled(true);
    }
    if (param1 == 0x4000004F) {// right key
        printf("press right key\n");
        if (cursorPosition < txt.length()) cursorPosition++;
        std::string tempTXT = txt.substr(0, cursorPosition);
        cursor->Visibled(false);
        int cdx = tLabel::getLengthTextInPixels(fontContext, fontid, 1, tempTXT);
        cursor->changePosition(ex + 4 + cdx, ey + 4);
        cursor->Visibled(true);
    }
}

void tEdit::OnClick(uint32_t param1, uint32_t param2) {

}

void tEdit::OnClickUp(uint32_t param1, uint32_t param2) {
    setFocus();
    cursor->Visibled(true);
}

void tEdit::OnMove(uint32_t param1, uint32_t param2) {

}

void tEdit::OnEndMove(uint32_t param1, uint32_t param2) {

}


tHexViewer::tHexViewer(Graph* gc, Font* fc, Memory* mem, bool rom, uint32_t winId) {
    grContext = gc;
    fontContext = fc;
    memory = mem;
    win = winId;
    hx = 0;
    hy = 0;
    hx1 = 0;
    hy1 = 0;
    dx = 0;
    dy = 0;
    lbl = NULL;
    visible = false;
    enable = false;
    isROM = rom;
    scrollPosition = 0;
    scrollAdd = 0;
    ln = 0;
    memPointer = memory->getMemoryPointer(isROM, &startMemAddr, &maxMemAddr);
    curMemAddr = 0;
    for (int i = 0; i < 16; i++) {
        addrs[i] = new tLabel(grContext, fontContext);
    }
    for (int i = 0; i < 16; i++) 
        for (int j = 0; j < 16; j++) 
            datas[i][j] = new tLabel(grContext, fontContext);
    for (int i = 0; i < 16; i++) stringDatas[i] = new tLabel(grContext, fontContext);
    bMiddle = new tButton(grContext, fontContext);
    bDown = new tButton(grContext, fontContext);
    bUp = new tButton(grContext, fontContext);
    mx = 0;
    my = 0;
    scrollPressed = false;
}

tHexViewer::~tHexViewer() {
    delete bUp;
    delete bDown;
    delete bMiddle;
    for (int i = 0; i < 16; i++) delete stringDatas[i];
    for (int i = 0; i < 16; i++) for (int j = 0; j < 16; j++) delete datas[i][j];
    for (int i = 0; i < 16; i++) delete addrs[i];
    grContext = NULL;
    fontContext = NULL;
    memory = NULL;
    memPointer = NULL;
    win = 0;
    hx = 0;
    hy = 0;
    hx1 = 0;
    hy1 = 0;
    dx = 0;
    dy = 0;
    lbl = NULL;
    visible = false;
    enable = false;
    startMemAddr = 0;
    maxMemAddr = 0;
    curMemAddr = 0;
    isROM = false;
    scrollPosition = 0;
    scrollAdd = 0;
    ln = 0;
    mx = 0;
    my = 0;
    scrollPressed = false;
}

bool tHexViewer::isROMStatus() {
    return isROM;
}

uint16_t tHexViewer::getLebgth() {
    return ln;
}

void tHexViewer::create(int x, int y) {
    hx = x;
    hy = y;
    dx = 600;
    dy = 340;
    hx1 = hx + dx;
    hy1 = hy + dy;
    for (int i = 0; i < 16; i++) {
        addrs[i]->create(hx + 10, hy + (20 * i) + 10, 36, 20, cBLACK, decToHexWord(i * 16));
        addrs[i]->setparam(0, 1, 0, 0, 0);
    }
    for (int i = 0; i < 16; i++) 
        for (int j = 0; j < 16; j++) {
            datas[i][j]->create(hx + 58 + (24 * j), hy + (20 * i) + 10, 20, 20, cBLACK, decToHexByte(memPointer[i*16+j]));
            datas[i][j]->setparam(0, 1, 0, 0, 0);
    }
    for (int i = 0; i < 16; i++) {
        std::string txt = "";
        for (int j = 0; j < 16; j++) {
            if (memPointer[i * 16 + j] < 0x20) txt += "."; else txt += memPointer[i * 16 + j];
        }
        stringDatas[i]->create(hx + 450, hy + (20 * i) + 10, 132, 20, cBLACK, txt);
    }
    bUp->create(hx1 - 19, hy + 3, 16, 16, "");
    bUp->setSpecialButtonType(tButtonUp);
    bDown->create(hx1 - 19, hy1 - 19, 16, 16, "");
    bDown->setSpecialButtonType(tButtonDown);
    ln = (maxMemAddr - startMemAddr) / 16 + 5;// -32;
    ln = (hy1 - 19 - hy) - ln;
    float tmp = (float)ln * (float)(-1.0);
    if (ln < 8) {
        ln = 8;
        scrollAdd = (float)(hy1 - 19 - hy - 8) / tmp;
    }
    else {
        scrollAdd = 1.0;
    }
    bMiddle->create(hx1 - 19, hy + 20, 16, ln, "");
    scrollPosition = hy + 20;
}


void tHexViewer::Visibled(bool vis) {
    if (vis != visible) {
        visible = vis;
        if (visible) {
            lbl = grContext->getImage(hx, hy, hx1, hy1);
            grContext->SetFillColor(cWHITE);
            grContext->bar(hx, hy, hx1, hy1);
            grContext->SetColor(cWINGRAY);
            grContext->line(hx, hy, hx1 - 1, hy);
            grContext->line(hx, hy, hx, hy1 - 1);
            grContext->SetColor(cWINDARKGRAY);
            grContext->line(hx + 1, hy + 1, hx1 - 2, hy + 1);
            grContext->line(hx + 1, hy + 1, hx + 1, hy1 - 2);
            grContext->SetColor(cWINLIGHTGRAY);
            grContext->line(hx + 1, hy1 - 1, hx1 - 1, hy1 - 1);
            grContext->line(hx1 - 1, hy + 1, hx1 - 1, hy1 - 1);
            
            for (int y = hy+3; y < hy1-3; y++) {
                for (int x = 0; x < 16; x++) {
                    if ((y % 2 == 0 && x % 2 == 0) || (y % 2 == 1 && x % 2 == 1)) grContext->putpixel(hx1 - 19 + x, y, cWINLIGHTGRAY); else grContext->putpixel(hx1 - 19 + x, y, cWHITE);
                }
            }
            
            for (int i = 0; i < 16; i++) addrs[i]->Visibled(true);
            for (int i = 0; i < 16; i++)
                for (int j = 0; j < 16; j++)
                    datas[i][j]->Visibled(true);
            for (int i = 0; i < 16; i++) stringDatas[i]->Visibled(true);
            bUp->Visibled(true);
            bDown->Visibled(true);
            bMiddle->Visibled(true);
        }
        else {
            bMiddle->Visibled(false);
            bDown->Visibled(false);
            bUp->Visibled(false);
            for (int i = 0; i < 16; i++) addrs[i]->Visibled(false);
            for (int i = 0; i < 16; i++)
                for (int j = 0; j < 16; j++)
                    datas[i][j]->Visibled(false);
            for (int i = 0; i < 16; i++) stringDatas[i]->Visibled(false);
            if (lbl != NULL)
            {
                grContext->putImage(hx, hy, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
        }
    }
}

void tHexViewer::update() {
    for (int i = 0; i < 16; i++) addrs[i]->changecaption(decToHexWord(curMemAddr + i * 16));
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++) {
            datas[i][j]->changeColor(cBLACK);
            datas[i][j]->changecaption(decToHexByte(memPointer[curMemAddr + i * 16 + j]));
        }
    for (int i = 0; i < 16; i++) {
        std::string txt = "";
        for (int j = 0; j < 16; j++) {
            if (memPointer[curMemAddr + i * 16 + j] < 0x20) txt += "."; else txt += memPointer[curMemAddr + i * 16 + j];
        }
        stringDatas[i]->changecaption(txt);
    }
}

void tHexViewer::updateData() {
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++) {
            std::string t = decToHexByte(memPointer[curMemAddr + i * 16 + j]);
            if (datas[i][j]->getText() != t) {
                datas[i][j]->changeColor(cLIGHTRED);
                datas[i][j]->changecaption(decToHexByte(memPointer[curMemAddr + i * 16 + j]));
            }
            else datas[i][j]->changeColor(cBLACK);
        }
    for (int i = 0; i < 16; i++) {
        std::string txt = "";
        for (int j = 0; j < 16; j++) {
            if (memPointer[curMemAddr + i * 16 + j] < 0x20) txt += "."; else txt += memPointer[curMemAddr + i * 16 + j];
        }
        stringDatas[i]->changecaption(txt);
    }
}

std::string tHexViewer::getTextElement(int i, int j) {
    return datas[i][j]->getText();
}

void tHexViewer::setMemoryForTextElement(int i, int j, std::string sData) {
    uint8_t data = hexToDec(sData);
    memPointer[curMemAddr + i * 16 + j] = data;
}

std::string tHexViewer::getText() {
    return "";
}

void tHexViewer::getPositionAndSize(int* x, int* y, int* x1, int* y1) {
    *x = hx;
    *y = hy;
    *x1 = hx1;
    *y1 = hy1;
}

void tHexViewer::Enabled(bool en) {

}


void tHexViewer::OnKeyDown(uint32_t param1, uint32_t param2) {

}

void tHexViewer::OnClick(uint32_t param1, uint32_t param2) {
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy + 3 && param2 <= hy + 19) bUp->OnClick(param1, param2);
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy1 - 19 && param2 <= hy1 - 3) bDown->OnClick(param1, param2);
    int x, y, x1, y1;
    bMiddle->getPositionAndSize(&x, &y, &x1, &y1);
    if (param1 >= x && param1 <= x1 && param2 >= y && param2 <= y1) {
        mx = param1;
        my = param2;
        scrollPressed = true;
    }
}


void tHexViewer::OnClickUp(uint32_t param1, uint32_t param2) {
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy + 3 && param2 <= hy + 19) {
        bUp->OnClickUp(param1, param2);
        if (curMemAddr > 0) {
            curMemAddr -= 16;
            scrollPosition -= scrollAdd;
            bMiddle->changePosition(hx1 - 19, (int)round(scrollPosition));
            update();
        }
    }
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy1 - 19 && param2 <= hy1 - 3) {
        bDown->OnClickUp(param1, param2);
        if (curMemAddr < (maxMemAddr - 256)) {
            curMemAddr += 16;
            scrollPosition += scrollAdd;
            bMiddle->changePosition(hx1 - 19, (int)round(scrollPosition));
            update();
        }
    }
    scrollPressed = false;
}


void tHexViewer::OnMove(uint32_t param1, uint32_t param2) {
    if (scrollPressed) {
        if (param2 != my) {
            if (param2 > my) {
                float oldSP = scrollPosition;
                scrollPosition += (param2 - my);
                float step = (scrollPosition - oldSP) / scrollAdd;
                if (scrollPosition + ln <= hy1 - 20) {
                    curMemAddr += (16 * (int)round(step));
                }
                else {
                    curMemAddr = maxMemAddr - 256;
                    scrollPosition = hy1 - 20 - ln;
                }
                bMiddle->changePosition(hx1 - 19, scrollPosition);
                update();
            }
            else {
                float oldSP = scrollPosition;
                scrollPosition -= (my - param2);
                float step = (oldSP - scrollPosition) / scrollAdd;
                if (scrollPosition < hy + 20) {
                    curMemAddr = 0;
                    scrollPosition = hy + 20;
                }
                else {
                    curMemAddr -= ((int)round(step) * 16);
                }
                bMiddle->changePosition(hx1 - 19, scrollPosition);
                update();
            }
            mx = param1;
            my = param2;
        }
    }
}


void tHexViewer::OnEndMove(uint32_t param1, uint32_t param2) {

}


void tHexViewer::Destroy() {

}

bool tHexViewer::getFocus() {
    return false;
}


tListBox::~tListBox() {
    for (int i = 0; i < 16; i++) delete lst[i];
    if (lbl) {
        grContext->putImage(ex, ey, lbl);
        grContext->freeImage(lbl);
        lbl = NULL;
    }
    txt = NULL;
}

tListBox::tListBox(Graph* gc, Font* fc, uint32_t evType) {
    grContext = gc;
    fontContext = fc;
    for (int i = 0; i < 16; i++) lst[i] = new tLabel(grContext, fontContext);
}

uint8_t tListBox::getCount() {
    uint8_t i = 0;
    while (txt[i] != "") i++;
    return i;
}

void tListBox::create(int x, int y, int dx, int dy, int fid, int sz, std::string* text) {
    ex = x;
    ey = y;
    ex1 = x + dx;
    ey1 = y + dy;
    this->dx = dx;
    this->dy = dy;
    fontid = fid;
    size = sz;
    txt = text;
    visible = false;
    enable = true;
    lbl = NULL;
    uint8_t cnt = getCount();
    for (int i = 0; i < cnt; i++) {
        lst[i]->create(ex + 5, ey + 10 + i * 25, dx - 10, 25, cBLACK, text[i]);
        lst[i]->setparam(2, 1, 0, 0, 0);
    }
    selected = 255;
}

void tListBox::Visibled(bool vis) {
    if (vis != visible) {
        visible = vis;
        if (visible) {
            lbl = grContext->getImage(ex, ey, ex1, ey1);
            grContext->SetFillColor(cWHITE);
            grContext->bar(ex, ey, ex1, ey1);
            grContext->SetColor(cBLACK);
            grContext->rectangle(ex, ey, ex1, ey1);
            uint8_t cnt = getCount();
            for (int i = 0; i < cnt; i++) {
                lst[i]->Visibled(true);
            }
        }
        else {
            uint8_t cnt = getCount();
            for (int i = 0; i < cnt; i++) {
                lst[i]->Visibled(false);
            }
            if (lbl != NULL)
            {
                grContext->putImage(ex, ey, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
        }
    }
}

void tListBox::Enabled(bool en) {

}

bool tListBox::getFocus() {
    return false;
}

std::string tListBox::getText() {
    return txt[index];
}

void tListBox::getPositionAndSize(int* x, int* y, int* x1, int* y1) {

}

void tListBox::OnKeyDown(uint32_t param1, uint32_t param2) {

}

void tListBox::OnClick(uint32_t param1, uint32_t param2) {

}

void tListBox::OnClickUp(uint32_t param1, uint32_t param2) {

}

void tListBox::OnMove(uint32_t param1, uint32_t param2) {
    if (param2 != selected) {
        if (selected != 255) {
            lst[selected]->setBkColor(cWHITE);
            lst[selected]->changeColor(cBLACK);
        }
        lst[param2]->setBkColor(0x0A246A);
        lst[param2]->changeColor(cWHITE);
        selected = param2;
    }
}

void tListBox::OnEndMove(uint32_t param1, uint32_t param2) {

}

void tListBox::Destroy() {

}


tComboBox::tComboBox(Graph* gc, Font* fc, uint32_t evType) {
    grContext = gc;
    fontContext = fc;
    selectText = new tLabel(gc, fc);
    list = new tListBox(grContext, fontContext, evType);
    bDown = new tButton(grContext, fontContext);
}

tComboBox::~tComboBox() {
    delete bDown;
    delete list;
    delete selectText;
    if (lbl) {
        grContext->putImage(ex, ey, lbl);
        grContext->freeImage(lbl);
        lbl = NULL;
    }
    txt = NULL;
}

uint8_t tComboBox::getCount() {
    uint8_t i = 0;
    while (txt[i] != "") i++;
    return i;
}

void tComboBox::create(int x, int y, int dx, int dy, int fid, int sz, std::string* text, uint16_t indx) {
    ex = x;
    ey = y;
    ex1 = x + dx;
    ey1 = y + dy;
    this->dx = dx;
    this->dy = dy;
    fontid = fid;
    size = sz;
    txt = text;
    visible = false;
    enable = true;
    lbl = NULL;
    index = indx;
    open = false;
    selectText->create(ex + 4, ey + 2, dx - 23, dy - 2, cBLACK, text[index]);
    selectText->setparam(fontid, size, 0, 0, 0);
    bDown->create(ex1 - 19, ey1 - 19, 16, 16, "");
    bDown->setSpecialButtonType(tButtonDown);
    uint8_t cnt = getCount();
    list->create(ex, ey1 + 1, dx, cnt*25 + 20, fid, sz, text);
}

void tComboBox::Visibled(bool vis) {
    if (vis != visible) {
        visible = vis;
        if (visible) {
            lbl = grContext->getImage(ex, ey, ex1, ey1);
            grContext->SetFillColor(cWHITE);
            grContext->bar(ex, ey, ex1, ey1);
            grContext->SetColor(cWINGRAY);
            grContext->line(ex, ey, ex1 - 1, ey);
            grContext->line(ex, ey, ex, ey1 - 1);
            grContext->SetColor(cWINDARKGRAY);
            grContext->line(ex + 1, ey + 1, ex1 - 2, ey + 1);
            grContext->line(ex + 1, ey + 1, ex + 1, ey1 - 2);
            grContext->SetColor(cWINLIGHTGRAY);
            grContext->line(ex + 1, ey1 - 1, ex1 - 1, ey1 - 1);
            grContext->line(ex1 - 1, ey + 1, ex1 - 1, ey1 - 1);
            selectText->Visibled(true);
            bDown->Visibled(true);
        }
        else {
            bDown->Visibled(false);
            selectText->Visibled(false);
            if (lbl != NULL)
            {
                grContext->putImage(ex, ey, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
        }
    }
}

void tComboBox::Enabled(bool en) {

}

bool tComboBox::isOpen() {
    return open;
}

bool tComboBox::getFocus() {
    return false;
}

std::string tComboBox::getText() {
    return txt[index];
}

void tComboBox::getPositionAndSize(int* x, int* y, int* x1, int* y1) {

}

void tComboBox::OnKeyDown(uint32_t param1, uint32_t param2) {

}

void tComboBox::OnClick(uint32_t param1, uint32_t param2) {
    printf("Open Combobox\n");
    if (param1 == 0) {
        if (!open) {
            open = true;
            list->Visibled(true);
        }
        else {
            open = false;
            list->Visibled(false);
        }
    }
    if (param1 == 1) {
        open = false;
        list->Visibled(false);
        index = param2;
        selectText->changecaption(txt[index]);
    }
}

void tComboBox::OnClickUp(uint32_t param1, uint32_t param2) {

}

void tComboBox::OnMove(uint32_t param1, uint32_t param2) {
    list->OnMove(param1, param2);
}

void tComboBox::OnEndMove(uint32_t param1, uint32_t param2) {

}

void tComboBox::Destroy() {

}


tFileList::tFileList(Graph* gc, Font* fc, WIN32_FIND_DATAA* fls, uint32_t n, uint32_t winId) {
    grContext = gc;
    fontContext = fc;
    files = fls;
    count = n;
    win = winId;
    hx = 0;
    hy = 0;
    hx1 = 0;
    hy1 = 0;
    dx = 0;
    dy = 0;
    lbl = NULL;
    visible = false;
    enable = false;
    scrollPosition = 0;
    scrollAdd = 0;
    startSelect = 0;
    selectedItem = 0;
    ln = 0;
    //memPointer = memory->getMemoryPointer(isROM, &startMemAddr, &maxMemAddr);
    //curMemAddr = 0;
    for (int i = 0; i < 16; i++) {
        names[i] = new tLabel(grContext, fontContext);
        sizeAneDir[i] = new tLabel(grContext, fontContext);
        date[i] = new tLabel(grContext, fontContext);
    }

    bName = new tButton(grContext, fontContext);
    bSize = new tButton(grContext, fontContext);
    bDate = new tButton(grContext, fontContext);
    
    bMiddle = new tButton(grContext, fontContext);
    bDown = new tButton(grContext, fontContext);
    bUp = new tButton(grContext, fontContext);
    mx = 0;
    my = 0;
    scrollPressed = false;
    curSelect = 255;
}

tFileList::~tFileList() {
    delete bUp;
    delete bDown;
    delete bMiddle;

    delete bDate;
    delete bSize;
    delete bName;

    for (int i = 0; i < 16; i++) {
        delete names[i];
        delete sizeAneDir[i];
        delete date[i];
    }
    grContext = NULL;
    fontContext = NULL;
    if (files != NULL) {
        delete[] files;
        files = NULL;
    }
    win = 0;
    hx = 0;
    hy = 0;
    hx1 = 0;
    hy1 = 0;
    dx = 0;
    dy = 0;
    lbl = NULL;
    visible = false;
    enable = false;
    scrollPosition = 0;
    scrollAdd = 0;
    ln = 0;
    mx = 0;
    my = 0;
    scrollPressed = false;
    curSelect = 0;
}

string getFileTime(FILETIME* tm) {

    LPSYSTEMTIME st;
    st = new SYSTEMTIME;
    bool res = FileTimeToSystemTime((const FILETIME*)tm, st);
    char buf[64];
    sprintf_s(buf, 64, "%02d.%02d.%02d %02d:%02d", st->wDay, st->wMonth, st->wYear, st->wHour, st->wMinute);
    return string(buf);
}

void tFileList::create(int x, int y) {
    hx = x;
    hy = y;
    dx = 600;
    dy = 350;
    hx1 = hx + dx;
    hy1 = hy + dy;
    
    bName->create(hx + 2, hy + 2, 303, 20, "Имя");
    bSize->create(hx + 305, hy + 2, 100, 20, "Размер");
    bDate->create(hx + 405, hy + 2, 174, 20, "Дата");
    
    uint32_t cnt = 16;
    if (count < 16) cnt = count;
    for (int i = 0; i < cnt; i++) {
        names[i]->create(hx + 5, hy + (20 * i) + 25, 300, 20, cBLACK, files[i].cFileName);
        names[i]->setparam(2, 1, 0, 0, 0);
        string tmp = "";
        if (files[i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) tmp = "DIR"; else {
            char buf[20];
            sprintf_s(buf, 20, "%d", files[i].nFileSizeLow);
            tmp = string(buf);
        }
        sizeAneDir[i]->create(hx + 305, hy + (20 * i) + 25, 100, 20, cBLACK, tmp);
        sizeAneDir[i]->setparam(2, 1, 0, 0, 0);
        date[i]->create(hx + 405, hy + (20 * i) + 25, 170, 20, cBLACK, getFileTime(&files[i].ftCreationTime));
        date[i]->setparam(2, 1, 0, 0, 0);
    }
    bUp->create(hx1 - 19, hy + 3, 16, 16, "");
    bUp->setSpecialButtonType(tButtonUp);
    bDown->create(hx1 - 19, hy1 - 19, 16, 16, "");
    bDown->setSpecialButtonType(tButtonDown);
    ln = count;// -32;
    ln = (hy1 - 19 - hy - 5) - ln;
    float tmp = (float)ln * (float)(-1.0);
    if (ln < 8) {
        ln = 8;
        scrollAdd = (float)(hy1 - 19 - hy - 8) / tmp;
    }
    else {
        scrollAdd = 1.0;
    }
    if (count < 16) ln = hy1 - hy - 40;
    bMiddle->create(hx1 - 19, hy + 20, 16, ln, "");
    scrollPosition = hy + 20;
}

void tFileList::Visibled(bool vis) {
    if (vis != visible) {
        visible = vis;
        uint32_t cnt = 16;
        if (count < 16) cnt = count;
        if (visible) {
            lbl = grContext->getImage(hx, hy, hx1, hy1);
            grContext->SetFillColor(cWHITE);
            grContext->bar(hx, hy, hx1, hy1);
            grContext->SetColor(cWINGRAY);
            grContext->line(hx, hy, hx1 - 1, hy);
            grContext->line(hx, hy, hx, hy1 - 1);
            grContext->SetColor(cWINDARKGRAY);
            grContext->line(hx + 1, hy + 1, hx1 - 2, hy + 1);
            grContext->line(hx + 1, hy + 1, hx + 1, hy1 - 2);
            grContext->SetColor(cWINLIGHTGRAY);
            grContext->line(hx + 1, hy1 - 1, hx1 - 1, hy1 - 1);
            grContext->line(hx1 - 1, hy + 1, hx1 - 1, hy1 - 1);

            for (int y = hy + 3; y < hy1 - 3; y++) {
                for (int x = 0; x < 16; x++) {
                    if ((y % 2 == 0 && x % 2 == 0) || (y % 2 == 1 && x % 2 == 1)) grContext->putpixel(hx1 - 19 + x, y, cWINLIGHTGRAY); else grContext->putpixel(hx1 - 19 + x, y, cWHITE);
                }
            }

            bName->Visibled(true);
            bSize->Visibled(true);
            bDate->Visibled(true);

            for (int i = 0; i < cnt; i++) {
                names[i]->Visibled(true);
                sizeAneDir[i]->Visibled(true);
                date[i]->Visibled(true);
            }

            bUp->Visibled(true);
            bDown->Visibled(true);
            bMiddle->Visibled(true);
        }
        else {
            bMiddle->Visibled(false);
            bDown->Visibled(false);
            bUp->Visibled(false);
            
            bName->Visibled(false);
            bSize->Visibled(false);
            bDate->Visibled(false);
            
            for (int i = 0; i < cnt; i++) {
                names[i]->Visibled(false);
                sizeAneDir[i]->Visibled(false);
                date[i]->Visibled(false);
            }
            if (lbl != NULL)
            {
                grContext->putImage(hx, hy, lbl);
                grContext->freeImage(lbl);
                lbl = NULL;
            }
        }
    }
}

void tFileList::update() {
    
    for (int i = 0; i < 16; i++) {
        names[i]->changecaption(files[startSelect + i].cFileName);
        string tmp = "";
        if (files[startSelect + i].dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) tmp = "DIR"; else {
            char buf[20];
            sprintf_s(buf, 20, "%d", files[startSelect + i].nFileSizeLow);
            tmp = string(buf);
        }
        sizeAneDir[i]->changecaption(tmp);
        date[i]->changecaption(getFileTime(&files[startSelect + i].ftCreationTime));
        if (startSelect + i == selectedItem) {
            names[i]->setBkColor(0x0A246A);
            names[i]->changeColor(cWHITE);
            sizeAneDir[i]->setBkColor(0x0A246A);
            sizeAneDir[i]->changeColor(cWHITE);
            date[i]->setBkColor(0x0A246A);
            date[i]->changeColor(cWHITE);
        }
        else {
            names[i]->setBkColor(cWHITE);
            names[i]->changeColor(cBLACK);
            sizeAneDir[i]->setBkColor(cWHITE);
            sizeAneDir[i]->changeColor(cBLACK);
            date[i]->setBkColor(cWHITE);
            date[i]->changeColor(cBLACK);
        }
    }

    
}

void tFileList::updateData() {
    /*
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 16; j++) {
            std::string t = decToHexByte(memPointer[curMemAddr + i * 16 + j]);
            if (datas[i][j]->getText() != t) {
                datas[i][j]->changeColor(cLIGHTRED);
                datas[i][j]->changecaption(decToHexByte(memPointer[curMemAddr + i * 16 + j]));
            }
            else datas[i][j]->changeColor(cBLACK);
        }
    for (int i = 0; i < 16; i++) {
        std::string txt = "";
        for (int j = 0; j < 16; j++) {
            if (memPointer[curMemAddr + i * 16 + j] < 0x20) txt += "."; else txt += memPointer[curMemAddr + i * 16 + j];
        }
        stringDatas[i]->changecaption(txt);
    }
    */
}

std::string tFileList::getText() {
    return "";
}

uint16_t tFileList::getSelectedIndex() {
    return startSelect + curSelect;
}

std::string tFileList::getSelectedFileName() {
    if (curSelect != 255) {
        return names[curSelect]->getText();
    }
    else return"";
}

std::string tFileList::getSelectedFileSize() {
    if (curSelect != 255) {
        return sizeAneDir[curSelect]->getText();
    }
    else return "";
}

void tFileList::getPositionAndSize(int* x, int* y, int* x1, int* y1) {
    *x = hx;
    *y = hy;
    *x1 = hx1;
    *y1 = hy1;
}

void tFileList::Enabled(bool en) {

}


void tFileList::OnKeyDown(uint32_t param1, uint32_t param2) {

}

void tFileList::OnClick(uint32_t param1, uint32_t param2) {
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy + 3 && param2 <= hy + 19) bUp->OnClick(param1, param2);
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy1 - 19 && param2 <= hy1 - 3) bDown->OnClick(param1, param2);
    
    int x, y, x1, y1;
    bMiddle->getPositionAndSize(&x, &y, &x1, &y1);
    if (param1 >= x && param1 <= x1 && param2 >= y && param2 <= y1) {
        mx = param1;
        my = param2;
        scrollPressed = true;
    }

    uint32_t cnt = 16;
    if (count < 16) cnt = count;
    for (int i = 0; i < cnt; i++) {
        if (param1 >= hx + 5 && param1 <= hx + 570 && param2 >= hy + (20 * i) + 25 && param2 <= hy + (20 * i) + 45) {
            if (i != curSelect) {
                 curSelect = i;
                selectedItem = startSelect + i;
                update();
             }
        }
    }
}

void tFileList::OnDblClick(uint32_t param1, uint32_t param2) {

}

void tFileList::OnClickUp(uint32_t param1, uint32_t param2) {
    
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy + 3 && param2 <= hy + 19) {
        bUp->OnClickUp(param1, param2);
        if (startSelect > 0) {
            startSelect--;
            scrollPosition -= scrollAdd;
            bMiddle->changePosition(hx1 - 19, (int)round(scrollPosition));
            update();
        }
    }
    if (param1 >= hx1 - 19 && param1 <= hx1 - 3 && param2 >= hy1 - 19 && param2 <= hy1 - 3) {
        bDown->OnClickUp(param1, param2);
        if (count > 16) {
            if (startSelect < (count - 16)) {
                startSelect++;
                scrollPosition += scrollAdd;
                bMiddle->changePosition(hx1 - 19, (int)round(scrollPosition));
                update();
            }
        }
    }
    scrollPressed = false;
    
}


void tFileList::OnMove(uint32_t param1, uint32_t param2) {
    if (scrollPressed) {
        if (param2 != my) {
            if (param2 > my) {
                float oldSP = scrollPosition;
                scrollPosition += (param2 - my);
                float step = (scrollPosition - oldSP) / scrollAdd;
                if (scrollPosition + ln <= hy1 - 20) {
                    //curMemAddr += (16 * (int)round(step));
                    startSelect += (int)round(step);
                }
                else {
                    //curMemAddr = maxMemAddr - 256;
                    startSelect = count - 16;
                    scrollPosition = hy1 - 20 - ln;
                }
                bMiddle->changePosition(hx1 - 19, scrollPosition);
                update();
            }
            else {
                float oldSP = scrollPosition;
                scrollPosition -= (my - param2);
                float step = (oldSP - scrollPosition) / scrollAdd;
                if (scrollPosition < hy + 20) {
                    //curMemAddr = 0;
                    startSelect = 0;
                    scrollPosition = hy + 20;
                }
                else {
                    //curMemAddr -= ((int)round(step) * 16);
                    startSelect -= (int)round(step);
                }
                bMiddle->changePosition(hx1 - 19, scrollPosition);
                update();
            }
            mx = param1;
            my = param2;
        }
    }
}


void tFileList::OnEndMove(uint32_t param1, uint32_t param2) {

}


void tFileList::Destroy() {

}

bool tFileList::getFocus() {
    return false;
}
