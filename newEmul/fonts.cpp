/** \addtogroup pur_module PUR
@{
*/
/** \addtogroup font_module fonts.cpp
@{
 * \ref fonts.cpp\n
 * \ref fonts.h\n
*/
/**
 * \file fonts.cpp
 * @brief Модуль шрифтов\n
 * @detailed Поддержка стандартных DOS шрифтов 8*16 и 8*8, а так же windows шрифтов из .FON файлов.
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fonts.h"

#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

/**
@struct tfrd
@brief Структура параметров зарегистрированных шрифтов.
*/
PACK(struct tfrd {
    unsigned short id;           ///< Тип шрифта
    int offs;                    ///< смещение данных шрифта от начала файла
    int size;                    ///< размер данных шрифта
    unsigned short nameid;       ///< Поле содержит либо идентификатор шрифта, либо указатель на его имя в таблице имен ресурсов
    unsigned short dfPoints;     ///< размер в пунктах, при котором этот шрифт выглядит лучше всего.
    unsigned char dfCharSet;     ///< символьный набор шрифта
    char dfName[128];             ///< Название шрифта
    char dfFileName[128];         ///< Имя файла, содержащего набор шрифтов
});


/**
@struct TFontDirEntry
@brief Структура параметров шрифта или группы шрифтов в файле.
*/
PACK(struct TFontDirEntry {
    unsigned short dfVersion;               ///< номер версии
    int dfSize;                             ///< размер структуры
    char dfCopyright[60];                   ///< авторские права
    unsigned short dfType;                  ///< Устанавливает тип файла шрифта.
    unsigned short dfPoints;                ///< Устанавливает размер в пунктах, при котором этот символьный набор выглядит лучше всего
    unsigned short dfVertRes;               ///< Устанавливает разрешающую способность по вертикали, в точках на дюйм, при котором этот символьный набор был оцифрован.
    unsigned short dfHorizRes;              ///< Устанавливает разрешающую способность по горизонтали, в точках на дюйм, при котором этот символьный набор был оцифрован.
    unsigned short dfAscent;                ///< Устанавливает расстояние от верха  ячейки формирования рисунка символа до базовой линии типографского шрифта.
    unsigned short dfInternalLeading;       ///< Устанавливает величину междустрочного пробела в границах, установленных членом структуры dfPixHeight. В этой области могут встретиться знаки ударения и другие диакритические символы.
    unsigned short dfExternalLeading;       ///< Устанавливает величину дополнительного междустрочного пробела, который приложение добавляет между рядами.
    unsigned char dfItalic;                 ///< Устанавливает курсивный шрифт, если параметр не равен нулю.
    unsigned char dfUnderline;              ///< Устанавливает подчеркнутый шрифт, если параметр не равен нулю.
    unsigned char dfStrikeOut;              ///< Устанавливает зачеркнутый шрифт, если параметр не равен нулю.
    unsigned short dfWeight;                ///< Устанавливает толщину шрифта в диапазоне от 0 до 1000. Например, 400 является прямым (нормальным) шрифтом, а 700 жирным.
    unsigned char dfCharSet;                ///< Устанавливает символьный набор шрифта.
    unsigned short dfPixelWidth;            ///< Устанавливает ширину сетки, в которой векторный шрифт был переведен в цифровую форму.
    unsigned short dfPixelHeight;           ///< Устанавливает высоту битового образа символа для растровых шрифтов или высоту сетки, в которой  векторный шрифт был переведен в цифровую форму.
    unsigned char dfPitchAndFamily;         ///< Устанавливает наклон и семейство шрифта.
    unsigned short dfAvgWidth;              ///< Устанавливает среднюю ширину символов в шрифте
    unsigned short dfMaxWidth;              ///< Устанавливает ширину самого широкого символа в шрифте
    unsigned char dfFirstChar;              ///< Устанавливает первый символьный код, определяемый в шрифте.
    unsigned char dfLastChar;               ///< Устанавливает последний символьный код, определяемый в шрифте.
    unsigned char dfDefaultChar;            ///< Устанавливает символ, замещающий символы отсутствующие в шрифте.
    unsigned char dfBreakChar;              ///< Устанавливает символ, который должен быть использован, чтобы определить границу слова для выравнивания текста.
    unsigned short dfWidthBytes;            ///< Устанавливает число байтов в каждом ряде точечного рисунка.
    int dfDevice;                           ///< Устанавливает смещение символьной строки с нулем в конце в файле, которая определяет имя устройства. Для универсального шрифта, это значение равняется нулю.
    int dfFace;                             ///< Устанавливает смещение символьной строки с нулем в конце в файле, которая  именует гарнитуру шрифта.
    int dfReserved;                         ///< Этот член структуры зарезервирован.
});

