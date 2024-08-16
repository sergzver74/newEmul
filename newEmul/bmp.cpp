/** \addtogroup pur_module PUR
@{
*/
/** \addtogroup bmp_module bmp.cpp
@{
 * \ref bmp.cpp\n
 * \ref bmp.h\n
*/
/**
 @file bmp.cpp
 @brief Модуль BMP\n
 @detailed Используется для вывода изображений находящихся в .BMP файлах
*/

#include <stdio.h>
#include <stdlib.h>
//#include "fbgpu.h"
#include <stdint.h>
#include <math.h>
#include "bmp.h"

# define M_PI           3.14159265358979323846  /* pi */

#define Sigma 10.0
#define Sigma_2D 5.0
// Cosin component
#define Lambda 20
#define Lambda_2D 20
// Aspect ratio
#define Gamma 0.5
// Direction
#define Direction 8
// Coordinate width for gabor
#define K1 3
#define K2 3
#define K1_1D 25
#define K1_2D 25
#define K2_2D 25


/**
@struct palette
@brief Структура палитры.
*/
struct palette {
	unsigned char B;    ///< компонента синего цвета
	unsigned char G;    ///< компонента зеленого цвета
	unsigned char R;    ///< компонента красного цвета
	unsigned char fil;  ///< зарезервиравано и не используется
};

/**
@struct bmpheader
@brief Структура заголовка .bmp файла.
*/
#pragma pack(push,1)
struct bmpheader {
	char id1;                       ///< Первая часть сигнатуры "B"
	char id2;                       ///< Вторая часть сигнатуры "M"
	int FileSize;                   ///< размер файла
	int Reserved;                   ///< зарезервировано
	int DataOffset;                 ///< смещение данных для постоения изображения относительно начала файла
	int HeaderSize;                 ///< размер заголовка
	int Width;                      ///< ширина изображения
	int Height;                     ///< высота изображения
	unsigned short Planes;          ///< Число плоскостей, должно быть 1
	unsigned short BitsPerPixel;    ///< количество бит на обну точку изображения
	int Compression;                ///< тип сжатия
	int DataSize;                   ///< размер данных
	int HResolution;                ///< разрешение по горизонтали, пиксел/м
	int VResolution;                ///< разрешение по вертикали, пиксел/м
	int Colors;                     ///< Количество используемых цветов
	int ImColors;                   ///< Количество "важных" цветов.
	palette pal[256];               ///< Палитра, для менее, чем 8-битного изображения.
};
#pragma pack(pop)

double gabor_2d(int x, int y, int Deg)
{
  double X, Y;
  double Rad = Deg * M_PI / 180;
  double pos_psi = 0;
  // Phase offset
  double psi = 2 * M_PI * pos_psi / 180;
  X = (x * cos(Rad)) + (y * sin(Rad));
  Y = (-(x * sin(Rad)) + (y * cos(Rad)));
  return (exp(-(pow(X, 2) + pow(Gamma * Y, 2)) / (2.0 * pow(Sigma_2D, 2))) * sin((2.0 * M_PI / Lambda_2D * X) + psi)) / 20;
}

void tBitmap::imgMean(int size)
{
	unsigned int x,y,a,c;
    int p,q,s;
    unsigned char *tmp;
    tmp = new unsigned char [wid*hei];
    for (unsigned int i=0;i<wid*hei;i++) *(tmp+i) = *(pimage+i);

    s = size / 2;
    a = size * size;

    for( y = s; y < hei - s; y++ )
    {
		for( x = s; x < wid - s; x++ )
		{
            c = 0;
            for( q = -s; q <= s; q++ )
            {
                for( p = -s; p <= s; p++ )
                {
                    c += *(tmp+(y+q)*wid+(x+p));
                }
            }
            *(pimage+(y*wid)+x ) = c / a;
		}
    }
    delete [] tmp;
}




tBitmap::tBitmap()
{
    left = 0;
    top = 0;
    wid = 0;
    hei = 0;
    al = 0xFF;
    vis = false;
    pimage = NULL;
    colorKey = 0;
    colorKeyPresent = false;
    image = NULL;
    grayImage = NULL;
    globalMemoryPointer = 0;
}

void tBitmap::setPosition(int x, int y)
{
    left = x;
    top = y;
}

