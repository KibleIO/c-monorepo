#ifndef LAYOUT_SIZE_H_
#define LAYOUT_SIZE_H_

#include <cmath>
#include "Layout_Utilities.h"

enum SIZE_TYPE {
	// Interpret size ratio of the widget as a ratio of the panel's total size
	SIZE_TYPE_RATIO_OF_PANEL_TOTAL,
	// Interpret size ratio of the widget
	// as a ratio of the panel's remaining space
	SIZE_TYPE_RATIO_OF_PANEL_REMAINDER,
	// Interpret size ratio of the widget as a ratio of it's opposite side
	// E.g. a widget can have a width which is a ratio of its own height
	SIZE_TYPE_RATIO_OF_OPPOSITE,
	// Ratio is an exact measurement in pixels
	SIZE_TYPE_EXACT_SIZE
};

enum ORIENTATION {
	HORIZONTAL,
	VERTICAL
};

/*
LAYOUT SIZE
*/

// Simple struct encapsulates a size which is either
// a ratio of another fixed size, or a fixed size itself
struct LAYOUT_SIZE {
	float size;
	SIZE_TYPE type;
};

// CONSTRUCTORS
LAYOUT_SIZE Layout_Size(float size, SIZE_TYPE type);

LAYOUT_SIZE Ratio_Of_Total(float ratio);
LAYOUT_SIZE Ratio_Of_Remainder(float ratio);
LAYOUT_SIZE Ratio_Of_Opposite(float ratio);
LAYOUT_SIZE Exact_Size(float size);

// ACCESSORS

// Get the exact size of the layout ratio
// based on the current state of the nk_context
uint32_t Exact_Size(
const LAYOUT_SIZE*, const struct nk_context*, ORIENTATION,
uint32_t remaining_horizontal_space);
uint32_t Exact_Size_Horizontal(
const LAYOUT_SIZE*, const struct nk_context*,
uint32_t remaining_horizontal_space);
uint32_t Exact_Size_Vertical(const LAYOUT_SIZE*, const struct nk_context*);

// Converters return a layout ratio with the same exact size
// as the given layout but as a ratio of the total size
LAYOUT_SIZE Convert_To_Ratio_Of_Total(
const LAYOUT_SIZE*, const struct nk_context*, ORIENTATION,
uint32_t remaining_horizontal_space);
LAYOUT_SIZE Convert_To_Ratio_Of_Total_Vertical(
const LAYOUT_SIZE*, const struct nk_context*);
LAYOUT_SIZE Convert_To_Ratio_Of_Total_Horizontal(
const LAYOUT_SIZE*, const struct nk_context*,
uint32_t remaining_horizontal_space);

uint32_t Current_Usable_Panel_Space(const struct nk_context*, ORIENTATION);

#endif
