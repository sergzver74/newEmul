/** \addtogroup pur_module PUR
@{
*/
/** \addtogroup graph_module graph.cpp
@{
 * \ref graph.cpp\n
 * \ref graph.h\n
*/
/**
 * \file graph.cpp
 * @brief Модуль графики\n
 * @detailed Вывод графических примитивов используя sdl
 *
*/

#include <stdio.h>
#include <fcntl.h>
//#include <unistd.h>
#include <stdint.h>
//#include <sys/ioctl.h>
//#include <linux/fb.h>
//#include <sys/mman.h>
#include <stdlib.h>
#include <math.h>
#include "graph.h"

Graph::Graph(void* fbPointer, uint16_t width, uint16_t heigth, uint8_t bps) {
    if (!grinit)
    {
        vinfo.xres = width;
        vinfo.yres = heigth;
        vinfo.bits_per_pixel = bps;
        fbsize = vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8);
        fbbuf1 = (uint32_t*)fbPointer;

        /*
        putpixel = &putpixel32;
        getpixel = &getpixel32;
        clearScreen = &clearScreen32;
        RGBtoColor = &RGBtoColor32;
        ColortoRGB = ColortoRGB32;
        bar = &bar32;
        bargradient = &bargradient32;
        fillgradient = &fillgradient32;
        getImage = &getImage32;
        putImage = &putImage32;
        */

        pms = 0;
        pic = cBLACK;
        cur_color = cWHITE;
        fill_color = cWHITE;
        minx = 0;
        miny = 0;
        maxx = getmaxx() - 1;
        maxy = getmaxy() - 1;
        alpha = 0;
        clearScreen();
    }
};

Graph::~Graph() {
    if (grinit)
    {
        grinit = false;
    }
};

void Graph::moveAxis(int x, int y)
{
    if (x==0&&y==0)
    {
        maxis = false;
        movex = x;
        movey = y;
    } else
    {
        maxis = true;
        movex = x;
        movey = y;
    }
}

int Graph::getmaxx()
{
	return vinfo.xres;
}

int Graph::getmaxy()
{
	return vinfo.yres;
}

/*
unsigned int RGBtoColor16(unsigned int r, unsigned int g, unsigned int b)
{
    unsigned int col =0 ;
    r = r >> 3;
    r = r << 11;
    g = g >> 2;
    g = g << 5;
    b = b >> 3;
    col = col | r | g | b;
    return col;
}
*/

unsigned int Graph::RGBtoColor(unsigned int r, unsigned int g, unsigned int b)
{
    unsigned int col =0 ;
    r = r << 16;
    g = g << 8;
    col = col | r | g | b;
    return col;
}

/*
void ColortoRGB16(unsigned int col, unsigned int* r, unsigned int* g, unsigned int* b)
{
    *b = col & 0x001F;
    *b = *b << 3;
    *g = col & 0x07E0;
    *g = *g >> 3;
    *r = col & 0xF800;
    *r = *r >> 8;
}
*/

void Graph::ColortoRGB(unsigned int col, unsigned int* r, unsigned int* g, unsigned int* b)
{
    *b = col & 0x000000FF;
    *g = col & 0x0000FF00;
    *g = *g >> 8;
    *r = col & 0x00FF0000;
    *r = *r >> 16;
}

/*
unsigned int convertColor32to16(unsigned int c)
{
    unsigned int r,g,b;
    ColortoRGB32(c,&r,&g,&b);
    c = RGBtoColor(r,g,b);
    return c;
}
*/

void Graph::SetColor(unsigned int col)
{
    //cur_color = convertColor32to16(col);
    cur_color = col;
}

void Graph::SetFillColor(unsigned int col)
{
    //fill_color = convertColor32to16(col);
    fill_color = col;
}

unsigned int Graph::GetColor()
{
    return cur_color;
}

/*
void putpixel16(int x, int y, unsigned int col)
{
    if (maxis)
    {
        int p=x;
        x = 600 - y;
        y = p;
        x = x + movex;
        y = y + movey;
    }
    if (x>minx&&x<maxx&&y>miny&&y<maxy)
    {
        int addr = y*vinfo.xres+x;
        *(fbbuf+addr) = col;
    }
}
*/

void Graph::putpixel(int x, int y, unsigned int col)
{
    if (x>minx&&x<maxx&&y>miny&&y<maxy)
    {
        int addr = y*vinfo.xres+x;
        *(fbbuf1+addr) = col;
    }
}

/*
unsigned int getpixel16(int x, int y)
{
    if (maxis)
    {
        int p=x;
        x = 600 - y;
        y = p;
        x = x + movex;
        y = y + movey;
    }
    unsigned int col = 0;
    if (x>minx&&x<maxx&&y>miny&&y<maxy)
    {
        int addr = y*vinfo.xres+x;

        col = *(fbbuf+addr);
    }
    return col;
}
*/

unsigned int Graph::getpixel(int x, int y)
{
    unsigned int col = 0;
    if (x>minx&&x<maxx&&y>miny&&y<maxy)
    {
        int addr = y*vinfo.xres+x;

        col = *(fbbuf1+addr);
    }
    return col;
}