void tBitmap::loadBitmap(char *name)
{
    bmpheader head;
    int nc1, nc, n, width, heigth, col;
    int x, y;
    unsigned char *buf;
    buf = (unsigned char*) malloc(32768);
    int ddis, r, g, b;
    unsigned char ch;
    FILE *bmpFile = NULL;
    bmpFile = fopen(name,"rb");
    vis = false;
    if (bmpFile==NULL)
    {
            printf("Error, File %s not found.\n",name);
            exit(1);
    }
    int res;
    res = fread(&head,sizeof(head),1,bmpFile);
    if (res!=1)
    {
        printf("Error reading file %s %d\n",name,res);
    }
    fseek(bmpFile, head.DataOffset, SEEK_SET);
    width = head.Width;
    heigth = head.Height;
    wid = width;
    hei = heigth;
    if (image!=NULL) delete image;
    image = new etna_bo;
    if (image == NULL)
    {
        printf("do not allocated memory. Aborted.\n");
        exit(1);
    }
    image->width = wid;
    image->height = hei;
    image->image = new unsigned int [wid*hei];
    if (image->image == NULL)
    {
        printf("do not allocated memory. Aborted.\n");
        exit(1);
    }
    int dis = 0;
    void *pimage1 = image->image;
    pimage = (uint32_t*) pimage1;
    int strlen;
    switch (head.BitsPerPixel)
    {
        case 1:
            res = fread(buf,head.DataSize,1,bmpFile);
            x = 0;
            y = 0;
            for (n=0;n<head.DataSize;n++)
            {
                ch = buf[n];
                do
                {
                    nc = ch & 1;
                    ch = ch>>1;
                    r = head.pal[nc].R;
                    g = head.pal[nc].G;
                    b = head.pal[nc].B;
                    col = RGBToColor(0,r,g,b);
                    ddis = dis + ((heigth-1-y)*width+x);
                    if (colorKeyPresent) *(pimage+ddis) = col; else *(pimage+ddis) = col|(al<<24);
                    x++;
                    if (x==head.Width)
                    {
                        x = 0;
                        y++;
                    }
                } while (ch>0);
            }
          break;
        case 4:
            res = fread(buf,head.DataSize,1,bmpFile);
            x = 0;
            y = 0;
            for (n=0;n<head.DataSize;n++)
            {
                nc = buf[n] / 16;
                nc1 = buf[n] % 16;
                r = head.pal[nc].R;
                g = head.pal[nc].G;
                b = head.pal[nc].B;
                col = RGBToColor(0,r,g,b);
                ddis = dis + ((heigth-1-y)*width+x);
                if (colorKeyPresent) *(pimage+ddis) = col; else *(pimage+ddis) = col|(al<<24);
                x++;
                if (x==head.Width)
                {
                    x = 0;
                    y++;
                }
                r = head.pal[nc1].R;
                g = head.pal[nc1].G;
                b = head.pal[nc1].B;
                col = RGBToColor(0,r,g,b);
                ddis = dis + ((heigth-1-y)*width+x);
                if (colorKeyPresent) *(pimage+ddis) = col; else *(pimage+ddis) = col|(al<<24);
                x++;
                if (x==head.Width)
                {
                    x = 0;
                    y++;
                }
            }
            break;
        case 8:
            if (head.Width%4==0) strlen = head.Width; else strlen = head.Width + (4-head.Width%4);
            for (y=0;y<head.Height;y++)
            {
                res = fread(buf,1,strlen,bmpFile);
                for (x=0;x<head.Width;x++)
                {
                    r = head.pal[buf[x]].R;
                    g = head.pal[buf[x]].G;
                    b = head.pal[buf[x]].B;
                ddis = dis + ((heigth-1-y)*width+x);
                col = RGBToColor(0,r,g,b);
                if (colorKeyPresent) *(pimage+ddis) = col; else *(pimage+ddis) = col|(al<<24);
                }
            }
            break;
        case 16:
            if ((head.Width*2)%4==0) strlen = head.Width*2; else strlen = head.Width*2 + 2;
            for (y=0;y<head.Height;y++)
            {
                res = fread(buf,2,strlen,bmpFile);
                n = 0;
                for(x=0;x<head.Width;x++)
                {
                    col = buf[n+1] * 256 + buf[n];
                    n = n+2;
                    b = (col % 32)*8;
                    col = col / 32;
                    g = (col % 32)*8;
                    col = col / 32;
                    r = (col % 32)*8;
                    ddis = dis + ((heigth-1-y)*width+x);
                    col = RGBToColor(0,r,g,b);
                    if (colorKeyPresent) *(pimage+ddis) = col; else *(pimage+ddis) = col|(al<<24);
                }
            }
            break;
        case 24:
            if ((head.Width*3)%4==0) strlen = head.Width*3; else strlen = head.Width*3 + (4-(head.Width*3)%4);
            for (y=0;y<head.Height;y++)
            {
                res = fread(buf,3,head.Width,bmpFile);
                n = 0;
                for(x=0;x<head.Width;x++)
                {
                    b = buf[n];
                    g = buf[n+1];
                    r = buf[n+2];
                    n = n+3;
                    ddis = dis + ((heigth-1-y)*width+x);
                    col = RGBToColor(0,r,g,b);
                    if (colorKeyPresent) *(pimage+ddis) = col; else *(pimage+ddis) = col|(al<<24);
                }
            }
            break;
        case 32:
            {
                for (y=0;y<head.Height;y++)
                {
                    res = fread(buf,4,head.Width,bmpFile);
                    n = 0;
                    for(x=0;x<head.Width;x++)
                    {
                        b = buf[n];
                        g = buf[n+1];
                        r = buf[n+2];
                        n = n+4;
                        ddis = dis + ((heigth-1-y)*width+x);
                        col = RGBToColor(0,r,g,b);
                        if (colorKeyPresent) *(pimage+ddis) = col; else *(pimage+ddis) = col|(al<<24);
                    }
                }
            }
            break;
        default:
          break;
    };
    free(buf);
    fclose(bmpFile);
}

