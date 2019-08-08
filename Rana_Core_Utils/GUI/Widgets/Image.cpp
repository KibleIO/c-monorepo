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

void Delete_Image(IMAGE* image) {
	// TODO
	//Delete_RAW_PICTURE(&image->picture);

	(void)image;
}
