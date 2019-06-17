#ifndef IMAGE_H_
#define IMAGE_H_

#include "../GUI.h"
#include "../../Utilities/Stuff.h"
#include "../Layout/Layout_Engine.h"
#include "../Style.h"

struct IMAGE {
	BMP image;
	bool imageInitialized;	// True if the image has been initialized

	string imageDirectory;
	float widthRatio;	// Ratio of the layout width that the image fills up
	float heightRatio;	// Ratio of the layout height that the image fills up

	// Current width/height of the image
	uint32_t width;
	uint32_t height;
};

void Initialize_Image(
IMAGE*, string imageDir, float width_ratio, float height_ratio);

// Render the image in the center of the current column
void Render_Image(IMAGE*, struct nk_context*);
void Render_Image_With_Buffer(IMAGE*, struct nk_context*);
void Delete_Image(IMAGE*);

// HELPERS

// Load the image using the given rect
void Load_Image(IMAGE*, struct nk_rect);
void Check_And_Load_Image(IMAGE*, struct nk_rect);

// Load the image in the current widget rect
void Load_Image_In_Current_Rect(IMAGE*, struct nk_context*);
void Check_And_Load_Image_In_Current_Rect(IMAGE*, struct nk_context*);

#endif