void Graph::line(int x1, int y1, int x2, int y2)
{
    if (y1==y2&&vinfo.bits_per_pixel==32)
    {
        if (y1>miny&&y2<maxy)
        {
            if (x1<minx) x1=minx;
            if (x2>maxx) x2=maxx;
            int addr = y1*vinfo.xres+x1;
            int cnt = x2-x1+1;
            int endaddr = addr+cnt;
            while (addr<endaddr)
            {
                *(fbbuf1+addr) = cur_color;
                addr++;
            }
        }
    } else
        if (y1==y2&&vinfo.bits_per_pixel==16)
        {
            if (maxis)
            {
                int p=x1;
                x1 = 600 - y1;
                y1 = p;
                p=x2;
                x2 = 600 - y2;
                y2 = p;
                x1 = x1 + movex;
                y1 = y1 + movey;
                x2 = x2 + movex;
                y2 = y2 + movey;
                if (x1>x2)
                {
                    p = x1;
                    x1 = x2;
                    x2 = p;
                }
                if (y1>y2)
                {
                    p = y1;
                    y1 = y2;
                    y2 = p;
                }
            }
            if (y1>miny&&y2<maxy)
            {
                if (x1<minx) x1=minx;
                if (x2>maxx) x2=maxx;
                unsigned int *newFb;
                newFb = (unsigned int*) fbbuf;
                char fl1 = x1 & 1;
                int xr = vinfo.xres >> 1;
                int addr = y1*xr+(x1 >> 1);
                int cnt = x2-x1+1;
                char fl = cnt & 1;
                cnt = cnt >> 1;
                int cl = (cur_color << 16) + cur_color;
                int endaddr = addr+cnt;
                if (fl1)
                {
                    *(newFb+addr) = (*(newFb+addr)&0xFFFF)+(cur_color<<16);
                    addr++;
                }
                while (addr<endaddr)
                {
                    *(newFb+addr) = cl;
                    addr++;
                }
                if (fl)
                {
                    *(newFb+addr) = (*(newFb+addr)&0xFFFF0000)+cur_color;
                }
            }
        } else
            if (x1==x2&&vinfo.bits_per_pixel==32)
            {
                if (x1>minx&&x2<maxx)
                {
                    if (y1<miny) y1=miny;
                    if (y2>maxy) y2=maxy;
                    int addr = y1*vinfo.xres+x1;
                    int cnt = y2-y1+1;
                    int endaddr = addr+cnt*vinfo.xres;
                    while (addr<endaddr)
                    {
                        *(fbbuf1+addr) = cur_color;
                        addr += vinfo.xres;
                    }
                }
            } else
                if (x1==x2&&vinfo.bits_per_pixel==16)
                {
                    if (maxis)
                    {
                        int p=x1;
                        x1 = 600 - y1;
                        y1 = p;
                        p=x2;
                        x2 = 600 - y2;
                        y2 = p;
                        x1 = x1 + movex;
                        y1 = y1 + movey;
                        x2 = x2 + movex;
                        y2 = y2 + movey;
                        if (x1>x2)
                        {
                            p = x1;
                            x1 = x2;
                            x2 = p;
                        }
                        if (y1>y2)
                        {
                            p = y1;
                            y1 = y2;
                            y2 = p;
                        }
                    }
                    if (x1>minx&&x2<maxx)
                    {
                        if (y1<miny) y1=miny;
                        if (y2>maxy) y2=maxy;
                        int addr = y1*vinfo.xres+x1;
                        int cnt = y2-y1+1;
                        int endaddr = addr+cnt*vinfo.xres;
                        while (addr<endaddr)
                        {
                            *(fbbuf+addr) = cur_color;
                            addr += vinfo.xres;
                        }
                    }
                } else
                {
                    const int deltaX = abs(x2 - x1);
                    const int deltaY = abs(y2 - y1);
                    const int signX = x1 < x2 ? 1 : -1;
                    const int signY = y1 < y2 ? 1 : -1;
                    int error = deltaX - deltaY;
                    putpixel(x2, y2, cur_color);
                    while(x1 != x2 || y1 != y2) {
                        putpixel(x1, y1, cur_color);
                        const int error2 = error * 2;
                        if(error2 > -deltaY) {
                            error -= deltaY;
                            x1 += signX;
                        }
                        if(error2 < deltaX) {
                            error += deltaX;
                            y1 += signY;
                        }
                    }
                }
}

void Graph::rectangle(int x1, int y1, int x2, int y2)
{
    line(x1,y1,x2,y1);
    line(x2,y1,x2,y2);
    line(x1,y1,x1,y2);
    line(x1,y2,x2,y2);
}

