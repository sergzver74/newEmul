#ifndef __BMP_H__
#define __BMP_H__

//extern "C" {
//    #include "inc/etna_bo.h"
//}

typedef struct{
    unsigned int width;
    unsigned int height;
    unsigned int *image;
}etna_bo;


/**
@class tBitmap
@brief Структура палитры.
*/
class tBitmap
{
	private:
		unsigned int left;                   ///< координата X начала изображения (левый верхний угол)
		unsigned int top;                    ///< координата Y начала изображения (левый верхний угол)
		unsigned int wid;                    ///< ширина изображения в точках
		unsigned int hei;                    ///< высота изображения в точках
		unsigned char al;                    ///< прозрачность в процентах
		unsigned int *pimage;                ///< указатель на данные изображения
        unsigned int colorKey;               ///< Цветовой ключ (данный цвет не отображается при выводе картинки)
        bool colorKeyPresent;                ///< флаг присутствия цветового ключа
        etna_bo *image;                      ///< указатель на поверхность, содержащая изображение
		bool vis;                            ///< разрешение на показ изображения. 0 - изображение не показывается, 1 - показать изображение.
        unsigned char *grayImage;            ///< Изображение в оттенках серого
        int globalMemoryPointer;             ///< Указатель на текущееположение чтения из памяти
        int mread(void* buf, int elSize, int elCount, unsigned char* data);
        void mseek(unsigned char* data, int offset, int type);
    public:
        tBitmap();
        /**
        *@fn void setPosition(int x, int y);
        *@brief Функция устанавливает координаты верхнего левого угла для вывода изображения.
        *@param x координата по оси X.
        *@param y координата по оси Y.
        */
		void setPosition(int x, int y);
        /**
        *@fn void loadBitmapFromMemory (unsigned char* bmpData);
        *@brief Функция загружает bmp файл из массива в памяти.
        *@param bmpData указатель на данные файла.
        */
        void loadBitmapFromMemory(unsigned char* bmpData);
        /**
        *@fn void loadBitmap (char *name);
        *@brief Функция загружает bmp файл.
        *@param name имя файла для загрузки.
        */
        void loadBitmap (char *name);
        /**
        *@fn void getData(unsigned short* width, unsigned short* height, unsigned short data[]);
        *@brief Функция получает изображение для дальнейшей его обработки.
        *@param width Ширина.
        *@param height Высота.
        *@param data данные изображения
        */
		void getData(unsigned int* width, unsigned int* height, unsigned int data[]);
        /**
        *@fn void visible(bool f);
        *@brief Функция показывает или убирает изображение.
        *@param f 0 - убрать изображение с экрана, 1 - показать изображение.
        */
		void visible(bool f);
        /**
        *@fn void setalpha(char a);
        *@brief Функция устанавливает уровень прозрачности.
        *@param a прозрачность в процентах.
        */
		void setalpha(unsigned char a);
        /**
        *@fn int getVisible();
        *@brief возвращает состояние видимости картинки.
        *@return 0-невидима 1-показывается.
        */
        int getVisible();
        /**
        *@fn int getWidth();
        *@brief возвращает длину картинки в точках.
        *@return длина.
        */
        int getWidth();
        /**
        *@fn int getHeight();
        *@brief возвращает высоту картинки в точках.
        *@return высота.
        */
        int getHeight();
        /**
        *@fn unsigned int *getImagePointer();
        *@brief возвращает указатель на изображение картинки.
        *@return указатель.
        */
        unsigned int *getImagePointer();
        /**
        *@fn void clearVisible();
        *@brief сбрасывает свойство видимости объекта
        */
        void clearVisible();
        /**
        *@fn void setColorKey(unsigned int col);
        *@brief Устанавливает цветовой ключ
        *@param col цвет.
        */
        void setColorKey(unsigned int col);
        /**
        *@fn void refrashImage();
        *@brief добавляет изображение в очередь для отображения
        */
        void refrashImage();
        /**
        *@fn void destroyImage();
        *@brief Функция удаляет изображение.
        */
		void destroyImage();
        unsigned int RGBToColor(unsigned char alpha, unsigned char r, unsigned char g, unsigned char b);
        void colorToRGB(unsigned int color, unsigned char *alpha, unsigned char *r, unsigned char *g, unsigned char *b);
        void makeGrayImage();
        unsigned char *getGrayImagePointer();
        void imgMean(int size);
        void gaborFilterGrayImage();
        unsigned int* getBMPImage();
        ~tBitmap();
};


#endif
