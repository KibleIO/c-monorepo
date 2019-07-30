#ifndef IMAGE_H_
#define IMAGE_H_

#include "Panel.h"
#include "../GUI.h"
#include "../../Utilities/Stuff.h"
#include "../Layout/Layout_Engine.h"
#include "../Layout/Rect_Transform.h"
#include "../Style.h"

#define SIGNIFICANT_DIFFERENCE 5//pixels

struct IMAGE {
	BMP image;
	bool imageInitialized;

	string imageDirectory;	// Directory used to initialize the image

	// Current width/height of the image
	uint32_t width;
	uint32_t height;
};

void Initialize_Image(IMAGE*, string imageDir);

// Render the image in the center of the current column
void Render_Image(IMAGE*, struct nk_context*, RECT_TRANSFORM);
void Render_Image_With_Buffer(IMAGE*, struct nk_context*, RECT_TRANSFORM);
void Delete_Image(IMAGE*);

// HELPERS

// Load the image using the given rect
void Load_Image(IMAGE*, struct nk_vec2);
void Check_And_Load_Image(IMAGE*, struct nk_vec2);

// Load the image in the current widget rect
void Load_Image_In_Current_Rect(IMAGE*, struct nk_context*);
void Check_And_Load_Image_In_Current_Rect(
IMAGE*, struct nk_context*, RECT_TRANSFORM);

// Check to see if the rect we want to load the image in
// is not too small or too large
bool Size_Is_Reasonable(struct nk_vec2);
// Check if the rect we want to load the image in
// is different from the size of the image as it has already been loaded
bool Size_Is_New(const IMAGE*, struct nk_vec2);

#endif