unsigned int* tBitmap::getBMPImage() {
    unsigned int* imb = NULL;
    int imageSize = (wid * hei) * 4 + 8;
    imb = (unsigned int*)calloc(imageSize, sizeof(unsigned char));
    unsigned int addr = 0;
    imb[addr++] = wid;
    imb[addr++] = hei;
    if (pimage != NULL)
    {
        for (int j = 0; j<int(wid * hei); j++)
        {
            imb[addr++] = *(pimage + j);
        }
    }
    return imb;
}

int tBitmap::mread(void* buf, int elSize, int elCount, unsigned char* data)
{
    unsigned char* bbuf = (unsigned char*)buf;
    for (int i = 0; i < elSize * elCount; i++)
    {
        *(bbuf+i) = data[globalMemoryPointer++];
    }
    return elSize * elCount;
}

void tBitmap::mseek(unsigned char* data, int offset, int type) {
    if (type == SEEK_SET) globalMemoryPointer = offset;
    if (type == SEEK_CUR) globalMemoryPointer = globalMemoryPointer + offset;
}

void tBitmap::loadBitmapFromMemory(unsigned char* bmpData)
{
    bmpheader head;
    int nc1, nc, n, width, heigth, col;
    int x, y;
    unsigned char* buf;
    buf = (unsigned char*)malloc(32768);
    int ddis, r, g, b;
    unsigned char ch;
    vis = false;
    int res;
    res = mread(&head, sizeof(head), 1, bmpData);
    mseek(bmpData, head.DataOffset, SEEK_SET);
    width = head.Width;
    heigth = head.Height;
    wid = width;
    hei = heigth;
    if (image != NULL) delete image;
    image = new etna_bo;
    if (image == NULL)
    {
        printf("do not allocated memory. Aborted.\n");
        exit(1);
    }
    image->width = wid;
    image->height = hei;
    image->image = new unsigned int[wid * hei];
    if (image->image == NULL)
    {
        printf("do not allocated memory. Aborted.\n");
        exit(1);
    }
    int dis = 0;
    void* pimage1 = image->image;
    pimage = (uint32_t*)pimage1;
    int strlen;
    switch (head.BitsPerPixel)
    {
    case 1:
        res = mread(buf, head.DataSize, 1, bmpData);
        x = 0;
        y = 0;
        for (n = 0; n < head.DataSize; n++)
        {
            ch = buf[n];
            do
            {
                nc = ch & 1;
                ch = ch >> 1;
                r = head.pal[nc].R;
                g = head.pal[nc].G;
                b = head.pal[nc].B;
                col = RGBToColor(0, r, g, b);
                ddis = dis + ((heigth - 1 - y) * width + x);
                if (colorKeyPresent) *(pimage + ddis) = col; else *(pimage + ddis) = col | (al << 24);
                x++;
                if (x == head.Width)
                {
                    x = 0;
                    y++;
                }
            } while (ch > 0);
        }
        break;
    case 4:
        res = mread(buf, head.DataSize, 1, bmpData);
        x = 0;
        y = 0;
        for (n = 0; n < head.DataSize; n++)
        {
            nc = buf[n] / 16;
            nc1 = buf[n] % 16;
            r = head.pal[nc].R;
            g = head.pal[nc].G;
            b = head.pal[nc].B;
            col = RGBToColor(0, r, g, b);
            ddis = dis + ((heigth - 1 - y) * width + x);
            if (colorKeyPresent) *(pimage + ddis) = col; else *(pimage + ddis) = col | (al << 24);
            x++;
            if (x == head.Width)
            {
                x = 0;
                y++;
            }
            r = head.pal[nc1].R;
            g = head.pal[nc1].G;
            b = head.pal[nc1].B;
            col = RGBToColor(0, r, g, b);
            ddis = dis + ((heigth - 1 - y) * width + x);
            if (colorKeyPresent) *(pimage + ddis) = col; else *(pimage + ddis) = col | (al << 24);
            x++;
            if (x == head.Width)
            {
                x = 0;
                y++;
            }
        }
        break;
    case 8:
        if (head.Width % 4 == 0) strlen = head.Width; else strlen = head.Width + (4 - head.Width % 4);
        for (y = 0; y < head.Height; y++)
        {
            res = mread(buf, 1, strlen, bmpData);
            for (x = 0; x < head.Width; x++)
            {
                r = head.pal[buf[x]].R;
                g = head.pal[buf[x]].G;
                b = head.pal[buf[x]].B;
                ddis = dis + ((heigth - 1 - y) * width + x);
                col = RGBToColor(0, r, g, b);
                if (colorKeyPresent) *(pimage + ddis) = col; else *(pimage + ddis) = col | (al << 24);
            }
        }
        break;
    case 16:
        if ((head.Width * 2) % 4 == 0) strlen = head.Width * 2; else strlen = head.Width * 2 + 2;
        for (y = 0; y < head.Height; y++)
        {
            res = mread(buf, 2, strlen, bmpData);
            n = 0;
            for (x = 0; x < head.Width; x++)
            {
                col = buf[n + 1] * 256 + buf[n];
                n = n + 2;
                b = (col % 32) * 8;
                col = col / 32;
                g = (col % 32) * 8;
                col = col / 32;
                r = (col % 32) * 8;
                ddis = dis + ((heigth - 1 - y) * width + x);
                col = RGBToColor(0, r, g, b);
                if (colorKeyPresent) *(pimage + ddis) = col; else *(pimage + ddis) = col | (al << 24);
            }
        }
        break;
    case 24:
        if ((head.Width * 3) % 4 == 0) strlen = head.Width * 3; else strlen = head.Width * 3 + (4 - (head.Width * 3) % 4);
        for (y = 0; y < head.Height; y++)
        {
            res = mread(buf, 3, head.Width, bmpData);
            n = 0;
            for (x = 0; x < head.Width; x++)
            {
                b = buf[n];
                g = buf[n + 1];
                r = buf[n + 2];
                n = n + 3;
                ddis = dis + ((heigth - 1 - y) * width + x);
                col = RGBToColor(0, r, g, b);
                if (colorKeyPresent) *(pimage + ddis) = col; else *(pimage + ddis) = col | (al << 24);
            }
        }
        break;
    case 32:
    {
        for (y = 0; y < head.Height; y++)
        {
            res = mread(buf, 4, head.Width, bmpData);
            n = 0;
            for (x = 0; x < head.Width; x++)
            {
                b = buf[n];
                g = buf[n + 1];
                r = buf[n + 2];
                n = n + 4;
                ddis = dis + ((heigth - 1 - y) * width + x);
                col = RGBToColor(0, r, g, b);
                if (colorKeyPresent) *(pimage + ddis) = col; else *(pimage + ddis) = col | (al << 24);
            }
        }
    }
    break;
    default:
        break;
    };
    free(buf);
}