/**
@struct Tgl
@brief Структура ширины и положения символов.
*/
PACK(struct Tgl {
    unsigned short gwidth;                  ///< Ширина символа
    unsigned short goffset;                 ///< Смещение символа, относительно начала данных шрифта
});

/**
@struct TFontHeader
@brief Структура заголовка шрифта.
*/
PACK(struct TFontHeader {
    unsigned short dfVersion;               ///< номер версии
    int dfSize;                             ///< размер структуры
    char dfCopyright[60];                   ///< авторские права
    unsigned short dfType;                  ///< Устанавливает тип файла шрифта.
    unsigned short dfPoints;                ///< Устанавливает размер в пунктах, при котором этот символьный набор выглядит лучше всего
    unsigned short dfVertRes;               ///< Устанавливает разрешающую способность по вертикали, в точках на дюйм, при котором этот символьный набор был оцифрован.
    unsigned short dfHorizRes;              ///< Устанавливает разрешающую способность по горизонтали, в точках на дюйм, при котором этот символьный набор был оцифрован.
    unsigned short dfAscent;                ///< Устанавливает расстояние от верха  ячейки формирования рисунка символа до базовой линии типографского шрифта.
    unsigned short dfInternalLeading;       ///< Устанавливает величину междустрочного пробела в границах, установленных членом структуры dfPixHeight. В этой области могут встретиться знаки ударения и другие диакритические символы.
    unsigned short dfExternalLeading;       ///< Устанавливает величину дополнительного междустрочного пробела, который приложение добавляет между рядами.
    unsigned char dfItalic;                 ///< Устанавливает курсивный шрифт, если параметр не равен нулю.
    unsigned char dfUnderline;              ///< Устанавливает подчеркнутый шрифт, если параметр не равен нулю.
    unsigned char dfStrikeOut;              ///< Устанавливает зачеркнутый шрифт, если параметр не равен нулю.
    unsigned short dfWeight;                ///< Устанавливает толщину шрифта в диапазоне от 0 до 1000. Например, 400 является прямым (нормальным) шрифтом, а 700 жирным.
    unsigned char dfCharSet;                ///< Устанавливает символьный набор шрифта.
    unsigned short dfPixelWidth;            ///< Устанавливает ширину сетки, в которой векторный шрифт был переведен в цифровую форму.
    unsigned short dfPixelHeight;           ///< Устанавливает высоту битового образа символа для растровых шрифтов или высоту сетки, в которой  векторный шрифт был переведен в цифровую форму.
    unsigned char dfPitchAndFamily;         ///< Устанавливает наклон и семейство шрифта.
    unsigned short dfAvgWidth;              ///< Устанавливает среднюю ширину символов в шрифте
    unsigned short dfMaxWidth;              ///< Устанавливает ширину самого широкого символа в шрифте
    unsigned char dfFirstChar;              ///< Устанавливает первый символьный код, определяемый в шрифте.
    unsigned char dfLastChar;               ///< Устанавливает последний символьный код, определяемый в шрифте.
    unsigned char dfDefaultChar;            ///< Устанавливает символ, замещающий символы отсутствующие в шрифте.
    unsigned char dfBreakChar;              ///< Устанавливает символ, который должен быть использован, чтобы определить границу слова для выравнивания текста.
    unsigned short dfWidthBytes;            ///< Устанавливает число байтов в каждом ряде точечного рисунка.
    int dfDevice;                           ///< Устанавливает смещение символьной строки с нулем в конце в файле, которая определяет имя устройства. Для универсального шрифта, это значение равняется нулю.
    int dfFace;                             ///< Устанавливает смещение символьной строки с нулем в конце в файле, которая  именует гарнитуру шрифта.
    int dfBitsPointer;                      ///< Устанавливает абсолютный адресс размещения шрифта в памяти
    int dfBitsOffset;                       ///< Устанавливает смещение данных необходимых для построения шрифта в файле.
    unsigned char dfReserved;               ///< Зарезервировано, не используется.
    struct Tgl gle[256];                    ///< Таблица ширины и положения символов.
});