/*
void bar16(int x1, int y1, int x2, int y2)
{
    if (maxis)
    {
        int p=x1;
        x1 = 600 - y1;
        y1 = p;
        p=x2;
        x2 = 600 - y2;
        y2 = p;
        x1 = x1 + movex;
        y1 = y1 + movey;
        x2 = x2 + movex;
        y2 = y2 + movey;
        if (x1>x2)
        {
            p = x1;
            x1 = x2;
            x2 = p;
        }
        if (y1>y2)
        {
            p = y1;
            y1 = y2;
            y2 = p;
        }
    }
    unsigned int *newFb;
    newFb = (unsigned int*) fbbuf;
    int j,addr,cnt,addrs,endaddr;
    char fl1 = x1 & 1;
    int xr = vinfo.xres >> 1;
    addr = y1*xr+(x1 >> 1);
    cnt = x2-x1+1;
    char fl = cnt & 1;
    cnt = cnt >> 1;
    addrs = addr;
    endaddr = addrs + cnt;
    int cl = (fill_color << 16) + fill_color;
    for (j=y1;j<=y2;j++)
    {
        if (fl1)
        {
            *(newFb+addr) = (*(newFb+addr)&0xFFFF)+(fill_color<<16);
            addr++;
        }
        while (addr<endaddr)
        {
            *(newFb+addr) = cl;
            addr++;
        }
        if (fl)
        {
            *(newFb+addr) = (*(newFb+addr)&0xFFFF0000)+fill_color;
        }
        addrs += xr;
        addr = addrs;
        endaddr = addrs + cnt;
    }
}
*/

void Graph::bar(int x1, int y1, int x2, int y2)
{
    int j,addr,cnt,addrs,endaddr;
    addr = y1*vinfo.xres+x1;
    cnt = x2-x1+1;
    addrs = addr;
    endaddr = addrs + cnt;
    for (j=y1;j<=y2;j++)
    {
        while (addr<endaddr)
        {
            *(fbbuf1+addr) = fill_color;
            addr++;
        }
        addrs += vinfo.xres;
        addr = addrs;
        endaddr = addrs + cnt;
    }
}