void tBitmap::getData(unsigned int* width, unsigned int* height, unsigned int data[])
{
    if (pimage!=NULL)
    {
        for(int j=0;j<int(wid*hei);j++)
        {
            data[j] = *(pimage+j);
        }
    }
}

void tBitmap::visible(bool f)
{
    vis = f;
}

int tBitmap::getVisible()
{
    return vis;
}

void tBitmap::setalpha(unsigned char a)
{
    if (a!=al)
    {
        al = a;
        if (pimage!=NULL)
        {
            if (colorKeyPresent)
            {
                for (unsigned int i = 0;i<wid*hei;i++)
                {
                    unsigned int cl = *(pimage+i)&0x00FFFFFF;
                    if (cl==colorKey)
                    {
                        *(pimage+i) = cl;
                    } else
                    {
                        *(pimage+i) = cl|(al<<24);
                    }
                }
            } else
            {
                for (unsigned int i = 0;i<wid*hei;i++)
                {
                    *(pimage+i) = ((*(pimage+i)&0x00FFFFFF)|(al<<24));
                }
            }
        }
    }
}

void tBitmap::setColorKey(unsigned int col)
{
    colorKeyPresent = true;
    colorKey = col&0x00FFFFFF;
    if (pimage!=NULL)
    {
        for (unsigned int i = 0;i<wid*hei;i++)
        {
            unsigned int cl = *(pimage+i)&0x00FFFFFF;
            if (cl==colorKey)
            {
                *(pimage+i) = cl;
            }
        }
    }
}

