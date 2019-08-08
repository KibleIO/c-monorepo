#ifndef __arm__

#ifndef NK_GLFB_H_
#define NK_GLFB_H_

#include <iostream>
#include <string>
#include "avir.h"
#include "stb_image.h"
#include "NK_BASE.h"
#include "FONT.h"
#include "../Graphics/Graphics.h"

using namespace std;

struct GUI_FONT {
	FONT userFont;
	nk_user_font nkFont;
};

struct FB_RENDERER;

void		Initialize_FB_RENDERER(FB_RENDERER*&);
uint32_t	Get_Width_FB_RENDERER(FB_RENDERER*);
uint32_t	Get_Height_FB_RENDERER(FB_RENDERER*);
void		Render_FB_RENDERER(FB_RENDERER*, uint8_t*);
void		Delete_FB_RENDERER(FB_RENDERER*);

struct NK_GEN {
	GUI_FONT*		fonts;
	uint32_t		number_of_fonts;
	uint32_t		width;
	uint32_t		height;
	int32_t			current_font;
	nk_context* 	NK_Context;
	uint8_t*		Graphics_Handle_Buffer;
	GRAPHICS*		Graphics_Handle;
	FB_RENDERER*	render_context;
};

float	Font_Get_Text_Width(nk_handle, float, const char*, int);
Color	rgba (char, char, char, char);
Color	fromNkColor(const struct nk_color&);

#endif

#endif