/**
@struct NEH
@brief Структура заголовка NE-файла. Заголовок .fon файла имеет NE формат.
*/
PACK(struct NEH {
    unsigned char sig[2];                   ///< Сигнатура "NE"
    unsigned char verhi;                    ///< Старшпя часть новера версии
    unsigned char verlo;                    ///< Младшая часть номера версии
    unsigned short EntryTableOffset;        ///< Смещение таблицы входов (относительно начала заголовка)
    unsigned short EntryTableSize;          ///< Длина таблицы входов (в байтах)
    int CheckSum;                           ///< Контрольная сумма
    unsigned short flags;                   ///< Набор флагов: (16 бит)
    unsigned short SegNum;                  ///< Число автосегментов данных
    unsigned short HeapSize;                ///< Начальный размер (в байтах) локальной кучи. При ее отсутствии равен нулю
    unsigned short StackSize;               ///< Начальный размер стека (в байтах). Равен нулю, если SS не равно DS, как в библиотеках
    unsigned short IP;                      ///< адрес указателя команд
    unsigned short CS;                      ///< адрес сегмента кода
    unsigned short SP;                      ///< адрес указателя стека
    unsigned short NumSS;                   ///< адрес сегмента стека
    unsigned short NumTables;               ///< Число входов в таблице сегментов
    unsigned short NumModules;              ///< Число входов в таблице ссылок на модули
    unsigned short NumBytesNonResident;     ///< Число байт в таблице нерезидентного имени
    unsigned short OffsetSegment;           ///< Относительное смещение начала таблицы сегментов от начала нового заголовка
    unsigned short OffsetResource;          ///< Относительное смещение начала таблицы ресурсов от начала нового заголовка
    unsigned short OffsetRNames;            ///< Относительное смещение начала таблицы резидентного имени от начала нового заголовка
    unsigned short OffsetModules;           ///< Относительное смещение начала таблицы ссылок на модули от начала нового заголовка
    unsigned short OffsetImported;          ///< Относительное смещение начала таблицы импортируемых имен от начала нового заголовка
    unsigned short OffsetNonResident;       ///< Относительное смещение начала таблицы нерезидентных имен от начала файла
    unsigned short NumMovEP;                ///< Число перемещаемых точек входа
    unsigned short Aligment;                ///< Множитель смещении. Используется при размещении логических секторов.Выражается степенью логарифма по основанию 2.По умолчанию равен 9 (512)
    unsigned char reserved[12];             ///< Зарезервировано 12 байт
});

/**
@struct rd
@brief Структура описания ресурса.
*/
PACK(struct rd {
    unsigned short offs;                    ///< Смещение данных ресурса относительно начала файла в единицах, указанных в начале таблицы ресурсов
    unsigned short len;                     ///< Длина ресурса (в байтах)
    unsigned short flags;                   ///< Флаги
    unsigned short name;                    ///< Определяет (если старший бит равен единице) или указывает на ID ресурса смещение относительно начала таблицы ресурсов
    int reserved;                           ///< Зарезервировано 4 байта
});

/**
@struct res
@brief Структура таблицы ресурсов.
*/
PACK(struct res {
    unsigned short types;                   ///< Тип ресурса
    unsigned short num;                     ///< Количество ресурсов данного типа
    int reserved;                           ///< Зарезервировано 4 байта
});

