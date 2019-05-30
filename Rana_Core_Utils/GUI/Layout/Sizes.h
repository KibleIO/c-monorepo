/*
layout.h
--------
Encapsulates information, structs and methods about the layout of certain buttons
and windows that stay consistent across screens
--------
*/

#ifndef SIZES_H_
#define SIZES_H_

#include "../GUI.h"

// Maximum value set for the width of the main width
#define MAIN_WIDTH_MAX 1000
// Width of the main screen without clamping
#define MAIN_WIDTH_UNCLAMPED (int)(GUI::Width * 0.52)

#define MAIN_WIDTH (((MAIN_WIDTH_UNCLAMPED) > (MAIN_WIDTH_MAX)) ? (MAIN_WIDTH_MAX) : (MAIN_WIDTH_UNCLAMPED))
#define MAIN_HEIGHT (int)(MAIN_WIDTH * 0.65)
#define MAIN_X ((GUI::Width - MAIN_WIDTH) / 2)
#define MAIN_Y ((GUI::Height - MAIN_HEIGHT) / 2)
#define MAIN_RECT nk_rect(MAIN_X, MAIN_Y, MAIN_WIDTH, MAIN_HEIGHT)

// Height of the remaining space after accounting for the height of the header and the height of the nav buttons
#define MAIN_GROUP_HEIGHT (MAIN_HEIGHT - STANDARD_HEADER_HEIGHT - STANDARD_FOOTER_HEIGHT)
#define MAIN_GROUP_RECT nk_rect(MAIN_X, MAIN_Y + STANDARD_HEADER_HEIGHT, MAIN_WIDTH, MAIN_GROUP_HEIGHT)

// Standard layout sizes
#define STANDARD_BOTTOM_BUFFER (int)(MAIN_HEIGHT * 0.05)
#define STANDARD_HEADER_HEIGHT (int)(MAIN_HEIGHT * 0.05)
#define STANDARD_FOOTER_HEIGHT (STANDARD_BUTTON_H + STANDARD_BOTTOM_BUFFER)

// Standard button sizes
#define STANDARD_BUTTON_H (int)(MAIN_HEIGHT * 0.1)
#define STANDARD_BUTTON_W (int)(MAIN_WIDTH * 0.25)
#define STANDARD_BUTTON_ROUNDING (int)(STANDARD_BUTTON_H * 0.2)

#endif
