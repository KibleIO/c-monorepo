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

struct BMP {
	char* Data;
	int W, H;
	bool Transparent;
	string name;

#ifdef GRAPHICS_USING_HARDWARE
	VGImage img;
#endif
};

void Initialize_BMP(BMP*, string);
void Initialize_BMP(BMP*, string, int, int);
void resizeBilinear(int*, int*, int, int, int, int);
void Draw_BMP(BMP*, GRAPHICS*, int, int);
void Draw_BMP(BMP*, char*, int, int, int, int);
void Draw_Inverted_Mouse_BMP(BMP*, char*, int, int, int, int);
void Delete_BMP(BMP*);

#endif
