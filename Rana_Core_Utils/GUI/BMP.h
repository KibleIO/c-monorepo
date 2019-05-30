//PLATFORMS: Linux, Windows, OSX

#ifndef BMP_H_
#define BMP_H_

#include <iostream>
#include <string>

#include "avir.h"
#include "../Graphics/Graphics.h"
#include "../Utilities/LOGGING.h"
#include "../Utilities/Utils.h"

#define BMP_FALLBACK_C1 0xFF00FF
#define BMP_FALLBACK_C2 0xCC00CC

using namespace std;

// Linux specific code {{{
#ifdef __linux__
#pragma pack(push, 1)

struct BITMAPFILEHEADER {
	unsigned short bfType;  //specifies the file type
	unsigned int bfSize;  //specifies the size in bytes of the bitmap file
	unsigned short bfReserved1;  //reserved; must be 0
	unsigned short bfReserved2;  //reserved; must be 0
	unsigned int bfOffBits;  //offset in bytes from header to the bitmap bits
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
#endif
// }}} Windows specific code {{{
#ifdef _WIN64
#endif
// }}} OSX specific code {{{
#ifdef __APPLE__
#endif
// }}}

struct BMP {
	char* Data;
	int W, H;
	bool Transparent;
	BITMAPFILEHEADER bmp_header;
	BITMAPINFOHEADER bmp_info_header;
	string name;
};

void Initialize_BMP(BMP*, string);
void Initialize_BMP(BMP*, string, int, int);
void resizeBilinear(int*, int*, int, int, int, int);
void Draw_BMP(BMP*, GRAPHICS*, int, int);
void Draw_BMP(BMP*, char*, int, int, int);
void Delete_BMP(BMP*);

#endif
