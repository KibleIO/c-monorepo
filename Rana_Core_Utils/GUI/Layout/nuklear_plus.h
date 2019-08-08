#ifndef NUKLEAR_PLUS_H_
#define NUKLEAR_PLUS_H_

#include "../NK_BASE.h"
#include "../../Utilities/TIMER.h"
#include "../../Utilities/LOGGING.h"
#include <cstdarg>
#include <cstring>

enum sizing_policy {
  SIZING_FIXED,  // The size must be exactly equal to the hint
  SIZING_MIN,  // The size range is [min, infinity)
  SIZING_MAX,  // The size range is [0, max]
  SIZING_CONSTRAINED,  // The size range is [min, max]
  SIZING_FREE,  // The size range is [0, infinity)
  SIZING_INHERITED  // The size is inherited from the next structure above in
  					// the layout heirarchy. Column width -> Row height and
					// Row height -> Window width
};

// Gives a size hint and a sizing policy to determine how the size will be
// readjusted by resizing algorithms
struct size_hint {
  int hint; // Size hint is interpreted based on the local policy
  // Min/max of the size may be used or discarded depending on the policy
  int min, max;
  enum sizing_policy policy; // Policy used to interpret the hint

  // Actual size of the object
  // WARNING: This is calculated in resizing algorithms, so directly modifying
  // may result in bad layouting
  int size;
  // Ratio of the parent size that this size will inherit
  // Only applies to hints with a SIZING_INHERITED policy
  float inheritanceRatio;
};

// A single column in a row, with a left buffer, width, and style applied to it
struct column_layout {
  struct size_hint width;
  struct size_hint leftBuffer;
  struct nk_style style;
};

// A single row in a window, with an upper buffer, height, and set of items inside
struct row_layout {
  struct size_hint height;
  struct size_hint upperBuffer;

  // TEMPLATED: can be calculated using convenient templates or manually set
  struct size_hint rightBuffer;
  struct column_layout* columns;
  int totalColumns;

  // PRIVATE: used to execute layouting
  int columnCapacity; // Space allocated for the array of columns
  int currentColumn; // Index of the current row being laid out
  // True if the row has finished layouting all of its columns
  bool layoutFinished;
};

struct window_layout {
  struct nk_rect geometry;
  struct nk_style_window style; // Style of the window
  char* title;  // Null-terminated string for the title of the window

  // TEMPLATED: can be set using convenient templates or set manually
  struct size_hint bottomBuffer;
  // If true, this layout describes a group;
  // otherwise, it describes its own standalone window
  bool isSubwindow;
  struct row_layout* rows;
  int totalRows;

  // PRIVATE: used to execute layouting
  int rowCapacity;  // Space allocated for the array of rows
  int currentRow; // Index of the current row being laid out
  // True if the window has finished laying out all of its rows
  bool layoutFinished;
};

/*
BASIC INITIALIZERS
------------------
Full initialization functions require all data to be manually set
by the client code to call
------------------
*/

void Initialize_Window_Layout(struct window_layout*, struct nk_rect,
	struct size_hint bottomBuff, struct nk_style_window, const char* title,
	bool subwindow, struct row_layout*, int totalRows);
void Initialize_Row_Layout(struct row_layout*, struct size_hint height,
	struct size_hint upperBuff, struct size_hint rightBuff,
	struct column_layout*, int totalColumns);
void Initialize_Column_Layout(struct column_layout*, struct size_hint width,
	struct size_hint leftBuff, struct nk_style);

// BASIC DELETERS
void Delete_Window_Layout(struct window_layout*);
void Delete_Row_Layout(struct row_layout*);

/*
TEMPLATED INITIALIZERS
----------------------
Initializers are slightly more specific and do not require the client code
to handle as much data manually
----------------------
*/

void Initialize_Row_Layout_Homogenous(struct row_layout*,
	struct size_hint height, struct size_hint upperBuff,
  	struct size_hint columnSize, struct size_hint columnLeftBuffer,
	struct nk_style columnStyle, int totalItems);
// Initialize a row layout and apply the symmetric even pairs template to it
void Initialize_Row_Layout_Symmetric_Even_Pairs(struct row_layout*,
	struct size_hint height, struct size_hint upperBuff,
  	struct column_layout* pairs, int totalPairs, struct size_hint midBuff);
// Initialize a row layout and apply the symmetric odd pairs to it
void Initialize_Row_Layout_Symmetric_Odd_Pairs(struct row_layout*,
	struct size_hint height, struct size_hint upperBuff,
  	struct column_layout* pairs, int totalColumns);

