#include "Image.h"

void Initialize_Image(IMAGE* image, string filename, uint32_t w, uint32_t h) {
	image->picture = Load_Image_NK_GEN(filename, w, h);
}

void Render_Image(
IMAGE* image, struct nk_context* ctx, RECT_TRANSFORM transform) {
	// Layout the area for the image
	if (
	Layout_Transformed_Area(ctx, transform)) {
		nk_image(ctx, image->picture);
		nk_group_end(ctx);
	}
}

void Render_Image_With_Buffer(
IMAGE* image, struct nk_context* ctx, RECT_TRANSFORM transform) {
	nk_label(ctx, "", 0);
	Render_Image(image, ctx, transform);
}

void Draw_Image(IMAGE* image, struct nk_context* ctx, struct nk_rect bounds) {
	nk_draw_image(nk_window_get_canvas(ctx), bounds, &image->picture, WHITE);
}

void Delete_Image(IMAGE* image) {
	// TODO
	// The Image class is totally agnostic to the RAW_PICTURE class,
	// so this delete is impossible.  The delete has to be different
	// based on whichever backend we happen to be using
	//Delete_RAW_PICTURE(&image->picture);

	(void)image;
}
