#include "Image.h"

void Initialize_Image(IMAGE* image, string imageDir) {
	image->image = {};
	image->imageInitialized = false;
	image->imageDirectory = imageDir;
	image->width = 0;
	image->height = 0;
}

void Render_Image(
IMAGE* image, struct nk_context* ctx, RECT_TRANSFORM transform) {
	PANEL group = Panel(Nk_Window_Style());

	Check_And_Load_Image_In_Current_Rect(image, ctx, transform);

	// Layout the area for the image
	if (
	Layout_Transformed_Area(ctx, transform)) {
		nk_image(ctx, nk_image_ptr(&image->image));
		nk_group_end(ctx);
	}

	Delete_Panel(&group);
}

void Render_Image_With_Buffer(
IMAGE* image, struct nk_context* ctx, RECT_TRANSFORM transform) {
	nk_label(ctx, "", 0);
	Render_Image(image, ctx, transform);
}

void Delete_Image(IMAGE* image) {
	Delete_BMP(&image->image);
}

void Load_Image(IMAGE* image, struct nk_vec2 size) {
	if (image->imageInitialized) {
		Delete_BMP(&image->image);
	}

	image->width = size.x;
	image->height = size.y;

	Initialize_BMP(
	&image->image, image->imageDirectory, image->width, image->height);
	image->image.Transparent = true;

	image->imageInitialized = true;
}

// Load the image only if the rect is a reasonable size
// and is a different size from the current image
void Check_And_Load_Image(IMAGE* image, struct nk_vec2 size) {
	if (Size_Is_Reasonable(size) && Size_Is_New(image, size)) {
		Load_Image(image, size);
	}
}

void Load_Image_In_Current_Rect(IMAGE* image, struct nk_context* ctx) {
	Load_Image(image, nk_rect_size(nk_widget_bounds(ctx)));
}

void Check_And_Load_Image_In_Current_Rect(
IMAGE* image, struct nk_context* ctx, RECT_TRANSFORM transform) {
	Check_And_Load_Image(
	image, nk_rect_size(Transform_Rect(nk_widget_bounds(ctx), transform)));
}

bool Size_Is_Reasonable(struct nk_vec2 size) {
	return
	size.x > 0 && size.x <= GUI::Width && size.y > 0 && size.y <= GUI::Height;
}
bool Size_Is_New(const IMAGE* image, struct nk_vec2 size) {
	return
	abs(size.x - image->width) > SIGNIFICANT_DIFFERENCE &&
	abs(size.y - image->height) > SIGNIFICANT_DIFFERENCE;
}
