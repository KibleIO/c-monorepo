#ifndef IMAGE_H_
#define IMAGE_H_

#include "../GUI.h"
#include "../../Utilities/Stuff.h"
#include "../Layout/Layout_Engine.h"
#include "../Style.h"

struct IMAGE {
	BMP image;
	uint32_t width;
	uint32_t height;
};

void Initialize_Image(IMAGE*, string imageDir, uint32_t width, uint32_t height);

// Render the image in the center of the current column
void Render_Image(IMAGE*, struct nk_context*);
void Render_Image_With_Buffer(IMAGE*, struct nk_context*);
void Delete_Image(IMAGE*);

#endif
