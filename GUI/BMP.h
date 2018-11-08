#ifndef BMP_H_
#define BMP_H_

#include <iostream>
#include <string>
#include <Graphics/Graphics.h>
#include <Utilities/LOGGING.h>

using namespace std;

#pragma pack(push, 1)

struct BITMAPFILEHEADER {
    unsigned short bfType;  //specifies the file type
    unsigned int bfSize;  //specifies the size in bytes of the bitmap file
    unsigned short bfReserved1;  //reserved; must be 0
    unsigned short bfReserved2;  //reserved; must be 0
    unsigned int bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

#pragma pack(pop)

#pragma pack(push, 1)

struct BITMAPINFOHEADER {
    unsigned int biSize;  //specifies the number of bytes required by the struct
    unsigned int biWidth;  //specifies width in pixels
    unsigned int biHeight;  //species height in pixels
    unsigned short biPlanes; //specifies the number of color planes, must be 1
    unsigned short biBitCount; //specifies the number of bit per pixel
    unsigned int biCompression;//spcifies the type of compression
    unsigned int biSizeImage;  //size of image in bytes
    unsigned int biXPelsPerMeter;  //number of pixels per meter in x axis
    unsigned int biYPelsPerMeter;  //number of pixels per meter in y axis
    unsigned int biClrUsed;  //number of colors used by th ebitmap
    unsigned int biClrImportant;  //number of colors that are important
};

#pragma pack(pop)

struct BMP {
	char* Data;
	int W, H;
	BITMAPFILEHEADER bmp_header;
    BITMAPINFOHEADER bmp_info_header;
};

void Initialize_BMP(BMP*, string);
void Initialize_BMP(BMP*, string, int, int);
void resizeBilinear(int*, int*, int, int, int, int);
void Draw_BMP(BMP*, Graphics*, int, int);
void Draw_BMP(BMP*, char*, int, int, int);
void Delete_BMP(BMP*);

#endif
