#ifndef LAYOUT_RATIO_H_
#define LAYOUT_RATIO_H_

#include <cmath>
#include "Layout_Utilities.h"

enum RATIO_TYPE {
	// Interpret size ratio of the widget as a ratio of the panel's total size
	RATIO_OF_PANEL_TOTAL,
	// Interpret size ratio of the widget
	// as a ratio of the panel's remaining space
	RATIO_OF_PANEL_REMAINDER,
	// Interpret size ratio of the widget as a ratio of it's opposite side
	// E.g. a widget can have a width which is a ratio of its own height
	RATIO_OF_OPPOSITE
};

enum ORIENTATION {
	HORIZONTAL,
	VERTICAL
};

/*
LAYOUT SIZE
*/

// Simple struct encapsulates a size which is a ratio of another fixed size.
// Defined as a "buffer" or "breadth" to be handled differently
// by the layouting system
struct LAYOUT_RATIO {
	float ratio;
	RATIO_TYPE type;
};

// CONSTRUCTORS
LAYOUT_RATIO Layout_Ratio(float ratio, RATIO_TYPE type);

LAYOUT_RATIO Ratio_Of_Total(float ratio);
LAYOUT_RATIO Ratio_Of_Remainder(float ratio);
LAYOUT_RATIO Ratio_Of_Opposite(float ratio);

// ACCESSORS

// Get the exact size of the layout ratio
// based on the current state of the nk_context
uint32_t Exact_Size(
const LAYOUT_RATIO*, const struct nk_context*, ORIENTATION,
uint32_t remaining_horizontal_space);
uint32_t Exact_Size_Horizontal(
const LAYOUT_RATIO*, const struct nk_context*,
uint32_t remaining_horizontal_space);
uint32_t Exact_Size_Vertical(const LAYOUT_RATIO*, const struct nk_context*);

// Converters return a layout ratio with the same exact size
// as the given layout but as a ratio of the total size
LAYOUT_RATIO Convert_To_Ratio_Of_Total(
const LAYOUT_RATIO*, const struct nk_context*, ORIENTATION,
uint32_t remaining_horizontal_space);
LAYOUT_RATIO Convert_To_Ratio_Of_Total_Vertical(
const LAYOUT_RATIO*, const struct nk_context*);
LAYOUT_RATIO Convert_To_Ratio_Of_Total_Horizontal(
const LAYOUT_RATIO*, const struct nk_context*,
uint32_t remaining_horizontal_space);

uint32_t Current_Usable_Panel_Space(const struct nk_context*, ORIENTATION);

#endif
