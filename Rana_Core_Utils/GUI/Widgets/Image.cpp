#include "Image.h"

void Initialize_Image(IMAGE* image, TEXTURE* texture) {
	image->texture = texture;
}

void Render_Image(
IMAGE* image, struct nk_context* ctx, RECT_TRANSFORM transform) {
	// Layout the area for the image
	if (Layout_Transformed_Area(ctx, transform)) {
		nk_image(ctx, image->texture->data);
		nk_group_end(ctx);
	}
}

void Render_Image_With_Buffer(
IMAGE* image, struct nk_context* ctx, RECT_TRANSFORM transform) {
	nk_label(ctx, "", 0);
	Render_Image(image, ctx, transform);
}

void Draw_Image(IMAGE* image, struct nk_context* ctx, struct nk_rect bounds) {
	nk_draw_image(
	nk_window_get_canvas(ctx), bounds, &image->texture->data, WHITE);
}

void Delete_Image(IMAGE* image) {
	(void)image;
}
