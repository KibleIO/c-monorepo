#define STB_TRUETYPE_IMPLEMENTATION
#include "FONT.h"

void Initialize_Font(FONT* &font, const char* fname, float text_height){
	font = new FONT;
	font->Baked_glyphs = new BAKED_GLYPH[96];
	font->Font_name = string(fname);

	log_dbg("Initializing font : " + string(fname) + ", size: " + to_string(text_height));

	float scale;
	int ascent, baseline, c_w, c_h, c_xoff, c_yoff, advance, lsb;
	unsigned char ttf_buffer[1<<20];
	if (fread(ttf_buffer, 1, 1<<20, fopen(fname, "rb")) < 0){
		log_err("Failed to load font\n");
		return;
	}
	log_dbg("Successfully opened font file");


	stbtt_InitFont(&font->Font_info, ttf_buffer, 0);
	scale = stbtt_ScaleForPixelHeight(&font->Font_info, text_height);
	stbtt_GetFontVMetrics(&font->Font_info, &ascent, 0, 0);
	font->Baseline = (int) ascent * scale;

	for (int i = 0; i < 96; i++){
		font->Baked_glyphs[i].Bitmap = stbtt_GetCodepointBitmap(&font->Font_info,scale,scale, 32+i,&c_w, &c_h, &c_xoff, &c_yoff);
		font->Baked_glyphs[i].W = c_w;
		font->Baked_glyphs[i].H = c_h;
		font->Baked_glyphs[i].Xoff = c_xoff;
		font->Baked_glyphs[i].Yoff = c_yoff;
		stbtt_GetCodepointHMetrics(&font->Font_info, char(32+i), &advance, &lsb);
		font->Baked_glyphs[i].Advance = advance * scale;
	}

	log_dbg("Font prepared");
}

void Delete_Font(FONT* &font){
	log_dbg("Deleted font: " + font->Font_name);
	if (font) {
			if (font->Baked_glyphs) {
				delete font->Baked_glyphs;
			}
		delete font;
	}
}
