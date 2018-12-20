#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <cmath>
#include <iostream>
#include <algorithm>
//#include <string>
#include <limits.h>
#include <Utilities/LOGGING.h>
#include <Graphics/Polygon.h>

#define ABS(x) ((x >= 0) ? x : -x)

using namespace std;

typedef int Color;

struct GRAPHICS {
  int   Width;
  int   Height;
  int   Width_Clip;
  int   Height_Clip;
  int   X_clip;
  int   Y_clip;
  char* Buffer;
  long* ContourX;
  bool  Transparent;
};

void Initialize_GRAPHICS(GRAPHICS*, char*, int, int);
void Delete_GRAPHICS(GRAPHICS*);

void SwapBuffers_GRAPHICS(GRAPHICS*, char* buff);

void ClearScreen_GRAPHICS(GRAPHICS*, int color);

void Set_Clip_GRAPHICS(GRAPHICS*, int x, int y, int width, int height);
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

void FillPolygon_GRAPHICS(GRAPHICS*, Polygon, int);
void FillPolygon_Transparent_GRAPHICS(GRAPHICS*, Polygon, int);
void FillPolygon_Opaque_GRAPHICS(GRAPHICS*, Polygon, int);

#endif
