#ifndef  GUI_H_
#define  GUI_H_

#include <cstring>

#include <Graphics/Graphics.h>
#include <GUI/BMP.h>
#include <GUI/FONT.h>
#include <Utilities/Assets.h>
#include <Utilities/Integer.h>
#include <GUI/MOUSE.h>
#include <GUI/KEYBOARD.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT

#include <GUI/nuklear.h>

#define GREY 0xff252525
#define WHITE 0xff989898
#define MID_GREY_DARK 0xffCCCCCB
#define MID_GREY_LIGHT 0xffBABABA

#define GUI_RGBA_BUFFER 0
#define GUI_X264_BUFFER 1
#define GUI_EMPTY_BUFFER 2

#define MAX_BUFFER 64
using namespace std;

struct GUI {
	int				Display_ID;
	int				Width;
	int				Height;
	int				Frame_Resolution;

	nk_context* 	NK_Context;
	FONT* 			Font;
	nk_user_font*	FontNK;
	char*			Graphics_Handle_Buffer;
	GRAPHICS*		Graphics_Handle;

	bool			BakedBmp;
};

static float Font_Get_Text_Width(nk_handle, float, const char*, int);
void Initialize_GUI_Themis(GUI* gui, int display_id);
void Initialize_GUI(GUI* gui, int width, int height, string font_path, char* frame_buffer = NULL);
void Delete_GUI(GUI*);
void Draw_Text(GUI*, GRAPHICS*, const struct nk_command*);
void Draw_Text(GUI*, GRAPHICS*, string, int, int, unsigned char*, unsigned char*);
void Render_Nuklear_GUI(GUI*);
void Render_Mouse_GUI(GUI*, double, double);
void Render_Mouse_GUI(GUI* gui, GRAPHICS* Graphics_Handle, double c_x, double c_y);
void Render_X264(GUI* gui, char* X264_Buffer, int size);
void Render_GUI(GUI* gui, char* output_buffer);

//void Handle_Input_GUI(GUI* gui, MOUSE** mouse, KEYBOARD** keyboard, int len);
void Handle_Input_GUI(GUI* gui, Queue<MOUSE_EVENT*>* m_events, Queue<EVENT_ELEMENT*>* k_events);
#endif
