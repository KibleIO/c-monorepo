#ifndef CALENDAR_UI_H_
#define CALENDAR_UI_H_

#include "../GUI.h"
#include "Panel.h"
#include "Button_Package.h"
#include "../Style.h"
#include "../Layout/Layout_Engine.h"
#include "../../Utilities/tm_ext.h"

#define CALENDAR_TOTAL_ROWS 5
#define CALENDAR_ROW_VERTICAL_BUFFER (Ratio_Of_Total(0.01))
#define CALENDAR_MONTH_SELECT_HEIGHT (Ratio_Of_Total(0.25))
#define CALENDAR_ROW_HEIGHT (Ratio_Of_Total(\
	(1.0 - (CALENDAR_MONTH_SELECT_HEIGHT.size +\
	CALENDAR_ROW_VERTICAL_BUFFER.size * (CALENDAR_TOTAL_ROWS + 1))) /\
	CALENDAR_TOTAL_ROWS))

#define CALENDAR_BUFFER_TO_BUTTON_RATIO 0.1
#define CALENDAR_BUFFER_WIDTH_RATIO (1.0 / ((DAYS_IN_WEEK + 1) + \
((float)DAYS_IN_WEEK / CALENDAR_BUFFER_TO_BUTTON_RATIO)))
#define CALENDAR_BUTTON_WIDTH_RATIO ((1 - \
((DAYS_IN_WEEK + 1) * CALENDAR_BUFFER_WIDTH_RATIO)) / (float)DAYS_IN_WEEK)

const string MONTH_NAMES[TOTAL_MONTHS] = {
		"Jan", "Feb", "Mar", "Apr", "May", "June",
		"July", "Aug", "Sept", "Oct", "Nov", "Dec"
};

struct CALENDAR_UI {
	tm calendar;	// Underlying data of the calendar ui
	uint8_t currentMonth;	// Current month being displayed by the calendar

	PANEL dateSelectionGroup;
	BUTTON_PACKAGE dayOfMonthButtons[TOTAL_MONTHS];
	BUTTON monthSelectButtons;	// Push buttons to scroll through the months
	BUTTON monthLabel;	// Display of the current month as a false button
};

// Initialize calendar ui using current time
void Initialize_CALENDAR_UI(
CALENDAR_UI*, struct nk_color background, struct nk_color foreground,
struct nk_color selected, struct nk_color text);
// Render calendar ui in its own group
void Render_CALENDAR_UI(CALENDAR_UI*, struct nk_context*);
void Render_CALENDAR_UI_With_Buffer(CALENDAR_UI*, struct nk_context*);
void Delete_CALENDAR_UI(CALENDAR_UI*);

// GETTERS

// Get calendar date in the form mm/dd/yyyy
string Date_String_Three_Nums(const CALENDAR_UI*);
// Get a string of the form hh:mm
string Time_String(const CALENDAR_UI*);
// Get a string of the form hh:mm in military time
string Military_Time_String(const CALENDAR_UI*);

// HELPERS

// Resolve a click on the day button of the given month
// Day and month passed in are both 0-based
void Resolve_Date_Selection(CALENDAR_UI*, uint8_t day, uint8_t month);

#endif
