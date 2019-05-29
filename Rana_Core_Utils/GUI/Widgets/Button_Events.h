#ifndef BUTTON_EVENTS_H_
#define BUTTON_EVENTS_H_

#include <stdint.h>
#include <iostream>

struct BUTTON_CLICKED_EVENT_DATA {
	void* ui;	// Pointer to the UI structure that generated the button
	// Identifies the UI structure that rendered the clicked button
	uint8_t invokerID;
	// Identifies the button with the click that created the event
	uint8_t buttonID;

	/*
	NOTE: 		both ids above expect a psuedo-enum value defined as a macro
	EXAMPLE:	#define OS_UI_ADD 0
	*/
};

BUTTON_CLICKED_EVENT_DATA* Button_Clicked_Event_Data(void*,
	uint8_t owner_id, uint8_t button_id);

// Universal function used by screen sequencers (like MAIN_SCREEN, for instance)
// to render the correct screen by id
void Update_Screen(void* uint8_t_screen_id_ptr,
	void* BUTTON_CLICKED_EVENT_DATA_ptr);

#endif // BUTTON_EVENTS_H_
