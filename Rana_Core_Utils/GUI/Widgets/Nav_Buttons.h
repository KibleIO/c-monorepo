#ifndef NAV_BUTTONS_H_
#define NAV_BUTTONS_H_

#include "../GUI.h"
#include <stdint.h>
#include "Button.h"
#include "Button_Events.h"
#include "../../Utilities/Function_Pointers.h"
#include "../Style.h"

struct NAV_BUTTON {
	// Button data. Provides a kind of psuedo-inheritance from BUTTON
	BUTTON data;
	// ID of the screen the nav button is rendered on
	uint8_t sourceID;
	// ID of the screen the nav button leads to
	uint8_t destinationID;
	// Event called when the nav button is clicked
	// PASS: NAV_BUTTON*
	MULTICAST_FUNCTION_POINTER navButtonClickedEvent;
};

void Initialize_Nav_Button(NAV_BUTTON*, BUTTON, uint8_t source,
	uint8_t destination);
void Initialize_Nav_Button(NAV_BUTTON*, struct nk_style_button normal,
	struct nk_style_button dormant, uint8_t source, uint8_t destination);
bool Render_Nav_Button_Label(NAV_BUTTON*, struct nk_context*,
	const char* title, bool interactable = true);
void Delete_Nav_Button(NAV_BUTTON*);

// Add the nav screen function as an event to the nav button
void Add_Navigate_Screen_Pointer(NAV_BUTTON*, uint8_t* screenId);

struct NAV_BUTTON_PACKAGE {
	NAV_BUTTON* buttons;
	uint8_t totalButtons;
	uint8_t currentButton;
};

// Initialize all nav buttons with the same button data
void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE*, BUTTON, uint8_t source,
	uint8_t* destinations, uint8_t totalButtons);
// Initialize all nav buttons with the same button styles
void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE*,
	struct nk_style_button normal, struct nk_style_button dormant,
	uint8_t source, uint8_t* destinations, uint8_t totalButtons);
void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE*, BUTTON_STYLE,
	uint8_t source, uint8_t* destinations, uint8_t totalButtons);
// Initialize all nav buttons with custom button data
void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE*, BUTTON*,
	uint8_t source, uint8_t* destination, uint8_t totalButtons);
// Initialize all nav buttons with custom styles
void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE*, BUTTON_STYLE*,
	uint8_t source, uint8_t* destinations, uint8_t totalButtons);

bool Render_Next_Nav_Button_Label(NAV_BUTTON_PACKAGE*, struct nk_context*,
	const char* title, bool interactable = true);

void Delete_Nav_Button_Package(NAV_BUTTON_PACKAGE*);

// Add the function pointer to every nav button clicked event
void Add_Function_Pointer_To_All(NAV_BUTTON_PACKAGE*, void(*ptr)(void*, void*),
	void* subscriberData);
// Add a pointer to void Navigate_Screen(void*, void*)
// to each nav button clicked event
void Add_Navigate_Screen_Pointer_To_All(NAV_BUTTON_PACKAGE*, uint8_t* screenId);

// Go to the next nav button
void Next_Nav_Button(NAV_BUTTON_PACKAGE*);
// Set the index of the button that will be rendered on the next call
// to "Render_Next_Nav_Button_xxx"
void Set_Next_Nav_Button(NAV_BUTTON_PACKAGE*, uint8_t button_index);

/*
Standard screen update function
Updates the integer in the first pointer to the destination value in the
nav button pointer in the second pointer
First pointer is uint8_t*, second is NAV_BUTTON*
*/
void Navigate_Screen(void* screen_id_ptr, void* nav_button_ptr);

#endif // BUTTONS_H_
