#ifndef IMAGE_H_
#define IMAGE_H_

#include "Panel.h"
#include "../NK_BASE.h"
#include "../../Utilities/Stuff.h"
#include "../Layout/Layout_Engine.h"
#include "../Layout/Rect_Transform.h"
#include "../Texture.h"

struct IMAGE {
	struct TEXTURE* texture;
};

void Initialize_Image(IMAGE*, TEXTURE*);

// Render the image in the center of the current column
void Render_Image(IMAGE*, struct nk_context*, RECT_TRANSFORM);
void Render_Image_With_Buffer(IMAGE*, struct nk_context*, RECT_TRANSFORM);
// Draw the image apart from the current layout space
// with the exact coordinates given
void Draw_Image(IMAGE*, struct nk_context*, struct nk_rect);

void Delete_Image(IMAGE*);

#endif