/*
INITIALIZATION HELPERS
----------------------
To make the daunting task of setting up the MANY particulars of a window layout more
intuitive, a system was devised that works the same way as actually executing the
window. In this system, the process is guided by beginning the initialization and
applying templates to each row one-by-one:

Initialize_Window_Layout_Begin(...)
Apply_Window_Template_xxx(...)

Apply_Row_Template_xxx(...)
Apply_Row_Template_xxx(...)
...

Initialize_Window_Layout_End(...)
----------------------
*/

// Begin initializing a standalone window
void Initialize_Window_Layout_Begin(struct window_layout*, struct nk_rect,
	struct nk_style_window, const char* title);
// Begin initializing a subwindow. The window calculates its own rect using the
// row and column of the parent window it will be nested inside of
// NOTE: size of the row-column of nesting should not be adjusted after
// initializing a subwindow in this way, or the rect of the window will not
// reflect its actual size
void Initialize_Subwindow_Layout_Begin(struct window_layout*,
	const struct window_layout* parent, int rowIndex, int columnIndex,
	struct nk_style_window, const char* title);
// Finalize the window layout initialization
void Initialize_Window_Layout_End(struct window_layout*);

// Apply template to the window so all rows have the same height and upper buffer
// NOTE: rows initialized in this way are vertically centered in the window
void Apply_Window_Template_Homogenous(struct window_layout*,
	struct size_hint height, struct size_hint upperBuff, int totalRows = 1);
// Apply custom template to the window where all info about the rows
// must be set manually by client code
void Apply_Window_Template_Custom(struct window_layout*,
	struct size_hint bottomBuff, struct row_layout*, int totalRows);

/*
WINDOW TEMPLATE VARIADIC
*/

// Function converts the variadic arguments int an array, so the client
// need not construct their own array
void Apply_Window_Template_Custom(struct window_layout*,
	struct size_hint bottomBuff, int totalRows, /*row_layout*/...);

/*
CURRENT ROW TEMPLATES: Call these if you are using the guided initialization process
to make the window apply the template to the current row being initialized
NOTE: the window automatically moves to the next row each time an "apply template"
function is called.
Each "Apply_Row" called in sequence applies to the next row in the window
*/

// Apply_Row_Template_Homogenous to the current row being initialized
void Apply_Row_Template_Homogenous(struct window_layout*,
	struct size_hint columnSize, struct size_hint columnLeftBuffer,
	struct nk_style columnStyle, int totalItems = 1);
// Apply_Row_Template_Homogenous_Custom_Sides to the current row being initialized
void Apply_Row_Template_Homogenous_Custom_Sides(struct window_layout*,
	struct size_hint sideBuff, struct size_hint columnSize,
	struct size_hint columnLeftBuffer, struct nk_style columnStyle,
	int totalItems);
// Apply_Row_Template_Symmetric_Even_Pairs to the current row
// in the layout being initialized
void Apply_Row_Template_Symmetric_Even_Pairs(struct window_layout*,
	struct column_layout* pairs, int totalPairs, struct size_hint midBuff);
// Apply_Row_Template_Symmetric_Odd_Pairs to the current row
//in the layout being initialized
void Apply_Row_Template_Symmetric_Odd_Pairs(struct window_layout*,
	struct column_layout* pairs, int totalColumns);
// Apply_Row_Template_Custom to the current row in the layout being initialized
void Apply_Row_Template_Custom(struct window_layout*,
	struct size_hint rightBuff, struct column_layout*, int totalColumns);
// Copy the previous row to the current row
void Apply_Row_Template_Ditto_Previous(struct window_layout*);
// Copy the row at the index specified to the current row
void Apply_Row_Template_Ditto(struct window_layout*, int rowIndex);

/*
CURRENT ROW TEMPLATE VARIADIC
*/
void Apply_Row_Template_Symmetric_Even_Pairs(struct window_layout*,
	struct size_hint midBuff, int totalPairs, /*column_layout*/ ...);
void Apply_Row_Template_Symmetric_Odd_Pairs(struct window_layout*,
	int totalPairs, /*column_layout*/ ...);
void Apply_Row_Template_Custom(struct window_layout*,
	struct size_hint rightBuff, int totalColumns, /*column_layout*/ ...);

/*
ROW TEMPLATE: Use these if you want to apply a template to a particular row,
apart from the guided initialization process
*/

// Initialize columns in a row layout where all items have the same sizes and styles
// NOTE: all items initialized and fitted in this way will automatically be centered
// inside of the containing space, so use this function if you want an item to be centered
void Apply_Row_Template_Homogenous(struct row_layout*,
	struct size_hint columnSize, struct size_hint columnLeftBuffer,
  	struct nk_style columnStyle, int totalItems = 1);
