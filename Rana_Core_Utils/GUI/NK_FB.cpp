#ifndef __arm__
#define NK_IMPLEMENTATION
#include "NK_FB.h"

float Font_Get_Text_Width(
nk_handle handle, float height, const char* text, int length) {
	(void)height;
	BAKED_GLYPH* glyphs = (BAKED_GLYPH*)handle.ptr;
	int _x = 0;
	for (int i = 0; i < length; i++) {
		_x += glyphs[int(text[i]) - 32].Advance;
	}
	return _x;
}

void Draw_Text(NK_GEN* nk_fb, const struct nk_command_text* command) {
	const struct nk_command_text* t = (const struct nk_command_text*)command;
	string str = string(t->string);
	int _x = t->x;
	int _y = t->y + nk_fb->fonts[nk_fb->current_font].userFont.Baseline;

	unsigned char fg[] = {
		t->foreground.b,
		t->foreground.g,
		t->foreground.r,
		t->foreground.a
	};
	unsigned char bg[] = {
		t->background.b,
		t->background.g,
		t->background.r,
		t->background.a
	};

	unsigned char result[] = {0,0,0,0xff};

	union {
		unsigned char bytes[4];
		int data;
	} kk;

	for (unsigned int i = 0; i < str.length(); i++) {
		BAKED_GLYPH* c = &(
		(BAKED_GLYPH*)command->font->userdata.ptr)[int(str[i]) - 32];

		for (int x = 0; x < c->W; x++){
			for (int y = 0; y < c->H; y++){
				unsigned int alpha = c->Bitmap[(y*c->W)+x];
				unsigned int inv_alpha = 255 - alpha;

				result[0] = (
				unsigned char)((alpha * fg[0] + inv_alpha * bg[0]) >> 8);

				result[1] = (
				unsigned char)((alpha * fg[1] + inv_alpha * bg[1]) >> 8);

				result[2] = (
				unsigned char)((alpha * fg[2] + inv_alpha * bg[2]) >> 8);

				result[3] = (
				unsigned char)((alpha * fg[3] + inv_alpha * bg[3]) >> 8);

				memcpy(kk.bytes, result, 4);
				DrawPoint_GRAPHICS(
				nk_fb->Graphics_Handle, _x + x + c->Xoff, _y + y + c->Yoff,
				kk.data);
			}
		}
		_x += c->Advance;
	}
}

struct nk_image Load_Image_NK_GEN(
string filename, uint32_t width, uint32_t height) {
	RAW_PICTURE* picture = new RAW_PICTURE;
	Initialize_RAW_PICTURE(picture, filename);

	// Bypass compiler errors
	(void)width;
	(void)height;

	return nk_image_ptr(picture);
}

void Free_Image_NK_GEN(struct nk_image* image) {
	RAW_PICTURE* picture = (RAW_PICTURE*)image->handle.ptr;
	Delete_RAW_PICTURE(picture);
	delete picture;
}

