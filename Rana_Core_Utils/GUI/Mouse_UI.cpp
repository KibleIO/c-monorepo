#include "Mouse_UI.h"

/*
MOUSE IMAGE
*/

void Load_MOUSE_IMAGE(
MOUSE_IMAGE* image, struct nk_context* ctx, enum nk_style_cursor type,
string image_path, uint32_t w, uint32_t h) {
	Delete_MOUSE_IMAGE(image);
	image->cursor.img = Load_Image_NK_GEN(image_path, w, h);
	image->cursor.size = nk_vec2(w, h);
	nk_style_load_cursor(ctx, type, &image->cursor);
	image->loaded = true;
}

void Delete_MOUSE_IMAGE(MOUSE_IMAGE* image) {
	if (image->loaded) {
		Free_Image_NK_GEN(&image->cursor.img);
	}
}

/*
MOUSE UI
*/

void Load_Cursor_MOUSE_UI(
MOUSE_UI* mice, struct nk_context* ctx, enum nk_style_cursor type,
string image_path, uint32_t w, uint32_t h) {
	Load_MOUSE_IMAGE(&mice->images[(int)type], ctx, type, image_path, w, h);
}

void Delete_MOUSE_UI(MOUSE_UI* mice) {
	for (uint32_t i = 0; i < NK_CURSOR_COUNT; i++) {
		Delete_MOUSE_IMAGE(&mice->images[i]);
	}
}