// Same as homogenous template but the buffers on the left and right sides
// of the window are different from the buffers between each column
void Apply_Row_Template_Homogenous_Custom_Sides(struct row_layout*,
	struct size_hint sideBuff, struct size_hint columnSize,
  	struct size_hint columnLeftBuffer, struct nk_style columnStyle,
	int totalItems);
// Initialize a row layout's columns where each column in the array is
// duplicated on the opposite side of the containing width
void Apply_Row_Template_Symmetric_Even_Pairs(struct row_layout*,
	struct column_layout* pairs, int totalPairs, struct size_hint midBuff);
// Initialize the columns in a row layout where each column in the array is
// duplicated on the opposite side of the containing width
// EXCEPT the very last column, which is placed directly in the center
// NOTE: the function expects the total COLUMNS, not pairs.
// You must give the actual size of the array
void Apply_Row_Template_Symmetric_Odd_Pairs(struct row_layout*,
	struct column_layout* pairs, int totalColumns);
// Apply template to the row where all data must be manually set by client code
void Apply_Row_Template_Custom(struct row_layout*, struct size_hint rightBuff,
	struct column_layout*, int totalColumns);

/*
ROW TEMPLATE VARIADIC
*/
void Apply_Row_Template_Symmetric_Even_Pairs(struct row_layout*,
	struct size_hint midBuff, int totalPairs, struct column_layout cols...);
void Apply_Row_Template_Symmetric_Odd_Pairs(struct row_layout*,
	int totalPairs, struct column_layout cols...);
void Apply_Row_Template_Custom(struct row_layout*, struct size_hint rightBuff,
	int totalColumns, struct column_layout cols...);


/*
SIZE FITTING ALGORITHM
----------------------
Size fitting algorithm takes an array of size pointers and attempts to scale them
proportionately to fill up the containingSpace while also obeying the size policies
established in the size hints

NOTE: the sizes given are NOT GUARANTEED to fit the containingSpace after running
the algorithm.  The algorithm cannot adjust fixed sizes and cannot push constrained
sizes outside of their limits, which may result in the sizes not fitting
---------------------
*/
void Finalize_Window_Sizes(struct window_layout*);
// Fit the row heights into the window's full height
void Finalize_Vertical_Window_Sizes(struct window_layout*);
// Fit the column widths into the window's width
void Finalize_Horizontal_Window_Sizes(struct window_layout*);
// Fit the column widths of the given row into the given width of the window
void Finalize_Row_Sizes(struct row_layout*, int windowWidth);
void Fit(struct size_hint**, int totalRefs, int containingSpace);
void Adjust_Space(struct size_hint**, int totalRefs, int adjustableSpace,
	int availableSpace);
// Accumulate the size hints of all fixed sizes
int Get_Fixed_Hints(struct size_hint**, int totalRefs);
// Accumulate the size hints of all sizes
int Get_Total_Hints(struct size_hint**, int totalRefs);

void Clamp_Size_Hint(struct size_hint*);
// Check to make sure any children in the array of references
// inherit the parent size if the policy requires it
void Enforce_Inheritance(struct size_hint** children, int totalChildren,
	int parentSize);
// Let one size hint inherit it's size from the other size, if the policy allows
void Inherit_Size(struct size_hint* child, int parentSize);

// LAYOUTING EXECUTION
// -------------------
// Use these to actually execute the layout of your window in your rendering methods
// Execution uses the following pattern:
//
// if(Window_Layout_Begin(...)) {
//    Window_Layout_Next(...);
//    widget(...);
//    Window_Layout_Next(...);
//    widget(...);
//    ...
//    Window_Layout_End(...);
// }
// --------------------

// Start the layouting from the beginning and start an nk (sub)window
int Window_Layout_Begin(struct window_layout*, struct nk_context*, nk_flags);
// Set the row to start from the beginning for layouting
void Row_Layout_Begin(struct row_layout*);

// Layout the next part of the window
void Window_Layout_Next(struct window_layout*, struct nk_context*);
// Layout the next column in the row. This is called by Window_Layout_Next
// and should NOT be called by client code
void Row_Layout_Next(struct row_layout*, struct nk_context*);
// Simple function pushes out the column buffer and the column width with the
// specified style set. This is called by Row_Layout_Next
// and should NOT be called by client code
void Layout_Column(struct column_layout, struct nk_context*);