void Render_NK_GEN(NK_GEN* nk_fb) {
	const struct nk_command* command;

	nk_foreach(command, nk_fb->NK_Context) {
		switch (command->type) {
			case NK_COMMAND_NOP: break;
			case NK_COMMAND_SCISSOR: {
				const struct nk_command_scissor* s = (
				const struct nk_command_scissor*)command;

				Set_Clip_GRAPHICS(
				nk_fb->Graphics_Handle, s->x, s->y, s->w, s->h);
				break;
			}
			case NK_COMMAND_LINE: {
				const struct nk_command_line* l = (
				const struct nk_command_line*)command;

				DrawLine_GRAPHICS(
				nk_fb->Graphics_Handle, l->begin.x, l->begin.y, l->end.x,
				l->end.y, fromNkColor(l->color));

				break;
			}
			case NK_COMMAND_RECT: {
				const struct nk_command_rect* r = (
				const struct nk_command_rect*)command;

				DrawThickRect_GRAPHICS(
				nk_fb->Graphics_Handle, r->x, r->y, r->w, r->h,
				r->line_thickness, fromNkColor(r->color));

				break;
			}
			case NK_COMMAND_RECT_FILLED: {
				const struct nk_command_rect_filled* r = (
				const struct nk_command_rect_filled*)command;

				FillRoundedRect_GRAPHICS(
				nk_fb->Graphics_Handle, r->x, r->y, r->w, r->h, r->rounding,
				fromNkColor(r->color));

				break;
			}
			case NK_COMMAND_CIRCLE: {
				const struct nk_command_circle* c = (
				const struct nk_command_circle*)command;

				DrawCircle_GRAPHICS(
				nk_fb->Graphics_Handle, c->x + (c->w / 2), c->y + (c->w / 2),
				c->w / 2, fromNkColor(c->color));

				break;
			}
			case NK_COMMAND_CIRCLE_FILLED: {
				const struct nk_command_circle_filled* c = (
				const struct nk_command_circle_filled*)command;

				FillCircle_GRAPHICS(
				nk_fb->Graphics_Handle, c->x + (c->w / 2), c->y + (c->w / 2),
				c->w / 2, fromNkColor(c->color));

				break;
			}
			case NK_COMMAND_TRIANGLE: {
				const struct nk_command_triangle* t = (
				const struct nk_command_triangle*)command;
				(void)t;
				break;
			}
			case NK_COMMAND_TRIANGLE_FILLED: {
				const struct nk_command_triangle_filled* t = (
				const struct nk_command_triangle_filled*)command;

				POLYGON tri;
				tri.addPoint(t->a.x, t->a.y);
				tri.addPoint(t->b.x, t->b.y);
				tri.addPoint(t->c.x, t->c.y);

				FillPolygon_GRAPHICS(
				nk_fb->Graphics_Handle, tri, fromNkColor(t->color));

				break;
			}
			case NK_COMMAND_POLYGON: {
				const struct nk_command_polygon* p = (
				const struct nk_command_polygon*)command;
				(void)p;
				break;
			}
			case NK_COMMAND_POLYGON_FILLED: {
				const struct nk_command_polygon_filled* p = (
				const struct nk_command_polygon_filled*)command;
				(void)p;
				break;
			}
			case NK_COMMAND_POLYLINE: {
				break;
			}
			case NK_COMMAND_TEXT: {
				const struct nk_command_text* t = (
				const struct nk_command_text*)command;
				Draw_Text(nk_fb, t);
				break;
			}
			case NK_COMMAND_CURVE:
			case NK_COMMAND_RECT_MULTI_COLOR:
			case NK_COMMAND_IMAGE: {
				const struct nk_command_image* image = (
				const struct nk_command_image*)command;

				RAW_PICTURE* picture = (RAW_PICTURE*)image->img.handle.ptr;
				if (picture) {
					Render_RAW_PICTURE(
					picture, nk_fb->Graphics_Handle,
					image->x, image->y, image->w, image->h);
				} else {
					log_err("missing image");
				}

				break;
			}
			case NK_COMMAND_ARC:
			case NK_COMMAND_ARC_FILLED:
			case NK_COMMAND_CUSTOM:
			default:
				break;
		}
	}

	nk_clear(nk_fb->NK_Context);
	// sets clip to full 0, 0, width, height
	Set_Clip_GRAPHICS(nk_fb->Graphics_Handle, -1, -1, -1, -1);

	Render_FB_RENDERER(nk_fb->render_context, nk_fb->Graphics_Handle_Buffer);
}

