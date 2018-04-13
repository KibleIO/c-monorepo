#ifndef  GUI_H_
#define  GUI_H_

#include <fstream>
#include <iostream>
#include <linux/input.h>
#include <libinput.h>
#include <string>
#include <Graphics/Graphics.h>
#include "../UTILITIES/LOGGING.h"
#include "FRAME.h"
#include "MOUSE.h"
#include "KEYBOARD.h"

#include "stb_truetype.h"

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"

#define MAX_BUFFER 64

static float Font_Get_Text_Width(nk_handle handle, float height, const char* text, int len);

struct Baked_Glyph {
	unsigned char* bmp;     // Refractor as 'Bitmap'
	int            w;       // Refractor as 'Base_Width'
	int            h;       // Refractor as 'Base_Height'
	int            xoff;    // Refractor as 'X_Offset'
	int            yoff;    // Refractor as 'Y_Offset'
	int            advance; // Refractor as 'Scaled_Width'
};

struct GUI {
	nk_context*   NK_Context;
	FRAME*        Frames;
	unsigned int  Frame_Count;
	unsigned int  Frame_Resolution;
	unsigned int  Width;
	unsigned int  Height;

	// Black box crap
	char**        Delta_Buffers_Reserved;
	char**        Delta_Buffers;
	unsigned int* Delta_Lengths;
	char**        History_Buffers; // unneeded

	MOUSE**       Mouse;
	KEYBOARD**    Keyboard;
	int*          num_dev;

	//these are mouse coordinates;
	double        Current_X;
	double        Current_Y;
	int           Abs_X;
	int           Abs_Y; //these are mouse coordinates;

	int           baseline;         // Refractor as 'Font_Base_Line'
	Baked_Glyph   baked_glyphs[96]; // Refractor as 'Baked_Glyphs'
};

void Initialize_GUI        (GUI*, MOUSE**, KEYBOARD**, int*, unsigned int, unsigned int, unsigned int);
GUI* Construct_GUI         (MOUSE**, KEYBOARD**, int*, unsigned int, unsigned int, unsigned int);
void Uninitialize_GUI      (GUI*);
void Deconstruct_GUI       (GUI*);

// Isolate to a FRAME_MANAGER struct
void Initialize_Frames_GUI (GUI*, unsigned int);
void Delete_Frames_GUI     (GUI*);
void Refresh_Frames_GUI    (GUI*, unsigned int);
void Refresh_Frames_GUI    (GUI*);

void Apply_GUI             (GUI*, FRAME*, Graphics*);
void Apply_GUI             (GUI*, Graphics*);
void Apply_GUI             (GUI*, FRAME*);
void Apply_GUI             (GUI*, unsigned int);
void Apply_Mouse_GUI       (GUI*, FRAME*);
void Apply_Mouse_GUI       (GUI*, unsigned int);

int  Render_GUI            (GUI*, char**);

void Handle_Mouse_GUI      (GUI*);       // Default mouse handling. Can be uniquely called elsewhere
void Handle_Keyboard_GUI   (GUI*);    // Default keyboard handling. Can be uniquely called elsewhere

#endif /* GUI */
