#include "Image.h"

void Initialize_Image(
IMAGE* image, string imageDir, float width_ratio, float height_ratio) {
	image->image = {};
	image->imageInitialized = false;
	image->imageDirectory = imageDir;
	image->widthRatio = width_ratio;
	image->heightRatio = height_ratio;
	image->width = 0;
	image->height = 0;
}

void Render_Image(IMAGE* image, struct nk_context* ctx) {
	Check_And_Load_Image_In_Current_Rect(image, ctx);

	if (nk_group_begin(ctx, "image panel", NK_WINDOW_NO_SCROLLBAR)) {

		// Layout the row with a left buffer and upper buffer
		// to push the image into the center of the column
		Layout_Row_Single(
		ctx, Buffer_And_Breadth(
		Ratio_Of_Total((1.0 - image->heightRatio) / 2.0),
		Ratio_Of_Total(image->heightRatio)),
		Buffer_And_Breadth(
		Ratio_Of_Total((1.0 - image->widthRatio) / 2.0),
		Ratio_Of_Total(image->widthRatio)));

		nk_label(ctx, "", 0);
		nk_image(ctx, nk_image_ptr(&image->image));
	}

	nk_group_end(ctx);
}

void Render_Image_With_Buffer(IMAGE* image, struct nk_context* ctx) {
	nk_label(ctx, "", 0);
	Render_Image(image, ctx);
}

void Delete_Image(IMAGE* image) {
	Delete_BMP(&image->image);
}

void Load_Image(IMAGE* image, struct nk_rect parent) {
	image->width = parent.w * image->widthRatio;
	image->height = parent.h * image->heightRatio;

	Initialize_BMP(
	&image->image, image->imageDirectory, image->width, image->height);
	image->imageInitialized = true;
	image->image.Transparent = true;
}

void Check_And_Load_Image(IMAGE* image, struct nk_rect parent) {
	if (!image->imageInitialized) {
		Load_Image(image, parent);
	}
}

// TEMP
#include "../../Utilities/utilities.h"

void Load_Image_In_Current_Rect(IMAGE* image, struct nk_context* ctx) {
	Load_Image(image, nk_widget_bounds(ctx));
}

void Check_And_Load_Image_In_Current_Rect(
IMAGE* image, struct nk_context* ctx) {
	Check_And_Load_Image(image, nk_widget_bounds(ctx));
}