void Graph::Circle(int x0, int y0, int radius)
{
	int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while(y >= 0) {
		putpixel(x0 + x, y0 + y,cur_color);   // right down
		putpixel(x0 + x, y0 - y,cur_color);   // right up
		putpixel(x0 - x, y0 + y,cur_color);   // left down
		putpixel(x0 - x, y0 - y,cur_color);   // left up
		error = 2 * (delta + y) - 1;
		if(delta < 0 && error <= 0) {
			++x;
			delta += 2 * x + 1;
			continue;
		}
		error = 2 * (delta - x) - 1;
		if(delta > 0 && error > 0) {
			--y;
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
	}
}

void Graph::arc(int x0, int y0, int radius, int part)
{
	int x = 0;
	int y = radius;
	int delta = 1 - 2 * radius;
	int error = 0;
	while(y >= 0) {
		switch (part)
		{
            case arcRIGHTDOWN: putpixel(x0 + x, y0 + y,cur_color);   // right down
                break;
            case arcRIGHTUP:   putpixel(x0 + x, y0 - y,cur_color);   // right up
                break;
            case arcLEFTDOWN:  putpixel(x0 - x, y0 + y,cur_color);   // left down
                break;
            case arcLEFTUP:    putpixel(x0 - x, y0 - y,cur_color);   // left up
                break;
            default:
                break;
        }
		error = 2 * (delta + y) - 1;
		if(delta < 0 && error <= 0)
		{
			++x;
			delta += 2 * x + 1;
			continue;
		}
		error = 2 * (delta - x) - 1;
		if(delta > 0 && error > 0)
		{
			--y;
			delta += 1 - 2 * y;
			continue;
		}
		++x;
		delta += 2 * (x - y);
		--y;
	}
}

/*
void bargradient16(int x1, int y1, int x2, int y2, int orient, unsigned int col1, unsigned int col2)
{
    unsigned int pColor,r1,g1,b1,r2,g2,b2;
    float dr,dg,db,drx,dry,dgx,dgy,dbx,dby,r,g,b;
    int dx,dy;
    col1 = convertColor32to16(col1);
    col2 = convertColor32to16(col2);
    if (maxis)
    {
        int p=x1;
        x1 = 600 - y1;
        y1 = p;
        p=x2;
        x2 = 600 - y2;
        y2 = p;
        x1 = x1 + movex;
        y1 = y1 + movey;
        x2 = x2 + movex;
        y2 = y2 + movey;
        if (x1>x2)
        {
            p = x1;
            x1 = x2;
            x2 = p;
        }
        if (y1>y2)
        {
            p = y1;
            y1 = y2;
            y2 = p;
        }
    }
    dx = x2 - x1;
    dy = y2 - y1;
    ColortoRGB(col1,&r1,&g1,&b1);
    ColortoRGB(col2,&r2,&g2,&b2);
    r = (float)r1;
    g = (float)g1;
    b = (float)b1;
    dr = (float)r2-(float)r1;
    dg = (float)g2-(float)g1;
    db = (float)b2-(float)b1;
    if (orient==oHORIZONTAL)
    {
        dry = dr / (float)dy;
        dgy = dg / (float)dy;
        dby = db / (float)dy;
        int i,j,addr;
        for (j=y1;j<=y2;j++)
        {
            r = r + dry;
            g = g + dgy;
            b = b + dby;
            pColor = RGBtoColor(r,g,b);
            for(i=x1;i<=x2;i++)
            {
                addr = j*vinfo.xres+i;
                *(fbbuf+addr) = pColor;
            }
        }
    } else
    {
        drx = dr / (float)dx;
        dgx = dg / (float)dx;
        dbx = db / (float)dx;
        int i,j,addr;
        for (j=x1;j<=x2;j++)
        {
            r = r + drx;
            g = g + dgx;
            b = b + dbx;
            pColor = RGBtoColor(r,g,b);
            for(i=y1;i<=y2;i++)
            {
                addr = i*vinfo.xres+j;
                *(fbbuf+addr) = pColor;
            }
        }
    }
}
*/

void Graph::bargradient(int x1, int y1, int x2, int y2, int orient, unsigned int col1, unsigned int col2)
{
    unsigned int pColor,r1,g1,b1,r2,g2,b2;
    float dr,dg,db,drx,dry,dgx,dgy,dbx,dby,r,g,b;
    int dx,dy;
    dx = x2 - x1;
    dy = y2 - y1;
    ColortoRGB(col1,&r1,&g1,&b1);
    ColortoRGB(col2,&r2,&g2,&b2);
    r = (float)r1;
    g = (float)g1;
    b = (float)b1;
    dr = (float)r2-(float)r1;
    dg = (float)g2-(float)g1;
    db = (float)b2-(float)b1;
    if (orient==oVERTICAL)
    {
        dry = dr / (float)dy;
        dgy = dg / (float)dy;
        dby = db / (float)dy;
        int i,j,addr;
        for (j=y1;j<=y2;j++)
        {
            r = r + dry;
            g = g + dgy;
            b = b + dby;
            pColor = RGBtoColor(r,g,b);
            for(i=x1;i<=x2;i++)
            {
                addr = j*vinfo.xres+i;
                *(fbbuf1+addr) = pColor;
            }
        }
    } else
    {
        drx = dr / (float)dx;
        dgx = dg / (float)dx;
        dbx = db / (float)dx;
        int i,j,addr;
        for (j=x1;j<=x2;j++)
        {
            r = r + drx;
            g = g + dgx;
            b = b + dbx;
            pColor = RGBtoColor(r,g,b);
            for(i=y1;i<=y2;i++)
            {
                addr = i*vinfo.xres+j;
                *(fbbuf1+addr) = pColor;
            }
        }
    }
}

/*
void fillgradient16(int xn,int yn, int x, int y, int x1, int y1, int orient, unsigned int col1, unsigned int col2)
{
    col1 = convertColor32to16(col1);
    col2 = convertColor32to16(col2);
	unsigned int tempcolor;
	tempcolor = GetColor();
	int i,xpn,xpk,ypn,ypk,dy,dx;
	unsigned int r2,g2,b2,r1,g1,b1,pColor;
	float dr,dg,db,r,g,b,dry,dgy,dby,drx,dgx,dbx;
	ColortoRGB(col1,&r1,&g1,&b1);
	ColortoRGB(col2,&r2,&g2,&b2);
	dx = x1-x;
	dy = y1-y;
	dr = (float)r2-(float)r1;
	dg = (float)g2-(float)g1;
	db = (float)b2-(float)b1;
	r = (float)r1;
	g = (float)g1;
	b = (float)b1;
	switch (orient)
	{
		case oVERTICAL:
		  dry = dr / (float)dy;
		  dgy = dg / (float)dy;
		  dby = db / (float)dy;
		  for (i=yn;i<y1;i++)
		  {
			  xpn = xn;
			  while (getpixel(xpn,i)!=tempcolor) {
				  xpn--;
				  if (xpn<=minx) break;
			  };
			  xpk = xn;
			  while (getpixel(xpk,i)!=tempcolor) {
				  xpk++;
				  if (xpk>=maxx) break;
			  };
			  r = r + dry;
			  g = g + dgy;
			  b = b + dby;
			  pColor = RGBtoColor(r,g,b);
			  SetColor(pColor);
			  line(xpn+1,i,xpk-1,i);
		  }
		  break;
		case oHORIZONTAL:
		  drx = dr / (float)dx;
		  dgx = dg / (float)dx;
		  dbx = db / (float)dx;
		  for (i=xn;i<x1;i++)
		  {
			  ypn = yn;
			  while (getpixel(i,ypn)!=tempcolor) {
				  ypn--;
				  if (ypn<=miny) break;
			  };
			  ypk = yn;
			  while (getpixel(i,ypk)!=tempcolor) {
				  ypk++;
				  if (ypk>=maxy) break;
			  };
			  r = r + drx;
			  g = g + dgx;
			  b = b + dbx;
			  pColor = RGBtoColor(r,g,b);
			  SetColor(pColor);
			  line(i,ypn+1,i,ypk-1);
		  }
		  break;
		default:
		  break;
	};
	SetColor(tempcolor);
}
*/

void Graph::fillgradient(int xn,int yn, int x, int y, int x1, int y1, int orient, unsigned int col1, unsigned int col2)
{
	unsigned int tempcolor;
	tempcolor = GetColor();
	int i,xpn,xpk,ypn,ypk,dy,dx;
	unsigned int r2,g2,b2,r1,g1,b1,pColor;
	float dr,dg,db,r,g,b,dry,dgy,dby,drx,dgx,dbx;
	ColortoRGB(col1,&r1,&g1,&b1);
	ColortoRGB(col2,&r2,&g2,&b2);
	dx = x1-x;
	dy = y1-y;
	dr = (float)r2-(float)r1;
	dg = (float)g2-(float)g1;
	db = (float)b2-(float)b1;
	r = (float)r1;
	g = (float)g1;
	b = (float)b1;
	switch (orient)
	{
		case oVERTICAL:
		  dry = dr / (float)dy;
		  dgy = dg / (float)dy;
		  dby = db / (float)dy;
		  for (i=yn;i<y1;i++)
		  {
			  xpn = xn;
			  while (getpixel(xpn,i)!=tempcolor) {
				  xpn--;
			  };
			  xpk = xn;
			  while (getpixel(xpk,i)!=tempcolor) {
				  xpk++;
			  };
			  r = r + dry;
			  g = g + dgy;
			  b = b + dby;
			  pColor = RGBtoColor(r,g,b);
			  SetColor(pColor);
			  line(xpn+1,i,xpk-1,i);
		  }
		  break;
		case oHORIZONTAL:
		  drx = dr / (float)dx;
		  dgx = dg / (float)dx;
		  dbx = db / (float)dx;
		  for (i=xn;i<x1;i++)
		  {
			  ypn = yn;
			  while (getpixel(i,ypn)!=tempcolor) {
				  ypn--;
			  };
			  ypk = yn;
			  while (getpixel(i,ypk)!=tempcolor) {
				  ypk++;
			  };
			  r = r + drx;
			  g = g + dgx;
			  b = b + dbx;
			  pColor = RGBtoColor(r,g,b);
			  SetColor(pColor);
			  line(i,ypn+1,i,ypk-1);
		  }
		  break;
		default:
		  break;
	};
	SetColor(tempcolor);
}

void Graph::roundrect(int x, int y, int x1, int y1, int radius)
{
	line(x+radius,y,x1-radius,y);
	line(x,y+radius,x,y1-radius);
	line(x1,y+radius,x1,y1-radius);
	line(x+radius,y1,x1-radius,y1);
	arc(x+radius+1,y+radius,radius,arcLEFTUP);
	arc(x1-radius-1,y+radius,radius,arcRIGHTUP);
	arc(x+radius+1,y1-radius,radius,arcLEFTDOWN);
	arc(x1-radius-1,y1-radius,radius,arcRIGHTDOWN);
}

void Graph::roundrectgradient(int x, int y, int x1, int y1, int radius, int orient, unsigned int col1, unsigned int col2)
{
	roundrect(x,y,x1,y1,radius);
	switch (orient)
	{
        case oVERTICAL: fillgradient(x+radius+1,y+1,x,y,x1,y1,orient,col1,col2);
            break;
        case oHORIZONTAL: fillgradient(x+1,y+radius,x,y,x1,y1,orient,col1,col2);
            break;
        default:
            break;
	};
}

void Graph::circlegradient(int x, int y, int radius, int orient, unsigned int col1, unsigned int col2)
{
	Circle(x,y,radius);
	switch (orient)
	{
        case oVERTICAL: fillgradient(x,y-radius+1,x-radius,y-radius,x+radius,y+radius,orient,col1,col2);
            break;
        case oHORIZONTAL: fillgradient(x-radius+1,y,x-radius-1,y-radius,x+radius+1,y+radius, orient,col1,col2);
            break;
        default:
            break;
	};
}

void Graph::setAlpha(char a)
{
    alpha = a % 100;
}

void Graph::setPutImageStyle(int type, unsigned int col)
{
    pms = type;
    pic = col;
}

/*
unsigned int* getImage16(int x1,int y1,int x2,int y2)
{
    if (x1%2==1) x1--;
    if (maxis)
    {
        int p=x1;
        x1 = 600 - y1;
        y1 = p;
        p=x2;
        x2 = 600 - y2;
        y2 = p;
        x1 = x1 + movex;
        y1 = y1 + movey;
        x2 = x2 + movex;
        y2 = y2 + movey;
        if (x1>x2)
        {
            p = x1;
            x1 = x2;
            x2 = p;
        }
        if (y1>y2)
        {
            p = y1;
            y1 = y2;
            y2 = p;
        }
    }
    unsigned int *imb = NULL;
    unsigned int *newFb;
    newFb = (unsigned int*) fbbuf;
    int imageSize = ((x2-x1+1)*(y2-y1+1))*4+8;
    imb = (unsigned int*) malloc(imageSize);
    if (imb==NULL)
    {
        printf("do not allocated memory. Aborted.\n");exit(1);
    }
    int j,addr,addr1,cnt,addrs,endaddr;
    imb[0] = ((y2-y1+1)<<16)+(x2-x1+1);
    addr1 = 1;
    if (x2 > maxx) x2 = maxx;
    if (y2 > maxy) y2 = maxy;
    int xr = vinfo.xres >> 1;
    addr = y1*xr+(x1 >> 1);
    cnt = x2-x1+1;
    int fl = cnt & 1;
    cnt = cnt >> 1;
    addrs = addr;
    endaddr = addrs + cnt;
    for (j=y1;j<=y2;j++)
    {
        while (addr<endaddr)
        {
            *(imb+addr1) = *(newFb+addr);
            addr1++;
            addr++;
        }
        if (fl)
        {
            *(imb+addr1) = *(newFb+addr)&0xFFFF;
            addr1++;
        }
        addrs += xr;
        addr = addrs;
        endaddr = addrs + cnt;
    }
    return imb;
}
*/

unsigned int* Graph::getImage(int x1,int y1,int x2,int y2)
{
    unsigned int *imb = NULL;
    int imageSize = ((x2-x1+1)*(y2-y1+1))*4+8;
    imb = (unsigned int*) calloc(imageSize,sizeof(unsigned char));
    if (imb==NULL)
    {
        printf("do not allocated memory. Aborted.\n");exit(1);
    }
    int j,addr,addr1,cnt,addrs,endaddr;
    addr1 = 0;
    imb[addr1] = x2-x1+1;
    addr1++;
    imb[addr1] = y2-y1+1;
    addr1++;
    if (x2 > maxx) x2 = maxx;
    if (y2 > maxy) y2 = maxy;
    addr = y1*vinfo.xres+x1;
    cnt = x2-x1+1;
    addrs = addr;
    endaddr = addrs + cnt;
    for (j=y1;j<=y2;j++)
    {
        while (addr<endaddr)
        {
            *(imb+addr1) = *(fbbuf1+addr);
            addr1++;
            addr++;
        }
        addrs += vinfo.xres;
        addr = addrs;
        endaddr = addrs + cnt;
    }
    return imb;
}

/*
void putImage16(int x1, int y1, unsigned int* imb)
{
    if (x1%2==1) x1--;
    if (maxis)
    {
        int p=x1;
        x1 = 600 - y1;
        y1 = p;
        x1 = x1 + movex;
        y1 = y1 + movey;
    }
    int j,addr,addr1,dx,dy,y2,cnt,addrs,endaddr;
    unsigned int cl;
    unsigned int *newFb;
    newFb = (unsigned int*) fbbuf;
    dx = imb[0]&0xFFFF;
    dy = imb[0] >> 16;
    addr1 = 1;
    if (x1+dx>maxx+1) dx = maxx - x1;
    if (y1+dy>maxy+1) dy = maxy - y1;
    int xr = vinfo.xres >> 1;
    addr = y1*xr+(x1 >> 1);
    int fl = dx & 1;
    cnt = dx >> 1;
    addrs = addr;
    endaddr = addrs + cnt;
    y2 = y1+dy-1;
    if (alpha==0)
    {
        if (pms==1)
        {
            for (j=y1;j<=y2;j++)
            {
                while (addr<endaddr)
                {
                    if (*(imb+addr1)!=pic)
                    {
                        *(newFb+addr) = *(imb+addr1);
                    }
                    addr1++;
                    addr++;
                }
                if (fl)
                {
                    if (*(imb+addr1)!=pic)
                    {
                        int pr = *(newFb+addr)&0xFFFF0000;
                        *(newFb+addr) = pr + (*(imb+addr1)&0xFFFF);
                    }
                    addr1++;
                }
                addrs += xr;
                addr = addrs;
                endaddr = addrs + cnt;
            }
        } else
        for (j=y1;j<=y2;j++)
        {
            while (addr<endaddr)
            {
                *(newFb+addr) = *(imb+addr1);
                addr1++;
                addr++;
            }
            if (fl)
            {
                int pr = *(newFb+addr)&0xFFFF0000;
                *(newFb+addr) = pr + (*(imb+addr1)&0xFFFF);
                addr1++;
            }
            addrs += xr;
            addr = addrs;
            endaddr = addrs + cnt;
        }
    } else
    {
        float al,al1;
        al = alpha;
        al = al / 100;
        al1 = 1-al;
        for (j=y1;j<=y2;j++)
        {
            while (addr<endaddr)
            {
                int ccc =*(imb+addr1);
                int ccc1 = *(newFb+addr);
                int ccr;
                cl = ccc&0xFFFF;
                unsigned int r1,g1,b1;
                ColortoRGB(cl,&r1,&g1,&b1);
                unsigned int cl1;
                cl1 = ccc1&0xFFFF;
                unsigned int r2,g2,b2;
                ColortoRGB(cl1,&r2,&g2,&b2);
                float r, g, b;
                r = r1*al1+r2*al;
                g = g1*al1+g2*al;
                b = b1*al1+b2*al;
                ccr = RGBtoColor(r,g,b);
                cl = ccc&0x0000FFFF;
                ColortoRGB(cl,&r1,&g1,&b1);
                cl1 = ccc1&0x0000FFFF;
                ColortoRGB(cl1,&r2,&g2,&b2);
                r = r1*al1+r2*al;
                g = g1*al1+g2*al;
                b = b1*al1+b2*al;
                RGBtoColor(r,g,b);
                *(newFb+addr) =((RGBtoColor(r,g,b))<<16) + ccr;
                addr1++;
                addr++;
            }
            if (fl)
            {
                int ccc =*(imb+addr1);
                int ccc1 = *(newFb+addr);
                //int ccr;
                cl = ccc&0xFFFF;
                unsigned int r1,g1,b1;
                ColortoRGB(cl,&r1,&g1,&b1);
                unsigned int cl1;
                cl1 = ccc1&0xFFFF;
                unsigned int r2,g2,b2;
                ColortoRGB(cl1,&r2,&g2,&b2);
                float r, g, b;
                r = r1*al1+r2*al;
                g = g1*al1+g2*al;
                b = b1*al1+b2*al;
                //ccr = RGBtoColor(r,g,b);
                *(newFb+addr) = (ccc1&0x0000FFFF) + RGBtoColor(r,g,b);
                addr1++;
            }
            addrs += xr;
            addr = addrs;
            endaddr = addrs + cnt;
        }
    }
}
*/

void Graph::putImage(int x1, int y1, unsigned int* imb)
{
    int j,addr,addr1,dx,dy,y2,cnt,addrs,endaddr;
    unsigned int cl;
    addr1 = 0;
    dx = imb[addr1];
    addr1++;
    dy = imb[addr1];
    addr1++;
    if (x1+dx>maxx+1) dx = maxx - x1;
    if (y1+dy>maxy+1) dy = maxy - y1;
    addr = y1*vinfo.xres+x1;
    cnt = dx+1;
    cnt = dx;
    addrs = addr;
    endaddr = addrs + cnt;
    y2 = y1+dy-1;
    if (alpha==0)
    {
        if (pms==1)
        {
            for (j=y1;j<=y2;j++)
            {
                while (addr<endaddr)
                {
                    if (*(imb+addr1)!=pic)
                    {
                        *(fbbuf1+addr) = *(imb+addr1);
                    }
                    addr1++;
                    addr++;
                }
                addrs += vinfo.xres;
                addr = addrs;
                endaddr = addrs + cnt;
            }
        } else
        for (j=y1;j<=y2;j++)
        {
            while (addr<endaddr)
            {
                *(fbbuf1+addr) = *(imb+addr1);
                addr1++;
                addr++;
            }
            addrs += vinfo.xres;
            addr = addrs;
            endaddr = addrs + cnt;
        }
    } else
    {
        float al,al1;
        al = alpha;
        al = al / 100;
        al1 = 1-al;
        for (j=y1;j<=y2;j++)
        {
            while (addr<endaddr)
            {
                cl = *(imb+addr1);
                unsigned int r1,g1,b1;
                ColortoRGB(cl,&r1,&g1,&b1);
                unsigned int cl1;
                cl1 = *(fbbuf1+addr);
                unsigned int r2,g2,b2;
                ColortoRGB(cl1,&r2,&g2,&b2);
                float r, g, b;
                r = r1*al1+r2*al;
                g = g1*al1+g2*al;
                b = b1*al1+b2*al;
                cl = RGBtoColor(r,g,b);
                *(fbbuf1+addr) = cl;
                addr1++;
                addr++;
            }
            addrs += vinfo.xres;
            addr = addrs;
            endaddr = addrs + cnt;
        }
    }
}

void Graph::freeImage(unsigned int *imb)
{
    free(imb);
}

/*
void clearScreen16()
{
    int i;
    int fbsize = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/8);
    for (i=0; i<(fbsize /2) ; i++)
    {
        *(fbbuf+i) = 0x0;
    }
}
*/

void Graph::clearScreen()
{
    int i;
    int fbsize = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/8);
    for (i=0; i<(fbsize /4) ; i++)
    {
        *(fbbuf1+i) = 0x0;
    }
}

