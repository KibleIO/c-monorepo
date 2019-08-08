#ifndef __arm__

#ifndef NK_GLFB_H_
#define NK_GLFB_H_

#include <iostream>
#include <string>
#include "NK_BASE.h"
#include "FONT.h"
#include "../Graphics/Graphics.h"
#include "../Utilities/utilities.h"
#include "Raw_Picture.h"

using namespace std;

struct GUI_FONT {
	FONT userFont;
	nk_user_font nkFont;
};

struct NK_GEN {
	GUI_FONT*		fonts;
	uint32_t		number_of_fonts;
	int32_t			current_font;
	nk_context* 	NK_Context;
	void*			userdata;
	uint8_t*		Graphics_Handle_Buffer;
	GRAPHICS*		Graphics_Handle;
};

float	Font_Get_Text_Width(nk_handle, float, const char*, int);
Color	rgba (char, char, char, char);
Color	fromNkColor(const struct nk_color&);

#endif

#endif
