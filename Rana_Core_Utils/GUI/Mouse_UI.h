#ifndef MOUSE_UI_H_
#define MOUSE_UI_H_

#include "NK_BASE.h"

/*
MOUSE IMAGE
*/

struct MOUSE_IMAGE {
	struct nk_cursor cursor;
	bool loaded = false;
};

// Load this mouse image's image, and give nuklear a reference to it
void Load_MOUSE_IMAGE(
MOUSE_IMAGE*, struct nk_context*, enum nk_style_cursor, string image_path,
uint32_t w, uint32_t h);
void Delete_MOUSE_IMAGE(MOUSE_IMAGE*);

/*
MOUSE UI
*/

// Manage the memory for the cursor images
// Nuklear handles the rendering of the images by holding references to
// the cursors, but this struct is in charge of
// allocation and deletion of the images on the heap
struct MOUSE_UI {
	MOUSE_IMAGE images[NK_CURSOR_COUNT];
};

void Load_Cursor_MOUSE_UI(
MOUSE_UI*, struct nk_context*, enum nk_style_cursor, string image_path,
uint32_t w, uint32_t h);
void Delete_MOUSE_UI(MOUSE_UI*);

#endif
