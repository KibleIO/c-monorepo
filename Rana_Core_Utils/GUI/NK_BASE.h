#ifndef NK_BASE_H_
#define NK_BASE_H_

#include <stdarg.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT

#ifdef __arm__
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#endif

#include "nuklear.h"
#include "../Graphics/SCREEN_DIM.h"

#include <string>

using namespace std;

struct NK_GEN;

void			Initialize_NK_GEN(NK_GEN*);
void			Load_Fonts_NK_GEN(NK_GEN*, string, uint32_t*, uint32_t);
void			Set_Font_NK_GEN(NK_GEN*, uint32_t);
SCREEN_DIM		Get_Screen_Dimensions_NK_GEN(NK_GEN*);

// Load a new image and get an nk_image object
struct nk_image	Load_Image_NK_GEN(string, uint32_t, uint32_t);
// Free dynamically allocated space for the nk_image given
void 			Free_Image_NK_GEN(struct nk_image*);
// Change the size of the nk_image object. Return true if the resize took place
// or false if it did not
void 			Resize_Image_NK_GEN(struct nk_image*, uint32_t, uint32_t);

void 			Render_NK_GEN(NK_GEN*);
void 			Delete_NK_GEN(NK_GEN*);

#endif
