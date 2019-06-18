#include "Calendar.h"

void Initialize_CALENDAR_UI(
CALENDAR_UI* ui, struct nk_color background, struct nk_color foreground,
struct nk_color selected, struct nk_color text) {
	time_t time_structure = time(NULL);
	ui->calendar = *localtime(&time_structure);
	ui->currentMonth = ui->calendar.tm_mon;

	Initialize_Panel(&ui->dateSelectionGroup, Nk_Window_Style(background));

	// Initialize each pacakge of buttons that
	for(uint8_t i = 0; i < TOTAL_MONTHS; i++) {
		Initialize_Button_Package_Radio(
		&ui->dayOfMonthButtons[i], Nk_Button_Style(foreground, text),
		Nk_Button_Style(selected, background),
		Nk_Button_Style_Dormant(LIGHT_GRAY, DARK_GRAY), DAYS_IN_MONTH[i]);
	}
	// Select the button on the package corresponding to the current date
	Select_Button(
	&ui->dayOfMonthButtons[ui->calendar.tm_mon], ui->calendar.tm_mday - 1);

	Initialize_Push_Button(
	&ui->monthSelectButtons, Nk_Button_Style(foreground, text),
	Nk_Button_Style_Dormant(LIGHT_GRAY, DARK_GRAY));

	Initialize_Push_Button(
	&ui->monthLabel, Nk_Button_Style_Dormant(foreground, text),
	Nk_Button_Style_Dormant(LIGHT_GRAY, DARK_GRAY));
}

void Render_CALENDAR_UI(CALENDAR_UI* ui, GUI* gui) {
	if (
	Start_Group(&ui->dateSelectionGroup, gui->NK_Context, "calendar group",
	NK_WINDOW_NO_SCROLLBAR)) {
		uint8_t current_button = 0;
		uint8_t weekday_of_first_day = dayofweek(
		1, ui->currentMonth, ui->calendar.tm_year);

		// Layout the header where current month is displayed and adjusted
		Layout_Row_Symmetric_Odd_Pairs(
		gui->NK_Context, Breadth(Ratio_Of_Total(0.25)), 2,
		Breadth(Ratio_Of_Total(0.2)), Breadth(Ratio_Of_Total(0.6)));

		// If back button is pressed, move current month back once
		if (
		Render_Button_Label_With_Buffer(&ui->monthSelectButtons,
		gui->NK_Context, "<")) {
			ui->currentMonth = (ui->currentMonth - 1) % TOTAL_MONTHS;
		}

		Render_Button_Label_With_Buffer(
		&ui->monthLabel, gui->NK_Context,
		MONTH_NAMES[ui->currentMonth].c_str(), NK_TEXT_CENTERED);

		// If forward button is pressed, more current month forward once
		if (
		Render_Button_Label_With_Buffer(&ui->monthSelectButtons,
		gui->NK_Context, ">")) {
			ui->currentMonth = (ui->currentMonth + 1) % TOTAL_MONTHS;
		}

		// Loop through each row in the calendar
		for(uint8_t row = 0; row < CALENDAR_TOTAL_ROWS; row++) {

			// Layout a row with seven equally sized columns
			Layout_Row_Homogenous(
			gui->NK_Context, Buffer_And_Breadth(Ratio_Of_Total(0.01),
			Ratio_Of_Total(0.15)), DAYS_IN_WEEK, Buffer_And_Breadth(
			Ratio_Of_Total(CALENDAR_BUFFER_WIDTH_RATIO),
			Ratio_Of_Total(CALENDAR_BUTTON_WIDTH_RATIO)));

			// Render each day in this week
			for(
			uint8_t column = 0; column < DAYS_IN_WEEK; column++) {
				// Render the button if this is not the first row,
				// or if it is the first row, render only if we've
				// past the first day of the week for the current month
				if (
				row != 0 || column >= weekday_of_first_day) {
					if (
					Render_Button_Label_With_Buffer(
					&ui->dayOfMonthButtons[ui->currentMonth],
					gui->NK_Context, to_string(current_button + 1).c_str(),
					current_button)) {
						Resolve_Date_Selection(
						ui, current_button, ui->currentMonth);
					}
					current_button++;
				}
				// If we aren't rendering the button, render empty widgets
				else {
					nk_label(gui->NK_Context, "", 0);
					nk_label(gui->NK_Context, "", 0);
				}
			} // END foreach column
		} // END foreach row
	} // END if

	nk_group_end(gui->NK_Context);
}

void Render_CALENDAR_UI_With_Buffer(CALENDAR_UI* ui, GUI* gui) {
	nk_label(gui->NK_Context, "", 0);
	Render_CALENDAR_UI(ui, gui);
}

void Delete_CALENDAR_UI(CALENDAR_UI* ui) {
	for(uint8_t i = 0; i < TOTAL_MONTHS; i++) {
		Delete_Button_Package(&ui->dayOfMonthButtons[i]);
	}
}

void Resolve_Date_Selection(CALENDAR_UI* ui, uint8_t day, uint8_t month) {
	Deselect_All_Buttons(&ui->dayOfMonthButtons[ui->calendar.tm_mon]);
	Select_Button(&ui->dayOfMonthButtons[month], day);
	tmset(&ui->calendar, day + 1, month);
}
