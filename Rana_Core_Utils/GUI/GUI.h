#ifndef GUI_H_
#define GUI_H_

#include <string.h>
#include "../Graphics/Graphics.h"
#include "../Utilities/MOUSE.h"
#include "../Utilities/KEYBOARD.h"
#include "../Utilities/CONCURRENT_QUEUE.h"
#include "NK_BASE.h"
#include "NK_FB.h"
#include "NK_GLES2.h"

#define GUI_TOTAL_FONTS 3
#define GUI_FONT_DEFAULT_SIZE 2

using namespace std;

struct GUI {
	static int32_t 		Width;
	static int32_t 		Height;
	NK_GEN* nk_backend;
};

void			Initialize_GUI(GUI*, string);
void			Render_Nuklear_GUI(GUI*);
void			Handle_Input_GUI(
GUI*, Queue<MOUSE_EVENT_T*>*, Queue<KEYBOARD_EVENT_T*>*);

void			Set_Style_Default_GUI(GUI*);
void			Set_Font(GUI*, int fontFlag);
struct nk_image	Load_Image_GUI(string, uint32_t, uint32_t);
void			Delete_GUI(GUI*);

#endif
