#ifndef IMAGE_H_
#define IMAGE_H_

#include "Panel.h"
#include "../NK_BASE.h"
#include "../../Utilities/Stuff.h"
#include "../Layout/Layout_Engine.h"
#include "../Layout/Rect_Transform.h"
#include "../Style.h"

#define SIGNIFICANT_DIFFERENCE 5//pixels

struct IMAGE {
	struct nk_image picture;
};

void Initialize_Image(IMAGE*, string filename, uint32_t w = 0, uint32_t h = 0);

// Render the image in the center of the current column
void Render_Image(IMAGE*, struct nk_context*, RECT_TRANSFORM);
void Render_Image_With_Buffer(IMAGE*, struct nk_context*, RECT_TRANSFORM);
void Delete_Image(IMAGE*);

#endif