/**
@struct tfontinfo
@brief Структура параметров шрифта.
*/
struct tfontinfo {
    int xres;                               ///< средняя ширина символа в точках
    int yres;                               ///< высота символа в точках
    int interval;                           ///< межсимвольный интервал
};


tfrd* frd;                              ///< Параметры зарегистрированных шрифтов
tfrd* frd1;                             ///< Параметры зарегистрированных шрифтов без учета каталогов шрифтов
unsigned short numfrd;                  ///< Количество найденных записей о шрифтах
unsigned short FontsCount;              ///< Количество зарегистрированных шрифтов
void* datafont;                         ///< Указатель на данные шрифта
TFontHeader* fontheader;                ///< Указатель на заголовок шрифта
unsigned char* dt;                      ///< Указатель на данные шрифта
NEH* NEHeader;                          ///< Указатель на NE заголовок файла
struct res* ResTable;                   ///< Указатель на таблицу ресурсов
rd* ResDes;                             ///< Указатель на запись о ресурсе
unsigned char defFont[256][16];         ///< стандартный (DOS) шрифт 8*16
unsigned char defFont1[256][8];         ///< стандартный (DOS) шрифт 8*8
struct tfontinfo fontinfo[256];          ///< таблица параметров шрифта


/**
*@fn int length(char *c)
*@brief Функция возвращает количество символов в строке.
*@param c строка символов
*@return возвращает количество символов.
*/
int Font::length(char *c)
{
    int i=0;
    while (*(c+i)!=0) i++;
    return i;
}

void Font::setPhisicalScreen()
{
    isPhisical = 1;
}

void Font::restoreVirtualScreen()
{
    isPhisical = 0;
}

/**
*@fn void LoadResourceFromNE(char *name)
*@brief Функция загружает параметры русурсов из .FON файла.
*@param name имя файла
*/
void Font::LoadResourceFromNE(char *name)
{
    FILE *f = NULL;
    char sig[4];
    unsigned short ROffset,ResShiftCount;
    int NEOffset,pr;
    unsigned char lstr,i,j;
    char c;
    f = fopen(name,"rb");
    if (f==NULL)
    {
        printf("Do not open font file\n");
        exit(1);
    }
    fread(sig,1,sizeof(sig),f);
    if (sig[0]!='M'&&sig[1]!='Z')
    {
        printf("Error! This is not MZ header");
        exit(1);
    }
    fseek(f,60,SEEK_SET);
    fread(&NEOffset,1,sizeof(NEOffset),f);
    fseek(f,NEOffset,SEEK_SET);
    NEHeader = (NEH*) calloc(sizeof(NEH),1);
    fread(NEHeader,1,sizeof(NEH),f);
    if (NEHeader->sig[0]!='N'&&NEHeader->sig[1]!='E')
    {
            printf("Error! This is not NE header");
            free(NEHeader);
            exit(1);
    }
    ROffset = NEHeader->OffsetResource;
    free(NEHeader);
    fseek(f,NEOffset+ROffset,SEEK_SET);
    fread(&ResShiftCount,1,sizeof(ResShiftCount),f);
    ResTable = (res*) calloc(sizeof(res),1);
    ResDes = (rd*) calloc(sizeof(rd),1);
    numfrd = 0;
    do
    {
        fread(ResTable,1,sizeof(res),f);
        if (ResTable->types>32768) ResTable->types=ResTable->types-32768;
        if (ResTable->types>0)
        {
            for(i=0;i<ResTable->num;i++)
            {
                fread(ResDes,1,sizeof(rd),f);
                if (ResTable->types==7||ResTable->types==8)
                {
                    frd1[numfrd].id = ResTable->types;
                    pr = ResDes->offs;
                    pr = pr << ResShiftCount;
                    frd1[numfrd].offs = pr;
                    pr = ResDes->len;
                    pr = pr << ResShiftCount;
                    frd1[numfrd].size = pr;
                    if (ResDes->name>32768)
                    {
                        ResDes->name = ResDes->name-32768;
                        frd1[numfrd].nameid = ResDes->name;
                    } else
                    {
                        pr = ftell(f);
                        fseek(f,NEOffset+ROffset+ResDes->name,SEEK_SET);
                        frd1[numfrd].nameid = 0;
                        fread(&lstr,1,sizeof(lstr),f);
                        for(j=1;j<lstr;j++)
                        {
                            fread(&c,1,sizeof(c),f);
                        }
                        fseek(f,pr,SEEK_SET);
                    }
                    numfrd++;
                 }
            }
        }
    } while(ResTable->types!=0);
    free(ResDes);
    free(ResTable);
    fclose(f);
}