int tBitmap::getWidth()
{
    return wid;
}

int tBitmap::getHeight()
{
    return hei;
}

unsigned int *tBitmap::getImagePointer()
{
    return pimage;
}

void tBitmap::clearVisible()
{
    vis = false;
}

void tBitmap::refrashImage()
{
    //if (vis&&image!=NULL) addBitmapToQueue(image,left,top,wid,hei);
}

void tBitmap::destroyImage()
{
    if (grayImage!=NULL) delete [] grayImage;
    grayImage = NULL;
	if (image!=NULL)
    {
        if(image->image!=NULL) delete [] image->image;
        image->image = NULL;
        image->width = 0;
        image->height = 0;
        delete image;
    }
	image = NULL;
	pimage = NULL;
	vis = false;
    colorKey = 0;
    colorKeyPresent = false;
	al=0;
}

unsigned int tBitmap::RGBToColor(unsigned char alpha, unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int color = 0;
    color = ((unsigned int)alpha<<24)|((unsigned int)r<<16)|((unsigned int)g<<8)|((unsigned int)b);
    return color;
}

void tBitmap::colorToRGB(unsigned int color, unsigned char *alpha, unsigned char *r, unsigned char *g, unsigned char *b)
{
    *alpha = (color&0xFF000000)>>24;
    *r = (color&0x00FF0000)>>16;
    *g = (color&0x0000FF00)>>8;
    *b = (color&0x000000FF);
}

void tBitmap::makeGrayImage()
{
    if (grayImage) delete [] grayImage;
    grayImage = new unsigned char [wid*hei];
    if (grayImage)
    {
        for (unsigned int i=0; i<hei*wid;i++)
        {
            unsigned int cl = *(pimage+i);
            unsigned char a,r,g,b;
            colorToRGB(cl,&a,&r,&g,&b);
            float cl8 = (float)r*(float)0.3+(float)g*(float)0.59+(float)b*(float)0.11;
            *(grayImage+i) = (unsigned char)cl8;
        }
    }
}

unsigned char *tBitmap::getGrayImagePointer()
{
    return grayImage;
}

/*
void tBitmap::gaborFilterGrayImage()
{
    unsigned int i;
    unsigned int j;
    for (i = K1; i < height - K1; ++i)
    {
        for (j = K2; j < width - K2; ++j)
        {
            for (k = -K1; k <= K1; ++k)
            {
                for (l = -K2; l <= K2; ++l)
                {
                    output[i - K1][j - K2] += input[i - k][j - l] * gabor_2d(k, l, theta);
                }
            }
        }
    }
    for (i = 0; i < height - K1 * 2; ++i)
    {
        for (j = 0; j < width - K2 * 2; ++j)
        {
            ofs1 << abs((int)output[i][j]);
            if (j < width - K2 * 2)
            {
                ofs1 << " ";
            }
        }
        ofs1 << endl;
    }
}

*/

tBitmap::~tBitmap()
{
    //if (pimage!=NULL) free(pimage);
	if (image!=NULL)
    {
        if(image->image!=NULL) delete [] image->image;
        image->image = NULL;
        image->width = 0;
        image->height = 0;
        delete image;
    }
    image = NULL;
    pimage = NULL;
    if (grayImage!=NULL) delete [] grayImage;
    grayImage = NULL;
    left = 0;
    top = 0;
    wid = 0;
    hei = 0;
    al = 0;
    colorKey = 0;
    colorKeyPresent = false;
    vis = false;
    globalMemoryPointer = 0;
}

/*@}*/ /* end of group bmp_module */
/*@}*/ /* end of group pur_module */
