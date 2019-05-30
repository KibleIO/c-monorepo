#include "Image.h"

void Initialize_Image(
IMAGE* image, string imageDir, uint32_t width, uint32_t height) {
	image->image = NULLIFY;
	Initialize_BMP(&image->image, imageDir, width, height);
	image->image.Transparent = true;

	image->width = width;
	image->height = height;
}

void Render_Image(IMAGE* image, struct nk_context* ctx) {
	// Get the current rect being laid out
	struct nk_rect current_rect = nk_widget_bounds(ctx);

	// Get the new x-y coordinates of the image
	uint32_t image_x = current_rect.x + ((current_rect.w - image->width) / 2);
	uint32_t image_y = current_rect.y + ((current_rect.h - image->height) / 2);

	// Get the x-y offset
	uint32_t x_off = image_x - current_rect.x;
	uint32_t y_off = image_y - current_rect.y;

	if (nk_group_begin(ctx, "image panel", NK_WINDOW_NO_SCROLLBAR)) {

		// Layout the row with a left buffer and upper buffer
		// to push the image into the center of the column
		Layout_Row_Single(ctx, LAYOUT_TYPE_STATIC,
		Layout_Size(y_off, image->height), Layout_Size(x_off, image->width));

		// Layout a buffer, then the image
		if(x_off > 0) {
			nk_label(ctx, "", 0);
		}
		nk_image(ctx, nk_image_ptr(&image->image));

		nk_group_end(ctx);
	}
}

void Render_Image_With_Buffer(IMAGE* image, struct nk_context* ctx) {
	nk_label(ctx, "", 0);
	Render_Image(image, ctx);
}

void Delete_Image(IMAGE* image) {
	Delete_BMP(&image->image);
}