/**
*@fn void LoadResourceFromFNT(char *name)
*@brief Функция загружает параметры русурсов из .FNT файла.
*@param name имя файла
*/
void Font::LoadResourceFromFNT(char *name)
{
    FILE *f;
    f = fopen(name,"rb");
    if (f!=NULL)
    {
        int sig;
        fread(&sig,4,1,f);
        TFontDirEntry tf;
        fread(&tf,sizeof(TFontDirEntry),1,f);
        frd1[0].id = 8;
        frd1[0].offs = 4;
        frd1[0].size = tf.dfSize - 4;
        frd1[0].dfPoints = tf.dfPoints;
        frd1[0].dfCharSet = tf.dfCharSet;
        int i=0;
        while(*(name+i)!='.')
        {
            if (*(name+i)>'0'&&*(name+i)<'9')
            {
                frd1[0].dfName[i] = 0;
                break;
            }
            frd1[0].dfName[i] = *(name+i);
            i++;
        }
        frd1[0].dfName[i] = 0;
        sprintf(frd1[0].dfFileName,"%s",name);
    }
}

/**
*@fn void FontDir(char *name, int offs)
*@brief Функция извлекает из русурсов .FON файла параметры шрифтов.
*@param name имя файла
*@param offs смещение ресурса в файле
*/
void Font::FontDir(char *name, int offs)
{
    FILE *f;
    unsigned short i,j,k,id;
    unsigned char b;
    unsigned short numfonts;
    TFontDirEntry *fde;
    char *szDeviceName, *szFaceName;
    szDeviceName = (char*) calloc(256,sizeof(char));
    szFaceName = (char*) calloc(16,sizeof(char));
    f = fopen(name,"rb");
    if (f==NULL)
    {
        printf("Do not open font file\n");
        exit(1);
    }
    fseek(f,offs,SEEK_SET);
    fread(&numfonts,1,sizeof(numfonts),f);
    fde = (TFontDirEntry*) calloc(1,sizeof(TFontDirEntry));
    for(i=0;i<numfonts;i++)
    {
        fread(&id,1,sizeof(id),f);
        j = 0;
        while (frd1[j].nameid !=id)
            j++;
        fread(fde,1,sizeof(TFontDirEntry),f);
        frd1[j].dfPoints = fde->dfPoints;
        frd1[j].dfCharSet = fde->dfCharSet;
        *szDeviceName = '\0';
        int ii = 0;
        do{
            fread(&b,1,1,f);
            *(szDeviceName+ii) = b;
            ii++;
        } while (b!=0);
        *szFaceName ='\0';
        ii = 0;
        do{
            fread(&b,1,1,f);
            *(szFaceName+ii) = b;
            ii++;
        } while (b!=0);
        for (k=0; k<128; k++)
        {
            frd1[j].dfName[k] = *(szFaceName+k);
            if (*(szFaceName+k)==0) break;
        }
        sprintf(frd1[j].dfFileName,"%s",name);
    }
    free(szFaceName);
    free(szDeviceName);
    free(fde);
    fclose(f);
}


/**
*@fn void SetFontID(unsigned short id)
*@brief Функция выбирает шрифт с помощью которого будет выводиться текст.
*@param id номер шрифта
*/
void Font::SetFontID(unsigned short id)
{

    FILE *f;
    FontID = id;
    f = fopen(frd[id].dfFileName,"rb");
    if (f==NULL)
    {
        printf("Do not open font file\n");
        exit(1);
    }
    fseek(f,frd[id].offs,SEEK_SET);
    if (datafont!=NULL) free(datafont);
    datafont = calloc(1,frd[id].size);
    fread(datafont,1,frd[id].size,f);
    fclose(f);
    fontheader = (TFontHeader*) datafont;
    dt = (unsigned char*) datafont;
    fontinfo[id].xres = fontheader->dfAvgWidth;
    fontinfo[id].yres = fontheader->dfPixelHeight;
}

