#include "Mouse_UI.h"

/*
MOUSE IMAGE
*/

void Load_MOUSE_IMAGE(
MOUSE_IMAGE* image, struct nk_context* ctx, enum nk_style_cursor type,
string image_path, struct nk_vec2 size) {
	TEXTURE* texture = Load_Image_GUI(image_path, size.x, size.y);

	if (texture != NULL) {
		image->path = image_path;
		image->cursor.img = texture->data;
		image->cursor.size = size;
		nk_style_load_cursor(ctx, type, &image->cursor);
		image->loaded = true;
	}
}
void Resize_MOUSE_IMAGE(
MOUSE_IMAGE* image, struct nk_context* ctx, enum nk_style_cursor type,
struct nk_vec2 size) {
	if (
	image->loaded &&
	(image->cursor.size.x != size.x || image->cursor.size.y != size.y)) {
		Load_MOUSE_IMAGE(image, ctx, type, image->path, size);
	}
}

/*
MOUSE UI
*/

void Load_Cursor_MOUSE_UI(
MOUSE_UI* mice, struct nk_context* ctx, enum nk_style_cursor type,
string image_path, struct nk_vec2 size) {
	Load_MOUSE_IMAGE(&mice->images[(int)type], ctx, type, image_path, size);
}

void Resize_Cursor_MOUSE_UI(
MOUSE_UI* mice, struct nk_context* ctx, enum nk_style_cursor type,
struct nk_vec2 size) {
	Resize_MOUSE_IMAGE(&mice->images[(int)type], ctx, type, size);
}
void Resize_All_Cursors_Uniform_MOUSE_UI(
MOUSE_UI* mice, struct nk_context* ctx, struct nk_vec2 sizes) {
	struct nk_vec2 size_ar[NK_CURSOR_COUNT];
	for (uint8_t i = 0; i < NK_CURSOR_COUNT; i++) {
		size_ar[i] = sizes;
	}
	Resize_All_Cursors_MOUSE_UI(mice, ctx, size_ar);
}
void Resize_All_Cursors_MOUSE_UI(
MOUSE_UI* mice, struct nk_context* ctx, const struct nk_vec2* size_ar) {
	for (uint8_t i = 0; i < NK_CURSOR_COUNT; i++) {
		Resize_Cursor_MOUSE_UI(mice, ctx, (nk_style_cursor)i, size_ar[i]);
	}
}
