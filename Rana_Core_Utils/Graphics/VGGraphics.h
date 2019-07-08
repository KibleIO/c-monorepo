#ifdef VGSUPPORTED
#ifndef _VGGRAPHICS_H
#define _VGGRAPHICS_H

#include <shapes.h>
#include <fontinfo.h>

#include "../Utilities/LOGGING.h"

#include "NotoSans.inc"

#define FIRST_QUADRANT 1
#define SECOND_QUADRANT 2
#define THIRD_QUADRANT 3
#define FOURTH_QUADRANT 4

struct ColorArray {
	uint8_t color[4];
};

struct VGGRAPHICS {
	static int w;
	static int h;
	Fontinfo font;
};

ColorArray Int_To_Color_Array(int icolor);

void Initialize_VGGRAPHICS();

void Finish_VGGRAPHICS();

void Start_Picture_VGGRAPHICS();

void End_Picture_VGGRAPHICS();

void Clear_Screen_VGGRAPHICS(uint8_t color[4]);

void Line_VGGRAPHICS(
int x1, int y1, int x2, int y2, VGfloat thickness, uint8_t color[4]);

void Rect_VGGRAPHICS(
int x, int y, int w, int h, int rounding, VGfloat thickness, uint8_t color[4]);

void Rect_Filled_VGGRAPHICS(
int x, int y, int w, int h, int rounding, uint8_t color[4]);

void Ellipse_VGGRAPHICS(
int x, int y, int w, int h, VGfloat thickness, uint8_t color[4]);

void Ellipse_Filled_VGGRAPHICS(
int x, int y, int w, int h, uint8_t color[4]);

void Polygon_VGGRAPHICS(
VGfloat* xp, VGfloat* yp, int np, VGfloat thickness, uint8_t color[4]);

void Polygon_Filled_VGGRAPHICS(
VGfloat* xp, VGfloat* yp, int np, uint8_t color[4]);

void Text_VGGRAPHICS(
int x, int y, const char* text, int h, uint8_t color[4]);

void Image_VGGRAPHICS(int x, int y, int w, int h, uint8_t* data);

void Quarter_Arc_Filled_VGGRAPHICS(
int x, int y, int w, int h, int q, uint8_t color[4]);
#endif
#endif