void Initialize_NK_GEN(NK_GEN* nk_fb) {
	*nk_fb = {};

	nk_fb->fonts			= NULL;
	nk_fb->number_of_fonts	= 0;
	nk_fb->current_font		= -1;
	nk_fb->NK_Context		= new nk_context;
	nk_fb->render_context	= NULL;

	if (!Construct_FB_RENDERER(nk_fb->render_context)) {
		log_err("render context was not initialized correctly");
		return;
	}

	nk_fb->width					= Get_Width_FB_RENDERER(
	nk_fb->render_context);

	nk_fb->height					= Get_Height_FB_RENDERER(
	nk_fb->render_context);

	nk_fb->Graphics_Handle			= new GRAPHICS;
	nk_fb->Graphics_Handle_Buffer	=
	new uint8_t[nk_fb->width * nk_fb->height * 4];

	Initialize_GRAPHICS(
	nk_fb->Graphics_Handle, nk_fb->Graphics_Handle_Buffer, nk_fb->width,
	nk_fb->height);
}

uint32_t Get_Width_NK_GEN(NK_GEN* nk_fb) {
	return nk_fb->width;
}

uint32_t Get_Height_NK_GEN(NK_GEN* nk_fb) {
	return nk_fb->height;
}

void Load_Fonts_NK_GEN(
NK_GEN* nk_fb, string font_path, uint32_t* font_heights,
uint32_t total_font_heights) {
	if (nk_fb->fonts != NULL) {
		delete nk_fb->fonts;
	}
	nk_fb->fonts 			= new GUI_FONT[total_font_heights];
	nk_fb->number_of_fonts	= total_font_heights;
	nk_fb->current_font		= 0;

	for (uint32_t i = 0; i < total_font_heights; i++) {
		Initialize_Font(
		&nk_fb->fonts[i].userFont, font_path.c_str(), font_heights[i]);

		nk_fb->fonts[i].nkFont.userdata.ptr	=
		nk_fb->fonts[i].userFont.Baked_glyphs;

		nk_fb->fonts[i].nkFont.height			= font_heights[i];
		nk_fb->fonts[i].nkFont.width			= Font_Get_Text_Width;
	}

	nk_init_default(
	nk_fb->NK_Context, &nk_fb->fonts[nk_fb->current_font].nkFont);
}

void Set_Font_NK_GEN(NK_GEN* nk_fb, uint32_t font_index) {
	if (font_index < nk_fb->number_of_fonts) {
		nk_fb->current_font = font_index;
		nk_style_set_font(nk_fb->NK_Context, &nk_fb->fonts[font_index].nkFont);
	}
}

void Delete_NK_GEN(NK_GEN* nk_fb) {
	if (nk_fb->render_context) {
		Delete_FB_RENDERER(nk_fb->render_context);
	}
	if (nk_fb->NK_Context) {
		log_dbg("deleting nk context");
		nk_free(nk_fb->NK_Context);
		delete nk_fb->NK_Context;
	}
	if (nk_fb->fonts) {
		log_dbg("deleting gui fonts");
		for (uint32_t i = 0; i < nk_fb->number_of_fonts; i++) {
			Delete_Font(&nk_fb->fonts[i].userFont);
		}
		delete [] nk_fb->fonts;
	}
	if (nk_fb->Graphics_Handle_Buffer) {
		log_dbg("deleting graphics handle buffer");
		delete [] nk_fb->Graphics_Handle_Buffer;
	}
	if (nk_fb->Graphics_Handle) {
		log_dbg("deleting graphics handle");
		Delete_GRAPHICS(nk_fb->Graphics_Handle);
		delete nk_fb->Graphics_Handle;
	}

	*nk_fb = {};
}

// Given rgba components, gives a color digestible by the Graphics class
Color rgba(char r, char g, char b, char a) {
	Color returnme;

	((char*)&returnme)[0] = b;
	((char*)&returnme)[1] = g;
	((char*)&returnme)[2] = r;
	((char*)&returnme)[3] = a;

	return returnme;
}

Color fromNkColor(const struct nk_color& color) {
	return rgba(color.r, color.g, color.b, color.a);
}

#endif