int Font::getFontHeight(int id, int size) {
    return fontinfo[id].yres * size;
}

/**
*@fn void print1(unsigned char c)
*@brief Функция выводит на экран символ текущим шрифтом.
*@param c символ
*/
void Font::print1(unsigned char c)
{
    unsigned short width,offs,x,y,xc,yc;
    unsigned char w,h,k,z,bytecount;

    xc = xt;
    yc = yt;
    c = c - fontheader->dfFirstChar;
    width = fontheader->gle[c].gwidth;
    offs = fontheader->gle[c].goffset;
    x = xc;
    y = yc;
    bytecount = ((width-1) / 8)+1;
    for (w=0;w<bytecount;w++)
    {
        for (h=0;h<fontheader->dfPixelHeight;h++)
        {
            int addr = offs+(w*fontheader->dfPixelHeight+h);
            c = dt[addr];
            k = 128;
            z = 0;
            do{
                if (c>=k)
                {
                    if (isPhisical) grContext->putpixel(x+z,y,t_color); //else putPixel(x+z,y,t_color);
                }
                if (c>=k) c = c - k;
                k = k / 2;
                z++;
            } while (k>0);
        y++;
        }
        x = x + 8;
        y = yc;
    }
    xt = xt + width-fontinfo[FontID].xres + fontheader->dfExternalLeading;
}

/**
*@fn void print(unsigned char c)
*@brief Функция выводит на экран символ, если текущим шрифтом выбран стандартный DOS шрифт (id=0 для шрифта 8*16 или id=1 для шрифта 8*8) функцией setFontParam.
*@param c символ
*/
void Font::print(unsigned char c)
{
 int j;
 int xp,yp;
    xp = xt;
    yp = yt;
   if (FontID<=1) {
        for(j=0;j<fontinfo[FontID].yres;j++)
        {
            unsigned char l;
            switch (FontID)
            {
                case 0: l = defFont[c][j];
                    break;
                case 1: l = defFont1[c][j];
                    break;
                default:
                    break;
            };
            int k;
            for(k=0;k<8;k++)
            {
                if ((l & 0x80) == 0x80)
                {
                    int i1,j1;
                    for(i1=0;i1<t_scale;i1++)
                    {
                        for(j1=0;j1<t_scale;j1++)
                        {
                            if (isPhisical) grContext->putpixel(xp+j1,yp+i1,t_color); //else putPixel(xp+j1,yp+i1,t_color);
                        }
                    }
                }
                xp = xp + t_scale;
                l = l<<1;
            }
            xp = xt;
            yp = yp + t_scale;
        }
    } else print1(c);
}

/**
*@fn int getCount(char *c)
*@brief Функция считает количество символов, которые поместятся в контекст вывода
*@param c строка символов
*@return количество символов
*/
int Font::getCount(char *c)
{
    int i = 0;
    int n = minx;
    do
    {
        unsigned char k = *(c+i);
        if (k >= fontheader->dfFirstChar) k = k - fontheader->dfFirstChar; k = 0;
        n = n + fontheader->gle[k].gwidth;
        if (*(c+i)==0) return i+1;
        i++;
    } while(n<maxx);
    return i-1;
}

/**
*@fn int getLenTextInPixels(char *c, int size)
*@brief Функция считает ширину текста в точках
*@param c строка символов
*@param size коэффициент масштабирования
*@return ширина текста в точках
*/
int Font::getLenTextInPixels(char* c, int size)
{
    int n = 0;
    int i = 0;
    unsigned char k = 0;
    while (*(c + i))
    {
        k = *(c + i) - fontheader->dfFirstChar;
        //n += fontheader->gle[k].gwidth; 
        //n += fontinfo[FontID].xres;
        if (FontID==0 || FontID==1) n += fontinfo[FontID].xres; else n += fontheader->gle[k].gwidth + fontheader->dfExternalLeading;
        i++;
    };
    return n * size;
}

