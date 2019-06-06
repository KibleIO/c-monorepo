#ifndef LAYOUT_ENGINE_H_
#define LAYOUT_ENGINE_H_

#include <algorithm>
#include <cmath>
#include "../GUI.h"
#include "../../Utilities/vararg_ext.h"

enum LAYOUT_TYPE {
	// The size given is truncated to an int and used exactly as is
	LAYOUT_TYPE_STATIC,
	// The size given is taken as a ratio of the window size
	LAYOUT_TYPE_DYNAMIC,
	// If the value is (0, 1], it is taken as a ratio,
	// If the value is (1, inf), it is taken as an exact size
	LAYOUT_TYPE_MIXED
};

/*
LAYOUT SIZE
*/

// Simple struct encapsulates a buffer and a length for a widget size
// The layouts in this header are a pre-buffer design. Designated buffer space
// goes before the actual widget - so buffers for rows go above the row,
// and buffers for columns go to the left of the columns
struct LAYOUT_SIZE {
	float buffer;
	float breadth;
};

LAYOUT_SIZE Layout_Size(float buffer, float breadth);
LAYOUT_SIZE Layout_Size(float breadth);

// Given the number of buffers, breadths, and the size of buffer
// as a ratio of the size of the breadths, return a layout size
// with a buffer and breadth so that "num_items" rows/columns will fit snugly
// in the full width of the panel
// Note that the resulting values in the layout size are RATIOS, so you have
// to call "Layout_Row_xxx" with type = LAYOUT_TYPE_MIXED or
// type = LAYOUT_TYPE_DYNAMIC for it to layout properly
LAYOUT_SIZE Evenly_Spaced_Item(
uint8_t num_items, float buffer_to_breadth_ratio, bool side_buffers);

// Scale each item in the layout size by the given scalar
void Scale_Layout_Size(LAYOUT_SIZE*, float scalar);

/*
LAYOUT BUFFER FLAGS
*/

// Specification for the buffers of columns recently laid out
// If the flag of the requested column is true, this means it has
// a blank buffer column to the left of it and the client sould render
// an empty widget in its place. If not, the client should render no
// empty widget for buffer and immediately render the widget
struct LAYOUT_BUFFER_FLAGS {
	bool bufferFlag[NK_MAX_LAYOUT_ROW_TEMPLATE_COLUMNS];
	uint8_t totalFlags;
};

/*
ROW LAYOUT: Functions that actually layout the rows
*/

// Layout a row with a single column with no buffer
// and that fills up all horizontal space
LAYOUT_BUFFER_FLAGS Layout_Row_Single_Full(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size);

// Layout a row with only one column
LAYOUT_BUFFER_FLAGS Layout_Row_Single(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size, LAYOUT_SIZE column_size);

// Layout evenly spaced items, with or without buffers between
// the sides of the panel
LAYOUT_BUFFER_FLAGS Layout_Row_Evenly_Spaced_Items(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size, uint8_t items,
float buffer_to_breadth_ratio, bool side_buffers);

// Layout multiple columns with the same buffers and breadths
LAYOUT_BUFFER_FLAGS Layout_Row_Homogenous(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size, int columns,
LAYOUT_SIZE column_size);

// Layout a row with custom columns
LAYOUT_BUFFER_FLAGS Layout_Row_Custom(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size,
int total_columns, /*LAYOUT_SIZE*/...);

// Layout a row where each column is duplicated
// on the opposite side of the horizontal median
LAYOUT_BUFFER_FLAGS Layout_Row_Symmetric_Even_Pairs(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size, float mid_buffer,
int total_pairs, /*LAYOUT_SIZE*/...);

// Layout a row where each column is duplicated on the opposite side of the
// horizontal median except the last column, placed directly in the center
LAYOUT_BUFFER_FLAGS Layout_Row_Symmetric_Odd_Pairs(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size,
int total_pairs, /*LAYOUT_SIZE*/...);

// Layout a row with columns with the given sizes
LAYOUT_BUFFER_FLAGS Layout_Row(
struct nk_context*, LAYOUT_TYPE, LAYOUT_SIZE row_size,
int total_columns, const LAYOUT_SIZE column_sizes[]);

/*
RATIO CONVERSION: standalone functions to convert ratios to exact sizes
using the type of layout desired
*/

// Get the exact size, given the layout type and the space that will contain it
int Exact_Size(LAYOUT_TYPE, float size, int containing_space);
// Return true if the size given should be interpreted as an exact size
bool Use_As_Exact(LAYOUT_TYPE, float size);
// Return true if the size given should be interpreted as a ratio
// of the overall panel size
bool Use_As_Ratio(LAYOUT_TYPE, float size);

/*
PANEL SIZING: used to get the size of the current panel and properly
calculate ratios and exact sizes
*/

// Usable height/width of the current "panel" being laid out
// A panel could be a window or a subgroup in a window, such as a
// menu, tooltip, or popup
float Current_Usable_Panel_Height(const struct nk_context*);
float Current_Usable_Panel_Width(const struct nk_context*);

float Usable_Panel_Height(const struct nk_style*,
	enum nk_panel_type, int total_height);
float Usable_Panel_Width(const struct nk_style*,
	enum nk_panel_type, int total_width);

struct nk_vec2 Get_Panel_Padding(const struct nk_style*, enum nk_panel_type);

/*
DEPRECATED
*/

// Given an array of layout sizes, get the horizontal space
// in the current panel not taken up.  Returns as a ratio if the layout type
// is by ratio or an exact size if the layout type is mixed of fixed
float Remaining_Horizontal_Space(const struct nk_context*, LAYOUT_TYPE,
	int total_sizes, const LAYOUT_SIZE*);

// Get the exact space remaining in the current panel
int Exact_Remaining_Horizontal_Space(const struct nk_context*, LAYOUT_TYPE,
	int total_sizes, const LAYOUT_SIZE*);

// Get the exact sizes of all layout sizes given, assuming that they
// are ratios of the current panel's width
int Exact_Horizontal_Size(const struct nk_context*, LAYOUT_TYPE, int total_sizes,
	const LAYOUT_SIZE*);

int Nk_Group_Begin_With_Buffer(struct nk_context*, const char*, nk_flags);
void Nk_Label_With_Buffer(struct nk_context*, const char*, nk_flags);

#endif // ENDIF
