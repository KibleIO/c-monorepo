#include "Raw_Picture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

RAW_PICTURE Raw_Picture(string filename) {
	RAW_PICTURE picture;
	Initialize_RAW_PICTURE(&picture, filename);
	return picture;
}

void Initialize_RAW_PICTURE(RAW_PICTURE* picture, string filename) {
	picture->filename = filename;
	picture->buffer = NULL;
	picture->width = 0;
	picture->height = 0;
}

void Render_RAW_PICTURE(
RAW_PICTURE* picture, GRAPHICS* graphics, uint32_t x, uint32_t y, uint32_t w,
uint32_t h) {
	Render_RAW_PICTURE(
	picture, graphics, x, y, w, h, DrawPoint_Transparent_GRAPHICS_UNSAFE);
}

void Render_No_Blend_RAW_PICTURE(
RAW_PICTURE* picture, GRAPHICS* graphics, uint32_t x, uint32_t y, uint32_t w,
uint32_t h) {
	Render_RAW_PICTURE(
	picture, graphics, x, y, w, h, DrawPoint_Opaque_GRAPHICS_UNSAFE);
}

void Render_RAW_PICTURE(
RAW_PICTURE* picture, GRAPHICS* graphics, uint32_t x, uint32_t y, uint32_t w,
uint32_t h, void (*drawpoint)(GRAPHICS*, int, int, int)) {
	// Check to see if the picture needs to be reloaded and resized
	Check_Load_RAW_PICTURE(picture, w, h);

	// Convert arguments to l-values that can be passed by reference
	// to "Clip_Rect_GRAPHICS" below
	int lx = x;
	int ly = y;
	int lw = w;
	int lh = h;

	if (Clip_Rect_Against_Clip_GRAPHICS(graphics, lx, ly, lw, lh)) {
		// Relative coordinates inside the picture's rect
		uint32_t picture_x = lx - x;
		uint32_t picture_y = ly - y;

		for (uint32_t diff_y = 0; diff_y < (uint32_t)lh; diff_y++) {
			for (uint32_t diff_x = 0; diff_x < (uint32_t)lw; diff_x++) {
				int index = (picture_y + diff_y) * picture->width +
				(picture_x + diff_x);

				drawpoint(
				graphics, lx + diff_x, ly + diff_y,
				*((int*)picture->buffer + index));
			}
		}
	}
}

void Delete_RAW_PICTURE(RAW_PICTURE* picture) {
	if (picture->buffer != NULL) {
		delete [] picture->buffer;
		picture->buffer = NULL;
	}
}

void Load_RAW_PICTURE(
RAW_PICTURE* picture, uint32_t width, uint32_t height) {
	int base_width;
	int base_height;
	int filetype_flag;
	uint32_t buffer_size = width * height * 4;
	uint8_t* base_buffer;
	avir::CImageResizer<> air(8);

	// Load the file
	base_buffer = stbi_load(
	picture->filename.c_str(), &base_width, &base_height, &filetype_flag, 4);

	// If no error occurred, enter
	if (base_buffer && filetype_flag == 4) {
		// If necessary, delete the existing buffer and re-initialize it
		Delete_RAW_PICTURE(picture);
		picture->buffer = new uint8_t[buffer_size];

		// Resize the base buffer and copy it into the picture's buffer
		air.resizeImage(
		base_buffer, base_width, base_height, 0,
		picture->buffer, width, height, 4, 0);

		// Store the width/height of the image
		picture->width = width;
		picture->height = height;

		// Rearrange rgba channels
		Rework_Buffer_RAW_PICTURE(picture);
	}
	else {
		// Initialize the raw picture buffer with an "empty" texture
		uint32_t* buffer = new uint32_t[width * height]{0xffff00ff};
		picture->buffer = (uint8_t*)buffer;
		picture->width = width;
		picture->height = height;

		if (!base_buffer) {
			log_err(string("Image file does not exist: ") + picture->filename);
		}
		else {
			log_err(string("Image file is wrong format: ") + picture->filename);
		}
	}

	// Free up the buffer generated by stbi
	if (base_buffer != NULL) {
		stbi_image_free(base_buffer);
	}
}

void Check_Load_RAW_PICTURE(
RAW_PICTURE* picture, int32_t width, int32_t height) {
	if (
	abs(picture->width - width) > RAW_PICTURE_SIGNIFICANT_DIFFERENCE ||
	abs(picture->height - height) > RAW_PICTURE_SIGNIFICANT_DIFFERENCE) {
		Load_RAW_PICTURE(picture, width, height);
	}
}

void Rework_Buffer_RAW_PICTURE(RAW_PICTURE* picture) {
	int byte_capture;

	for (int32_t x = 0; x < picture->width; x++) {
		for (int32_t y = 0; y < picture->height; y++) {
			// Capture 4 bytes in the buffer
			byte_capture = ((int*)picture->buffer)[y * picture->width + x];

			// Swap byte 1 and 3 (not sure exactly why)
			byte_capture = 	((byte_capture & 0xff000000)) | //______AA
							((byte_capture & 0x00ff0000) >> 16) | //____RR__
							((byte_capture & 0x0000ff00)) | //__GG____
							((byte_capture & 0x000000ff) << 16);

			// Put the capture back
			((int*)picture->buffer)[y * picture->width + x] = byte_capture;
		}
	}
}