/**
*@fn void printxy(int x, int y, char* text)
*@brief Функция выводит текст на экран начиная с точки с координатами (x,y) (левый верхний угол)
*@param x координата X
*@param y координата Y
*@param text выводимый текст
*/
void Font::printxy(int x, int y, char* text)
{
 int i, j, scount, lcount, maxlcount;
 unsigned char c;
 unsigned char txt[160][90];
 j = 1;
 i = 0;
 if (isPhisical) grContext->getOutputContext(&minx,&miny,&maxx,&maxy); //else ggetOutputContext(&minx,&miny,&maxx,&maxy);
 scount = getCount(text);
 maxlcount = (maxy - miny) / fontinfo[FontID].yres;
 lcount = 0;
 do {
   c = *(text+i);
   txt[j][lcount] = c;
   j++;
   i++;
   if (c==10)
   {
       txt[j-1][lcount] = 0;
       txt[0][lcount] = j-1;
       j = 1;
       lcount++;
   }
   if (j==scount)
   {
        txt[j][lcount] = 0;
        txt[0][lcount] = j-1;
        lcount++;
        j = 1;
   }
 } while (c!=0);
 txt[0][lcount] = j-1;
 xt = x;
 yt = y;
 lcount++;
 if (lcount>maxlcount)
 {
    lcount = maxlcount;
 }
 switch (t_valign)
    {
        case alVTOP: yt = y;
          break;
        case alVCENTER: yt = y + (maxy-miny)/2-(lcount*fontinfo[FontID].yres*t_scale)/2;
          break;
        case alVBOTTOM:  yt = y + maxy-miny-lcount*fontinfo[FontID].yres*t_scale;
          break;
        default: yt = y;
          break;
    };
 int ddx = 0;
 for (j=0;j<lcount;j++)
 {
 for(int t = 1;t<txt[0][j]+1;t++) ddx = ddx + fontheader->gle[txt[t][j]-fontheader->dfFirstChar].gwidth;
 switch (t_halign)
    {
        case alHLEFT: xt = x;
          break;
        case alHCENTER: xt = x + (maxx-minx)/2-ddx*t_scale/2;
          break;
        case alHRIGHT:  xt = maxx-ddx*t_scale;
          break;
        default: xt = x;
          break;
    };
    ddx = 0;
    for (i=1;i<=txt[0][j];i++)
        {
            if (txt[i][j]!=0)
                if (txt[i][j]!=32) print(txt[i][j]);
            if (txt[i][j]==32) xt = xt + fontinfo[FontID].xres * t_scale; else xt = xt + fontinfo[FontID].xres * t_scale;
        }
        yt = yt + fontinfo[FontID].yres;
        xt = x;
    }
}

/**
*@fn void setFontParam(int fid, unsigned int col, int scale, char halign, char valign)
*@brief Функция устанавливает параметры по ID шрифта
*@param fid Номер шрифта
*@param col Цвет шрифта
*@param scale коэффициент масштабирования
*@param halign горизонтальное выравнивание
*@param valign вертикальное выравнивание
*/
void Font::setFontParam(int fid, unsigned int col, int scale, char halign, char valign)
{
	if (FontID != fid)
	{
        FontID = fid;
        if (FontID>1) SetFontID(FontID);
	}
	t_color = col;
	t_scale = scale;
	t_halign = halign;
	t_valign = valign;
}

/**
*@fn int setFont(char *name, int sz, unsigned int col, char halign, char valign)
*@brief Функция устанавливает параметры по названию и размеру шрифта
*@param name имя шрифта
*@param sz размер шрифта
*@param col Цвет шрифта
*@param halign горизонтальное выравнивание
*@param valign вертикальное выравнивание
*@return ID шрифта
*/
int Font::setFont(char *name, int sz, unsigned int col, char halign, char valign)
{
    int i = 1;
    int fid = 0;
    bool fn = false;
    do
    {
        i++;
        if (strcmp(name,frd[i].dfName)==0&&sz==frd[i].dfPoints)
        {
            fid = i;
            fn = true;
            break;
        }
    } while(i<FontsCount);
    if (fn)
    {
        if (FontID != fid)
        {
            FontID = fid;
            if (FontID>1) SetFontID(FontID);
        }
        t_color = col;
        t_halign = halign;
        t_valign = valign;
        return fid;
    } else return -1;
}

