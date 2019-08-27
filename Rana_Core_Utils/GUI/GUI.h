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
#include "Texture_Manager.h"

#define GUI_TOTAL_FONT_HEIGHTS 5
#define GUI_FONT_DEFAULT_SIZE 2
// Size of the smallest font height
#define GUI_FONT_HEIGHT_MIN 20
// Height difference in each gui font height
#define GUI_FONT_HEIGHT_DIFFERENCE 5

using namespace std;

struct GUI {
	static SCREEN_DIM screen_dim;
	static TEXTURE_MANAGER texture_manager;
	NK_GEN* nk_backend;
};

void				Initialize_GUI(GUI*, string);
void				Render_Nuklear_GUI(GUI*);
void				Handle_Input_GUI(
GUI*, Queue<MOUSE_EVENT_T*>*, Queue<KEYBOARD_EVENT_T*>*);

void				Set_Style_Default_GUI(GUI*);
void				Set_Font(GUI*, int fontFlag);
void 				Set_Font_Default(GUI*);
// Add a texture to the GUI's list of textures
// and return a pointer to the texture allocated
struct TEXTURE*		Load_Image_GUI(string, uint32_t, uint32_t);
void				Delete_GUI(GUI*);

#endif