void Graph::setOutputContext(int x,int y,int x1,int y1)
{
    if (maxis)
    {
        x1 = x1 + movex;
        y1 = y1 + movey;
        x = x + movex;
        y = y + movey;
    }
    if (x<0)
        {x =0; }
    if (x1>=getmaxx())
        {x1 = getmaxx()-1;}
    if (y<0)
        {y =0; }
    if (y1>=getmaxy())
        {y1 = getmaxy()-1;}
    minx = x;
    miny = y;
    maxx = x1;
    maxy = y1;
}

void Graph::restoreDefaultContext()
{
    setOutputContext(0,0,getmaxx()-1,getmaxy()-1);
}

void Graph::getOutputContext(int *x, int *y, int *mx, int *my)
{
    *x = minx;
    *y = miny;
    *mx = maxx;
    *my = maxy;
}

void Graph::setVirtualScreen(unsigned short *scr, int width, int height)
{
    if (!fbSave.saved)
    {
        fbSave.fbSave = fbbuf;
        fbSave.xres = vinfo.xres;
        fbSave.yres = vinfo.yres;
        fbSave.maxx = maxx;
        fbSave.maxy = maxy;
        fbSave.minx = minx;
        fbSave.miny = miny;
        fbSave.saved = 1;
    }
    fbbuf = scr+2;
    vinfo.xres = width;
    vinfo.yres = height;
    minx = 0;
    miny = 0;
    maxx = width - 1;
    maxy = height - 1;
}

