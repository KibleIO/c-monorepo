#include "Nav_Buttons.h"

/*
NAV_BUTTON MEMBERS
*/

void Initialize_Nav_Button(NAV_BUTTON* button, BUTTON data, uint8_t source,
uint8_t destination) {
	button->data = data;
	button->sourceID = source;
	button->destinationID = destination;
	Initialize_Multicast_Function_Pointer(&button->navButtonClickedEvent);
}

void Initialize_Nav_Button(NAV_BUTTON* button, struct nk_style_button normal,
struct nk_style_button dormant, uint8_t source, uint8_t destination) {
	BUTTON data;
	Initialize_Push_Button(&data, normal, dormant);
	Initialize_Nav_Button(button, data, source, destination);
}

bool Render_Nav_Button_Label(NAV_BUTTON* button, struct nk_context* ctx,
const char* title, bool interactable) {
	// Render the base button data
	bool button_clicked = Render_Button_Label(
	&button->data, ctx, title, interactable);
	
	// Invoke this nav button's event
	if(button_clicked) {
		Invoke_All_Function_Pointers(&button->navButtonClickedEvent, button);
	}
	return button_clicked;
}

void Delete_Nav_Button(NAV_BUTTON* button) {
	Delete_Button(&button->data);
	Delete_Multicast_Function_Pointer(&button->navButtonClickedEvent);
}

void Add_Navigate_Screen_Pointer(NAV_BUTTON* button, uint8_t* screenID) {
	Add_Function_Pointer(&button->navButtonClickedEvent,
		Navigate_Screen, screenID);
}

/*
NAV_BUTTON_PACKAGE MEMBERS
*/

void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE* package, BUTTON data,
uint8_t source, uint8_t* destinations, uint8_t totalButtons) {
	// Allocate nav buttons
	package->buttons = new NAV_BUTTON[totalButtons];
	package->totalButtons = totalButtons;
	// Setup each nav button with the same source and given destinations
	for(uint8_t i = 0; i < totalButtons; i++) {
		Initialize_Nav_Button(&package->buttons[i], data,
			source, destinations[i]);
	}
	// Start current button at the first button
	package->currentButton = -1;
}

void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE* package,
struct nk_style_button normal, struct nk_style_button dormant,
uint8_t source, uint8_t* destinations, uint8_t total_buttons) {
	BUTTON data;
	Initialize_Push_Button(&data, normal, dormant);
	Initialize_Nav_Button_Package(package, data, source,
		destinations, total_buttons);
}

void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE* package, BUTTON* datas,
uint8_t source, uint8_t* destinations, uint8_t total_buttons) {
	// Allocate nav buttons
	package->buttons = new NAV_BUTTON[total_buttons];
	package->totalButtons = total_buttons;
	// Setup each nav button with the same source and given destinations
	for(uint8_t i = 0; i < total_buttons; i++) {
		Initialize_Nav_Button(&package->buttons[i], datas[i],
			source, destinations[i]);
	}
	// Start current button at the first button
	package->currentButton = -1;
}

void Initialize_Nav_Button_Package(NAV_BUTTON_PACKAGE* package,
BUTTON_STYLE* styles, uint8_t source, uint8_t* destinations,
uint8_t total_buttons) {
	BUTTON* datas = new BUTTON[total_buttons];
	for(uint8_t i = 0; i < total_buttons; i++) {
		Initialize_Push_Button(&datas[i], styles[i]);
	}
	Initialize_Nav_Button_Package(package, datas, source, destinations,
		total_buttons);
}

bool Render_Next_Nav_Button_Label(NAV_BUTTON_PACKAGE* package,
struct nk_context* ctx, const char* title, bool interactable) {
	Next_Nav_Button(package);
	return Render_Nav_Button_Label(&package->buttons[package->currentButton],
		ctx, title, interactable);
}

void Delete_Nav_Button_Package(NAV_BUTTON_PACKAGE* package) {
	for(uint8_t i = 0; i < package->totalButtons; i++) {
		Delete_Nav_Button(&package->buttons[i]);
	}
	delete [] package->buttons;
}

void Add_Function_Pointer_To_All(NAV_BUTTON_PACKAGE* package,
void(*function)(void*, void*), void* subscriberData) {
	// Add the given function pointer to every button clicked event
	// on every nav button
	for(uint8_t i = 0; i < package->totalButtons; i++) {
		Add_Function_Pointer(&package->buttons[i].navButtonClickedEvent, function,
			subscriberData);
	}
}

void Add_Navigate_Screen_Pointer_To_All(NAV_BUTTON_PACKAGE* package,
uint8_t* screenID) {
	Add_Function_Pointer_To_All(package, Navigate_Screen, screenID);
}

void Next_Nav_Button(NAV_BUTTON_PACKAGE* package) {
	package->currentButton =
		(package->currentButton + 1) % package->totalButtons;
}

void Set_Next_Nav_Button(NAV_BUTTON_PACKAGE* package, uint8_t button_index) {
	if(button_index < package->totalButtons) {
		package->currentButton = button_index - 1;
	}
}

void Navigate_Screen(void* screen_id_ptr, void* nav_button_ptr) {
	uint8_t* screen_id = (uint8_t*)screen_id_ptr;
	NAV_BUTTON* nav_button = (NAV_BUTTON*)nav_button_ptr;
	*screen_id = nav_button->destinationID;
}
