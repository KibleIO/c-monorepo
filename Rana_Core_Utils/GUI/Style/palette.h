/*
pallete.h
---------
The pallete header file contains definitions for nk colors so that its easy for
colors to stay consistent across similarly themed screens
---------
*/

#ifndef PALETTE_H_
#define PALETTE_H_

#include "../GUI.h"
#include "Palette_Color.h"

#define TRANSPARENT nk_rgba(0x00, 0x00, 0x00, 0x00)
// GRAYS
#define DARK_GRAY nk_rgba(0x70,0x86,0x9b,0xff)
#define MED_GRAY nk_rgba(0xcc, 0xcc, 0xcc, 0xff)
#define LIGHT_GRAY nk_rgba(0xdc, 0xdb, 0xdb, 0xff)
// BLUES
#define DARK_BLUE nk_rgba(0x33, 0x42, 0x59, 0xff)
#define MED_BLUE nk_rgba(0x63, 0x88, 0xad, 0xff)
#define MED_BLUE_2 nk_rgba(0x82, 0xa4, 0xc5, 0xff)
#define LIGHT_BLUE nk_rgba(0xbd, 0xd1, 0xdb, 0xff)
// MISC
#define GRAY_BLUE nk_rgba(0x66, 0x7a, 0x99, 0xff)
#define LIGHT_GRAY_BLUE nk_rgba(0xb4, 0xc4, 0xd0, 0xff)
#define WHITE_BLUE nk_rgba(0xc9, 0xd5, 0xdc, 0xff)
#define DARK_TURQUOIS nk_rgba(0x4e, 0x61, 0x6b, 0xff)
#define SUBMISSIVE_BUTTON_COLOR nk_rgba(0xa7, 0xb6, 0xc1, 0xff)
#define BLACK nk_rgba(0x00, 0x00, 0x00, 0xff)

// Alpha channel defined for "nearly transparent" colors
#define NEAR_TRANSPARENT 0x60

// Transparent versions of colors are nealy invisible
// GRAYS
#define TRANSPARENT_LIGHT_GRAY nk_rgba(LIGHT_GRAY.r, LIGHT_GRAY.g, LIGHT_GRAY.b, NEAR_TRANSPARENT)
// BLUES
#define TRANSPARENT_LIGHT_BLUE nk_rgba(LIGHT_BLUE.r, LIGHT_BLUE.g, LIGHT_BLUE.b, NEAR_TRANSPARENT)
// MISC
#define TRANSPARENT_BLACK nk_rgba(BLACK.r, BLACK.g, BLACK.b, NEAR_TRANSPARENT)

#define DIFF nk_rgb(0x0f, 0x0f, 0x0f)
// Gives a color slightly lighter/darker than the color passed in
// Great for creating different button colors when hovered/clicked
nk_color lighter(nk_color original);
nk_color darker(nk_color original);

bool bright(const struct nk_color&);

// Return a color with the r-b colors flipped
struct nk_color rb_flip(const struct nk_color&);

/*
EXPERIMENTAL
*/

#define MAX_COLORS 16

struct PALETTE {
	static PALETTE_COLOR colors[MAX_COLORS];
};

void Set_Color(struct nk_color, uint8_t index);
struct nk_color Get_Color(uint8_t index);

#endif /*PALLETE_H_*/
