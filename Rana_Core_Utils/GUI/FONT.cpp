//PLATFORMS: Linux, Windows, OSX

#define STB_TRUETYPE_IMPLEMENTATION

#include "FONT.h"

void Initialize_Font(FONT* font, const char* fname, float text_height){
	font->Baked_glyphs = new BAKED_GLYPH[FONT_GLYPH_COUNT];
	font->Font_name = string(fname);

	log_dbg("Initializing font : " + string(fname) + ", size: " + 
	to_string(text_height));

	float scale;
	int ascent, baseline, c_w, c_h, c_xoff, c_yoff, advance, lsb;
	unsigned char ttf_buffer[1<<20];
	if (fread(ttf_buffer, 1, 1<<20, fopen(fname, "rb")) < 0){
		log_err("Failed to load font\n");
		return;
	}

	stbtt_InitFont(&font->Font_info, ttf_buffer, 0);
	scale = stbtt_ScaleForPixelHeight(&font->Font_info, text_height);
	stbtt_GetFontVMetrics(&font->Font_info, &ascent, 0, 0);
	font->Baseline = (int) ascent * scale;

	for (int i = 0; i < FONT_GLYPH_COUNT; i++){
		font->Baked_glyphs[i].Bitmap =
		stbtt_GetCodepointBitmap(&font->Font_info, scale, scale, 32+i, &c_w,
		&c_h, &c_xoff, &c_yoff);
		font->Baked_glyphs[i].W = c_w;
		font->Baked_glyphs[i].H = c_h;
		font->Baked_glyphs[i].Xoff = c_xoff;
		font->Baked_glyphs[i].Yoff = c_yoff;
		stbtt_GetCodepointHMetrics(
		&font->Font_info, char(32+i), &advance, &lsb);
		font->Baked_glyphs[i].Advance = advance * scale;
	}
}

void Delete_Font(FONT* font){
	for (int i = 0; i < FONT_GLYPH_COUNT; i++) {
		stbtt_FreeBitmap(
		font->Baked_glyphs[i].Bitmap, font->Font_info.userdata);
	}
	delete [] font->Baked_glyphs;
	log_dbg("Deleted font: " + font->Font_name);
}
