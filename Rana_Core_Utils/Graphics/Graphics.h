//PLATFORMS: Linux, Windows, OSX

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <algorithm>
#include <iostream>

#include <limits.h>
#include <math.h>

#include "Polygon.h"
#include "SCREEN_DIM.h"
#include "../Utilities/LOGGING.h"
#include "../GUI/nk_ops.h"

#define ABS(x) ((x >= 0) ? x : -x)

#define FIRST_QUADRANT 1
#define SECOND_QUADRANT 2
#define THIRD_QUADRANT 3
#define FOURTH_QUADRANT 4

using namespace std;

typedef int Color;

struct GRAPHICS {
	SCREEN_DIM screen_dim;
	int		Width_Clip;
	int		Height_Clip;
	int		X_clip;
	int		Y_clip;
	uint8_t*	Buffer;
	long*		ContourX;
	bool		Transparent;
};

void Initialize_GRAPHICS(GRAPHICS*, uint8_t*, SCREEN_DIM);
void Delete_GRAPHICS(GRAPHICS*);

void SwapBuffers_GRAPHICS(GRAPHICS*, uint8_t*);

void ClearScreen_GRAPHICS(GRAPHICS*, int color);

void Set_Clip_GRAPHICS(GRAPHICS*, int x, int y, int width, int height);
void Clip_GRAPHICS(GRAPHICS*, int&, int&, int&, int&);
void ScanLine_GRAPHICS(GRAPHICS*, long x1, long y1, long x2, long y2);

void DrawPoint_GRAPHICS(GRAPHICS*, int x, int y, int color);
void DrawPoint_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int color);
void DrawPoint_Transparent_GRAPHICS(GRAPHICS*, int x, int y, int col1);
void DrawPoint_Transparent_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int col1);
void DrawPoint_Opaque_GRAPHICS(GRAPHICS*, int x, int y, int color);
void DrawPoint_Opaque_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int color);

void DrawLine_GRAPHICS(GRAPHICS*, int x1, int y1, int x2, int y2, int color);
void DrawLine_Transparent_GRAPHICS(GRAPHICS*, int x1, int y1, int x2, int y2, int color);
void DrawLine_Opaque_GRAPHICS(GRAPHICS*, int x1, int y1, int x2, int y2, int color);

void FillTriangle_GRAPHICS(GRAPHICS*, int x0, int y0, int x1, int y1, int x2, int y2, int color);
void FillTriangle_Transparent_GRAPHICS(GRAPHICS*, int x0, int y0, int x1, int y1, int x2, int y2, int color);
void FillTriangle_Opaque_GRAPHICS(GRAPHICS*, int x0, int y0, int x1, int y1, int x2, int y2, int color);

void DrawLine_Height_GRAPHICS(GRAPHICS*, int x, int y, int height, int color);
void DrawLine_Height_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int height, int color);
void DrawLine_Height_Transparent_GRAPHICS(GRAPHICS*, int x, int y, int height, int color);
void DrawLine_Height_Transparent_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int height, int color);
void DrawLine_Height_Opaque_GRAPHICS(GRAPHICS*, int x, int y, int height, int color);
void DrawLine_Height_Opaque_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int height, int color);

void DrawLine_Width_GRAPHICS(GRAPHICS*, int x, int y, int width, int color);
void DrawLine_Width_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int width, int color);
void DrawLine_Width_Transparent_GRAPHICS(GRAPHICS*, int x, int y, int width, int color);
void DrawLine_Width_Transparent_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int width, int color);
void DrawLine_Width_Opaque_GRAPHICS(GRAPHICS*, int x, int y, int width, int color);
void DrawLine_Width_Opaque_GRAPHICS_UNSAFE(GRAPHICS*, int x, int y, int width, int color);

void DrawSquare_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int color);
void DrawSquare_Transparent_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int color);
void DrawSquare_Opaque_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int color);

void FillSquare_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int color);
void FillSquare_Transparent_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int color);
void FillSquare_Opaque_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int color);

void DrawCircle_GRAPHICS(GRAPHICS*, int x0, int y0, int radius, int color);
void DrawCircle_Transparent_GRAPHICS(GRAPHICS*, int x0, int y0, int radius, int c);
void DrawCircle_Opaque_GRAPHICS(GRAPHICS*, int x0, int y0, int radius, int c);

void FillCircle_GRAPHICS(GRAPHICS*, int, int, int, int);
void FillCircle_Transparent_GRAPHICS(GRAPHICS*, int, int, int, int);
void FillCircle_Opaque_GRAPHICS(GRAPHICS*, int, int, int, int);

void FillPolygon_GRAPHICS(GRAPHICS*, POLYGON&, int);
void FillPolygon_Transparent_GRAPHICS(GRAPHICS*, POLYGON&, int);
void FillPolygon_Opaque_GRAPHICS(GRAPHICS*, POLYGON&, int);

void FillRoundedRect_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int rounding, Color);
	// Draw a filled square with the desired rounding on the edges
void FillQuarterArc_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int quadrant, Color);
	// Fill a quarter of an elliptical arc in the quadrant specified
void DrawThickHorizontalLine_GRAPHICS(GRAPHICS*, int x, int y, int w, int thickness, Color);
	// Draw a line horizontally starting at the TOP LEFT and going w pixels to the right with the desired thickness
void DrawThickVerticalLine_GRAPHICS(GRAPHICS*, int x, int y, int h, int thickness, Color);
	// Draw a line vertically starting at the TOP LEFT and going h pixels down with the desired thickness
void DrawThickRect_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int thickness, Color);
	// Draw an unfilled rect area with the desired border thickness
void DrawThickRoundedRect_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int thickness, int rounding, Color);
	// Draw an unfilled rect area with the desired border thickness and rounding off of the edges
void DrawQuarterArc_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int quadrant, Color);
	// Draw a quarter of an elliptical arc in the quadrant specified
void DrawThickQuarterArc_GRAPHICS(GRAPHICS*, int x, int y, int w, int h, int thickness, int quadrant, Color);

// Clip the given rect against the graphic's current clipping rect
bool Clip_Rect_Against_Clip_GRAPHICS(GRAPHICS*, int& x, int& y, int& w, int& h);
bool Clip_Rect_Against_Screen(SCREEN_DIM, int& x, int& y, int& w, int& h);
bool Clip_Rect(
int clip_x, int clip_y, int clip_w, int clip_h, int& x, int& y, int& w, int& h);
bool Clip_Line(int clip_start, int clip_magnitude, int& start, int& magnitude);

#endif
