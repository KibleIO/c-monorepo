#ifndef  GUI_H_
#define  GUI_H_

#include <string.h>

#include "BMP.h"
#include "FONT.h"
#include "MOUSE.h"
#include "KEYBOARD.h"
#include "../Utilities/CONCURRENT_QUEUE.h"
#include "../Graphics/Graphics.h"

#ifdef VGSUPPORTED
#include "../Graphics/VGGraphics.h"
#endif


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_DEFAULT_FONT

#include "nuklear.h"

#define GREY 0xff252525
#define WHITE 0xff989898
#define MID_GREY_DARK 0xffCCCCCB
#define MID_GREY_LIGHT 0xffBABABA

#define GUI_RGBA_BUFFER 0
#define GUI_X264_BUFFER 1
#define GUI_EMPTY_BUFFER 2

#define GUI_TOTAL_FONTS 3
#define GUI_FONT_DEFAULT_SIZE 2

#define MAX_BUFFER 64
using namespace std;

// All info the GUI struct needs to draw a font
struct GUI_FONT {
	FONT userFont;
	nk_user_font nkFont;
};

struct GUI {
	static int 		Width;
	static int 		Height;

	int				Display_ID;
	int				Frame_Resolution;

	nk_context* 	NK_Context;

	GUI_FONT* fonts;
	int* fontHeights;
	int currentFont;

	char*			Graphics_Handle_Buffer;
	GRAPHICS*		Graphics_Handle;

	bool			BakedBmp;
};

//static float Font_Get_Text_Width(nk_handle, float, const char*, int);
void Initialize_GUI_Themis(GUI* gui, int display_id);

void Initialize_GUI(GUI* gui, int width, int height, string font_path, char* frame_buffer = NULL);

void Start_Picture_GUI(GUI* gui);

void End_Picture_GUI();

void Pair_Fonts(nk_user_font*, FONT*, float height, const char* font_path);
void Set_Font(GUI*, int fontFlag);
void Delete_GUI(GUI*);
void Draw_Text(GUI*, GRAPHICS*, const struct nk_command_text*);
void Draw_Text(GUI*, GRAPHICS*, string, int, int, unsigned char*, unsigned char*);
void Render_Nuklear_GUI(GUI*);
#ifdef VGSUPPORTED
void Render_VG_Nuklear_GUI(GUI*);
#endif
void Render_GUI(GUI* gui, char* output_buffer);

//void Handle_Input_GUI(GUI* gui, MOUSE** mouse, KEYBOARD** keyboard, int len);
void Handle_Input_GUI(GUI* gui, Queue<MOUSE_EVENT_T*>* m_events, Queue<KEYBOARD_EVENT_T*>* k_events);

/*
Initialize helpers
*/
void Initialize_GUI_Font(GUI_FONT*, int height, const char* font_file_name);

void Set_GUI_Style_Default(GUI*);

/*
Runtime helpers
*/
float Font_Text_Width(
nk_handle handle, float height, const char* text, int length);

#endif