void Graph::restoreOriginalScreen()
{
    if (fbSave.saved)
    {
        fbbuf = fbSave.fbSave;
        vinfo.xres = fbSave.xres;
        vinfo.yres = fbSave.yres;
        maxx = fbSave.maxx;
        maxy = fbSave.maxy;
        minx = fbSave.minx;
        miny = fbSave.miny;
        fbSave.saved = 0;
    }
}

/*
int Graph::initgr(void* fbPointer, uint16_t width, uint16_t heigth, uint8_t bps)
{
    if (!grinit)
    {
        vinfo.xres = width;
        vinfo.yres = heigth;
        vinfo.bits_per_pixel = bps;
        fbsize = vinfo.xres * vinfo.yres * (vinfo.bits_per_pixel / 8);
        fbbuf1 = (uint32_t*)fbPointer;

        putpixel = &putpixel32;
        getpixel = &getpixel32;
        clearScreen = &clearScreen32;
        RGBtoColor = &RGBtoColor32;
        ColortoRGB = ColortoRGB32;
        bar = &bar32;
        bargradient = &bargradient32;
        fillgradient = &fillgradient32;
        getImage = &getImage32;
        putImage = &putImage32;

        pms = 0;
        pic = cBLACK;
        cur_color = cWHITE;
        fill_color = cWHITE;
        minx = 0;
        miny = 0;
        maxx = getmaxx() - 1;
        maxy = getmaxy() - 1;
        alpha = 0;
        clearScreen();
        return 0;
    }
    return 4;
}
/*
/*
int initgr(unsigned char n)
{
    if (!grinit)
    {
        char fbname[10];
        if (n>3) n=0;
        if  (n==0) sprintf(fbname,"%s","/dev/fb0");
        if  (n==1) sprintf(fbname,"%s","/dev/fb1");
        if  (n==2) sprintf(fbname,"%s","/dev/fb2");
        if  (n==3) sprintf(fbname,"%s","/dev/fb3");
        if ((fbfd0 = open(fbname, O_RDWR)) < 0)
        {
            return 1;
        }
        if (ioctl(fbfd0, FBIOGET_VSCREENINFO, &vinfo))
        {
            printf("Bad vscreeninfo ioctl\n");
            return 2;
        }
        fbsize = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/8);
        if (vinfo.bits_per_pixel==16)
        {
            if ((fbbuf = static_cast<unsigned short*>(mmap(0, fbsize, PROT_READ|PROT_WRITE,MAP_SHARED, fbfd0, 0))) == (void *) -1)
            {
                return 3;
            }
            putpixel = &putpixel16;
            getpixel = &getpixel16;
            clearScreen = &clearScreen16;
            RGBtoColor = &RGBtoColor16;
            ColortoRGB = ColortoRGB16;
            bar = &bar16;
            bargradient = &bargradient16;
            fillgradient = &fillgradient16;
            getImage = &getImage16;
            putImage = &putImage16;
            if (n==2) moveAxis(423, 276); else moveAxis(0,0);
        } else
            if (vinfo.bits_per_pixel==32)
            {
                if ((fbbuf1 = static_cast<unsigned int*>(mmap(0, fbsize, PROT_READ|PROT_WRITE,MAP_SHARED, fbfd0, 0))) == (void *) -1)
                {
                    return 3;
                }
                putpixel = &putpixel32;
                getpixel = &getpixel32;
                clearScreen = &clearScreen32;
                RGBtoColor = &RGBtoColor32;
                ColortoRGB = ColortoRGB32;
                bar = &bar32;
                bargradient = &bargradient32;
                fillgradient = &fillgradient32;
                getImage = &getImage32;
                putImage = &putImage32;
            }
        pms = 0;
        pic = cBLACK;
        cur_color = cWHITE;
        fill_color = cWHITE;
        minx = 0;
        miny = 0;
        maxx = getmaxx()-1;
        maxy = getmaxy()-1;
        alpha = 0;
        clearScreen();
        return 0;
    }
    return 4;
}
*/

/*
void Graph::closegr()
{
    if (grinit)
    {
        //munmap(fbbuf, fbsize);
        //close(fbfd0);
        grinit = false;
    }
}
*/
/*@}*/ /* end of group graph_module */
/*@}*/ /* end of group pur_module */
