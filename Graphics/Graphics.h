#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <cmath>
#include <iostream>
#include <string>
#include <limits.h>
#include "Polygon.h"
#include "Image.h"
#include "../Utilities/Integer.h"

#define ABS(x) ((x >= 0) ? x : -x)

using namespace std;

typedef int Color;
static Color _BLUE       = 0x000000FF;
static Color _NAVY_BLUE  = 0x000D6386;
static Color _GREEN      = 0x0000FF00;
static Color _RED        = 0x00FF0000;
static Color _YELLOW     = 0x0000FFFF;
static Color _PURPLE     = 0x00FF00FF;
static Color _PINK       = 0x00CC0099;
static Color _TEAL       = 0x0000ccFF;
static Color _WHITE      = 0x00FFFFFF;
static Color _BLACK      = 0x00000000;
static Color _WHITEISH   = 0x00F5F5F5;
static Color _BOLD_GREEN = 0x00009900;
static Color _LEAF_GREEN = 0x0055AE3A;
static Color _NERF_GREEN = 0x0049E20E;

class Graphics {
	private:
		char* buffer;
		void  ScanLine(long, long, long, long);
		long* ContourX;
	public:
		int  width;
		int  height;
		int  screen_w;
		int  screen_h;
		int  x_c;
		int  y_c;
		int  width_c;
		int  height_c;
		int* buffer_i;
		Graphics           (char *, int, int, int, int);
		void setClip       (int, int, int, int);
		void drawImage     (int, int, Image *);
		void drawImage2    (int, int, Image *);
		void drawPoint     (int, int, Color);
		void drawPoint_wa  (int, int, Color);
		void drawLine      (int, int, int, int, Color);
		void drawSquare    (int, int, int, int, Color);
		void fillSquare    (int, int, int, int, Color);
		void drawCircle    (int, int, int, Color);
		void fillCircle    (int, int, int, Color);
		void fillPolygon   (Polygon, Color);
		void fillTriangle  (int, int, int, int, int, int, Color);
		void drawLineWidth (int, int, int, Color);
		void drawLineHeight(int, int, int, Color);
		void Swapbuffers   (char*);
		inline void SetPriority(int p) {
			p           = p << 24;
    		_BLUE       = (_BLUE       & 0x00FFFFFF) | p;
    		_NAVY_BLUE  = (_NAVY_BLUE  & 0x00FFFFFF) | p;
    		_GREEN      = (_GREEN      & 0x00FFFFFF) | p;
    		_RED        = (_RED        & 0x00FFFFFF) | p;
    		_YELLOW     = (_YELLOW     & 0x00FFFFFF) | p;
    		_PURPLE     = (_PURPLE     & 0x00FFFFFF) | p;
    		_PINK       = (_PINK       & 0x00FFFFFF) | p;
    		_TEAL       = (_TEAL       & 0x00FFFFFF) | p;
    		_WHITE      = (_WHITE      & 0x00FFFFFF) | p;
    		_BLACK      = (_BLACK      & 0x00FFFFFF) | p;
    		_WHITEISH   = (_WHITEISH   & 0x00FFFFFF) | p;
    		_BOLD_GREEN = (_BOLD_GREEN & 0x00FFFFFF) | p;
    		_LEAF_GREEN = (_LEAF_GREEN & 0x00FFFFFF) | p;
    		_NERF_GREEN = (_NERF_GREEN & 0x00FFFFFF) | p;
		}
		char* GetBuffer() { return buffer; }
		~Graphics() { delete ContourX; }
};

#endif /* GRAPHICS_H_ */
