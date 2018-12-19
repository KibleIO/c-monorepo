#ifndef FONT_H_
#define FONT_H_

#include <fstream>
#include <iostream>
#include <string>
#include "stb_truetype.h"
#include <Utilities/LOGGING.h>

#define FONT_GLYPH_COUNT 96

struct BAKED_GLYPH {
	unsigned char*	Bitmap;
	int				W;
	int				H;
	int				Xoff;
	int				Yoff;
	int				Advance;
};

struct FONT{
	stbtt_fontinfo	Font_info;
	int 			Baseline;
	BAKED_GLYPH*	Baked_glyphs;
	string			Font_name;
};

void Initialize_Font(FONT* font, const char* fname, float text_height);

void Delete_Font(FONT* font);

#endif
