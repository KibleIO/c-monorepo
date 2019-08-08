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

#include <string>

using namespace std;

struct NK_GEN;

void			Initialize_NK_GEN(NK_GEN*);
void			Load_Fonts_NK_GEN(NK_GEN*, string, uint32_t*, uint32_t);
void			Set_Font_NK_GEN(NK_GEN*, uint32_t);
uint32_t		Get_Width_NK_GEN(NK_GEN*);
uint32_t		Get_Height_NK_GEN(NK_GEN*);
struct nk_image	Load_Image_NK_GEN(string, uint32_t, uint32_t);
void 			Render_NK_GEN(NK_GEN*);
void 			Delete_NK_GEN(NK_GEN*);

#endif
