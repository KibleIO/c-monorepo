#ifndef RAW_PICTURE_H_
#define RAW_PICTURE_H_

#include "avir.h"
#include "../Graphics/Graphics.h"

// Only resize the picture if either dimension changes by this many pixels
#define RAW_PICTURE_SIGNIFICANT_DIFFERENCE	5

// A raw picture with width and height
// This is the handle in all nk_image structs and image commands
struct RAW_PICTURE {
	string filename;	// Name of the file to init the picture from

	uint8_t* buffer;	// Buffer received from loading the picture
	int32_t width;	// Width of the picture
	int32_t height;	// Height of the picture
};

// FUNCTIONS
RAW_PICTURE Raw_Picture(string filename);
void Initialize_RAW_PICTURE(RAW_PICTURE*, string filename);
void Render_RAW_PICTURE(
RAW_PICTURE*, GRAPHICS*, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void Render_No_Blend_RAW_PICTURE(
RAW_PICTURE*, GRAPHICS*, uint32_t x, uint32_t y, uint32_t w, uint32_t h);
void Render_No_Blend_RAW_PICTURE(RAW_PICTURE* picture);
void Delete_RAW_PICTURE(RAW_PICTURE*);

// Load the picture buffer using the image at the given file
void Load_RAW_PICTURE(RAW_PICTURE*, uint32_t w, uint32_t h);
// Checks the given width/height against the picture's current width/height,
// and only reloads the picture if it needs resizing
void Check_Load_RAW_PICTURE(RAW_PICTURE*, int32_t w, int32_t h);
void Rework_Buffer_RAW_PICTURE(RAW_PICTURE*);

#endif