/**
*@fn fontRegistred(char *name, int sz);
*@brief Функция определяет зарегистрирован ли шрифт
*@param name имя шрифта
*@param sz размер шрифта
*@return 0 - шрифт не зарегистрирован, >0 - шрифт уже зарегистрирован
*/
bool Font::fontRegistred(char *name, int sz)
{
    int i = 1;
    //int fid = 0;
    bool fn = false;
    do
    {
        i++;
        if (strcmp(name,frd[i].dfName)==0&&sz==frd[i].dfPoints)
        {
            //fid = i;
            fn = true;
            break;
        }
    } while(i<=FontsCount);
    return fn;
}

/**
*@fn unsigned char registerNewFont(char* name)
*@brief Функция регистрирует новые шрифты в системе
*@param name Имя файла шрифта
*@return Количество зарегистрированных шрифтов
*/
unsigned char Font::registerNewFont(char* name)
{
    FILE *ffile;
    ffile = fopen(name,"rb");
    int sig;
    if (ffile!=NULL)
    {

        fread(&sig,4,1,ffile);
        fclose(ffile);
    }
    if (sig!=0x20544E46)
    {
        LoadResourceFromNE(name);
        int i = 0;
        do
        {
            if (frd1[i].id==7) FontDir(name,frd1[i].offs);
            i++;
        } while (frd1[i].id==7||i!=numfrd);
        for (i=0;i<numfrd;i++)
        {
            if (frd1[i].id==8)
            {
                FontsCount++;
                frd[FontsCount] = frd1[i];
            }
        }
    } else
    {
        LoadResourceFromFNT(name);
        FontsCount++;
        frd[FontsCount] = frd1[0];
    }
	return FontsCount;
}

Font::Font(Graph* gr)
{
    grContext = gr;
    isPhisical = 1;
    t_color = cWHITE;
    t_scale = 1;
    FontID = 0;
    t_halign = 0;
    t_valign = 0;
}

void Font::fontinit() {
    FontsCount = 0;
    datafont = NULL;
    FILE* defFontFile = NULL;
    defFontFile = fopen("8X16WIN1251.FNT", "rb");
    if (defFontFile == NULL)
    {
        printf("Do not open default font file: 8X16WIN1251.FNT Programm aborted.\n");
        exit(1);
    }
    int res;
    res = fread(defFont, sizeof(defFont), 1, defFontFile);
    if (res != 1)
    {
        printf("Do not read file: 8X16WIN1251.FNT Programm aborted.\n");
        exit(1);
    }
    fclose(defFontFile);
    fontinfo[0].xres = 8;
    fontinfo[0].yres = 16;
    defFontFile = fopen("8X8WIN1251.FNT", "rb");
    if (defFontFile == NULL)
    {
        printf("Do not open default font file: 8X8WIN1251.FNT Programm aborted.\n");
        exit(1);
    }
    res = fread(defFont1, sizeof(defFont1), 1, defFontFile);
    if (res != 1)
    {
        printf("Do not read file: 8X18IN1251.FNT Programm aborted.\n");
        exit(1);
    }
    fclose(defFontFile);
    fontinfo[1].xres = 8;
    fontinfo[1].yres = 8;
    frd1 = (tfrd*)calloc(16, sizeof(tfrd));
    frd = (tfrd*)calloc(512, sizeof(tfrd));
}

Font::~Font()
{
    grContext = NULL;
    isPhisical = 0;
    t_color = 0;
    t_scale = 0;
    FontID = 0;
    t_halign = 0;
    t_valign = 0;
}

void Font::closefont() {
    free(frd);
    free(frd1);
}


/*@}*/ /* end of group font_module */
/*@}*/ /* end of group pur_module */