// This MUST be called by client code when layouting is finished
void Window_Layout_End(struct window_layout*, struct nk_context*);
// This is called by the window - client code need not call
void Row_Layout_End(struct row_layout*, struct nk_context*);

/*
CURRENT ACCESSORS
-----------------
Access current row / column of the structure specified. Works with:
  Window_Layout_xxx
  Initialize_Window_Layout_xxx
  Window_Iterate_xxx

  Row_Layout_xxx
  Row_Iterate_xxx
-----------------
*/

struct row_layout& Current_Row(const struct window_layout*);
struct column_layout& Current_Column(const struct window_layout*);
struct column_layout& Current_Column(const struct row_layout*);
struct nk_rect Current_Rect(const struct window_layout*);

/*
LAYOUT MODIFICATION
-------------------
Allows runtime modification of layouts, treating them like containers
where layout structures can be added/removed
-------------------
*/

// Insert the desired row at the index specified
void Insert_Row(struct window_layout*, struct row_layout, int index);
// Remove the row at the desired index
void Remove_Row(struct window_layout*, int index);
// Allocate more storage for the array of rows
void Reserve_Rows(struct window_layout*, int additionalSpace);

// LAYOUTING HELPERS
// -----------------
// Help with layouting initialization/execution
// -----------------

// You may find these accessors more convenient than manually accessing
// the fields in a window_layout struct
struct row_layout& Row(const struct window_layout*, int row);
struct column_layout& Column(const struct window_layout*, int row, int column);
struct column_layout& Column(const struct row_layout*, int column);

// Return an unconstrained size hint with no min-max
struct size_hint Free_Size(int hint);
// Return a size hint that must be the exact size given
struct size_hint Fixed_Size(int size);
// Return a size hint with a minimum but no maximum
struct size_hint Lower_Bounded_Size(int hint, int min);
// Return a size hint with a maximum but no mimum
struct size_hint Upper_Bounded_Size(int hint, int max);
// Return a size hint with a minimum and maximum
struct size_hint Constrained_Size(int hint, int min, int max);
// Return a size hint that inherits its size from the next structure
// above in the heirarchy
struct size_hint Inherited_Size(float ratio);
struct column_layout Column_Layout(struct size_hint width,
	struct size_hint left_buffer, struct nk_style);
// Return a row with only the upper buffer and height set,
// but no additional significant data
struct row_layout Row_Layout(struct size_hint height,
	struct size_hint upperBuff);
// Return a window with only rect, bottom buffer and style set,
// but no additional significant data
struct window_layout Window_Layout(struct nk_rect, struct nk_style_window,
	const char* title);
// Return a subwindow where the rect is calculated given the parent window
// and the index of the row and column where the window will be nested
// WARNING: remember to FINALIZE the sizes of the parent window before using this!
struct window_layout Subwindow_Layout(const struct window_layout* parent,
	int rowIndex, int columnIndex, struct nk_style_window, const char* title);

// Get the rect of an item inside a window, given the window the item is in,
// the row and the column
struct nk_rect Item_Rect(const struct window_layout*, int row, int column);
nk_flags nk_edit_string_concealed(struct nk_context*, nk_flags, char* buffer,
	int* len, int max, char concealChar, nk_plugin_filter);
void nk_layout_row_push_buffer(struct nk_context*, float buffer);

/*
TESTING
-------
Functions useful for testing size hints
-------
*/

/*
EXPERIMENTAL
*/
namespace nk_experimental {
	/*
	GENERIC ITERATION (EXPERIMENTAL)
	--------------------------------
	Generic iterator functions allow client code to iterate through a window's
	rows / columns without laying them out, initializing them, etc.
	NOTE: the iterators CANNOT be used until the window is fully initialized
	--------------------------------
	*/

	// Begin an iteration through a window at the specified row and column indeces
	void Window_Iterate_Begin(struct window_layout*, int startingRowIndex = 0,
		int startingColumnIndex = 0);
	// Begin an iteration through a row at the specified column index
	void Row_Iterate_Begin(struct row_layout*, int startingColumnIndex = 0);

	// Move window to the next column. If it is at the end of a row, it automatically
	// updates to the next row, first column
	void Window_Iterate_Next_Column(struct window_layout*);
	// Move the window to the next row
	void Window_Iterate_Next_Row(struct window_layout*);
	// Move the row to the next column
	void Row_Iterate_Next(struct row_layout*);

	// End iteration. Calling these is not strictly necessary since calling a "Begin"
	// will just overwrite the current iteration
	void Window_Iterate_End(struct window_layout*);
	void Row_Iterate_End(struct row_layout*);
}

#endif //NUKLEAR_PLUS_H_
