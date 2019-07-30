#include "Button_Package.h"

void Initialize_Button_Package(BUTTON_PACKAGE* package, uint8_t type,
BUTTON_STYLE style, uint8_t total_buttons, bool require_one_selected) {
	BUTTON_STYLE* styles = new BUTTON_STYLE[total_buttons];
	for(uint8_t i = 0; i < total_buttons; i++) {
		styles[i] = style;
	}
	Initialize_Button_Package(
	package, type, styles, total_buttons, require_one_selected);
	delete [] styles;
}

void Initialize_Button_Package(BUTTON_PACKAGE* package, uint8_t type,
BUTTON_STYLE* styles, uint8_t total_buttons, bool require_one_selected) {
	// Set button types to toggle or push
	// depending on the pacakge type
	uint8_t button_types = type == BUTTON_PACKAGE_TYPE_PUSH ?
	BUTTON_TYPE_PUSH : BUTTON_TYPE_TOGGLE;

	// Allocate space for the buttons
	if (total_buttons > 0) {
		package->buttons = new BUTTON[total_buttons];
		package->buttonCapacity = total_buttons;
	}
	else {
		package->buttons = new BUTTON[BUTTON_PACKAGE_DEFAULT_INITIAL_CAPACITY];
		package->buttonCapacity = BUTTON_PACKAGE_DEFAULT_INITIAL_CAPACITY;
	}

	// Setup each button
	for(uint8_t i = 0; i < total_buttons; i++) {
		Initialize_Button(&package->buttons[i], button_types, styles[i]);
	}

	// Initialize data
	package->type = type;
	package->totalButtons = total_buttons;
	package->requireOneSelected = require_one_selected;
	Initialize_Multicast_Function_Pointer(&package->buttonClickedEvent);
	package->currentButton = -1;

	// If a button must stay selected, select one immediately
	if(require_one_selected) {
		Select_Button(package, 0);
	}
}

void Initialize_Button_Package_Push(BUTTON_PACKAGE* package,
struct nk_style_button normal, struct nk_style_button dormant,
uint8_t total_buttons) {
	Initialize_Button_Package(package, BUTTON_PACKAGE_TYPE_PUSH,
		Button_Style(normal, normal, dormant), total_buttons, false);
}

void Initialize_Button_Package_Push(
BUTTON_PACKAGE* package, struct nk_style_button normal, uint8_t total_buttons) {
	Initialize_Button_Package_Push(package, normal, normal, total_buttons);
}

void Initialize_Button_Package_Push(BUTTON_PACKAGE* package,
BUTTON_STYLE* styles, uint8_t total_buttons) {
	Initialize_Button_Package(package, BUTTON_PACKAGE_TYPE_PUSH,
		styles, total_buttons, false);
}

void Initialize_Button_Package_Push(BUTTON_PACKAGE* package) {
	Initialize_Button_Package_Push(
	package, Nk_Button_Style(DARK_GRAY, LIGHT_GRAY), 0);
}

void Initialize_Button_Package_Radio(BUTTON_PACKAGE* package,
struct nk_style_button normal, struct nk_style_button toggled,
struct nk_style_button dormant, uint8_t total_buttons,
bool require_one_selected) {
	Initialize_Button_Package(
	package, BUTTON_PACKAGE_TYPE_RADIO, Button_Style(normal, toggled, dormant),
	total_buttons, require_one_selected);
}

void Initialize_Button_Package_Radio(
BUTTON_PACKAGE* package, struct nk_style_button normal,
struct nk_style_button toggled, uint8_t total_buttons,
bool require_one_selected) {
	Initialize_Button_Package_Radio(
	package, normal, toggled, normal, total_buttons, require_one_selected);
}

void Initialize_Button_Package_Radio(BUTTON_PACKAGE* package,
BUTTON_STYLE* styles, uint8_t total_buttons, bool require_one_selected) {
	Initialize_Button_Package(package, BUTTON_PACKAGE_TYPE_RADIO,
		styles, total_buttons, require_one_selected);
}

void Initialize_Button_Package_Radio(
BUTTON_PACKAGE* package, bool require_one_selected) {
	Initialize_Button_Package_Radio(
	package, Nk_Button_Style(DARK_GRAY, LIGHT_GRAY),
	Nk_Button_Style(DARK_GRAY, LIGHT_GRAY), 0, require_one_selected);
}

void Initialize_Button_Package_Select(BUTTON_PACKAGE* package,
struct nk_style_button normal, struct nk_style_button toggled,
struct nk_style_button dormant, uint8_t total_buttons,
bool require_one_selected) {
	Initialize_Button_Package(package, BUTTON_PACKAGE_TYPE_SELECT,
		Button_Style(normal, toggled, dormant), total_buttons,
		require_one_selected);
}

bool Render_Button_Label(
BUTTON_PACKAGE* package, struct nk_context* ctx, const char* label,
bool trailing, uint8_t button_index, bool interactable) {
	if(button_index < package->totalButtons) {
		// Store the button previously selected
		uint8_t prev_selected = Toggled_Button_Index(package);

		// Set the current button to the index given
		package->currentButton = button_index;
		bool button_clicked = Render_Button_Label(
		&package->buttons[package->currentButton], ctx, label, trailing,
		interactable);

		// If the package always has one selected,
		// make sure this frame did not cause it to deselect
		Enforce_At_Least_One_Selected(package, prev_selected);

		// If this button is clicked, invoke the event
		return Check_And_Run_Button_Clicked(package, button_clicked);
	} else {
		return false;
	}
}

bool Render_Button_Image(BUTTON_PACKAGE* package, struct nk_context* ctx,
IMAGE* img, uint8_t button_index, bool interactable) {
	if(button_index < package->totalButtons) {
		// Store the button previously selected
		uint8_t prev_selected = Toggled_Button_Index(package);

		// Set the current button to the index given
		package->currentButton = button_index;
		bool button_clicked = Render_Button_Image(
		&package->buttons[package->currentButton], ctx, img, interactable);

		// If the package always has one selected,
		// make sure this frame did not cause it to deselect
		Enforce_At_Least_One_Selected(package, prev_selected);

		// If this button is clicked, invoke the event
		return Check_And_Run_Button_Clicked(package, button_clicked);
	} else {
		return false;
	}
}

bool Render_Button_Symbol_Label(
BUTTON_PACKAGE* package, struct nk_context* ctx, enum nk_symbol_type symbol,
const char* label, bool trailing, nk_flags alignment, uint8_t button_index,
bool interactable) {
	if(button_index < package->totalButtons) {
		// Store the button previously selected
		uint8_t prev_selected = Toggled_Button_Index(package);

		// Set the current button to the index given
		package->currentButton = button_index;
		bool button_clicked = Render_Button_Symbol_Label(
		&package->buttons[package->currentButton], ctx, symbol, label, trailing,
		alignment, interactable);

		// If the package always has one selected,
		// make sure this frame did not cause it to deselect
		Enforce_At_Least_One_Selected(package, prev_selected);

		// If this button is clicked, invoke the event
		return Check_And_Run_Button_Clicked(package, button_clicked);
	} else {
		return false;
	}
}

bool Render_Button_Label_With_Buffer(BUTTON_PACKAGE* package,
struct nk_context* ctx, const char* label, bool trailing, uint8_t button_index,
bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Button_Label(
	package, ctx, label, trailing, button_index, interactable);
}

bool Render_Button_Image_With_Buffer(
BUTTON_PACKAGE* package, struct nk_context* ctx, IMAGE* image,
uint8_t button_index, bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Button_Image(package, ctx, image, button_index, interactable);
}

bool Render_Button_Symbol_Label_With_Buffer(
BUTTON_PACKAGE* package, struct nk_context* ctx, enum nk_symbol_type symbol,
const char* label, bool trailing, nk_flags alignment, uint8_t button_index,
bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Button_Symbol_Label(
	package, ctx, symbol, label, trailing, alignment, button_index,
	interactable);
}

bool Render_Button_Label_Buffered(
BUTTON_PACKAGE* package, struct nk_context* ctx, const char* label,
bool trailing, uint8_t button_index, bool buffered, bool interactable) {
	if(buffered) {
		return Render_Button_Label_With_Buffer(
		package, ctx, label, trailing, button_index, interactable);
	}
	else {
		return Render_Button_Label(
		package, ctx, label, trailing, button_index, interactable);
	}
}

bool Render_Next_Button_Label(BUTTON_PACKAGE* package, struct nk_context* ctx,
const char* label, bool trailing, bool interactable) {
	// Update current button
	Next_Button(package);
	// Render the current button
	return Render_Button_Label(
	package, ctx, label, trailing, package->currentButton, interactable);
}

bool Render_Next_Button_Image(BUTTON_PACKAGE* package, struct nk_context* ctx,
IMAGE* img, bool interactable) {
	Next_Button(package);
	return Render_Button_Image(package, ctx, img, interactable);
}

bool Render_Next_Button_Label_With_Buffer(BUTTON_PACKAGE* package,
struct nk_context* ctx, const char* label, bool trailing, bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Next_Button_Label(
	package, ctx, label, trailing, interactable);
}

void Delete_Button_Package(BUTTON_PACKAGE* package) {
	for(uint8_t i = 0; i < package->totalButtons; i++) {
		Delete_Button(&package->buttons[i]);
	}
	delete [] package->buttons;
	Delete_Multicast_Function_Pointer(&package->buttonClickedEvent);
}

void Select_Button(BUTTON_PACKAGE* package, uint8_t button_index) {
	if(button_index < package->totalButtons) {
		Set_Button_Toggle_State(&package->buttons[button_index], true);
		Update_Button_Toggle_States(package, button_index);
	}
}

void Deselect_Button(BUTTON_PACKAGE* package, uint8_t button_index) {
	if(button_index < package->totalButtons) {
		// Assign previously selected button
		uint8_t prev_selected = Toggled_Button_Index(package);

		Set_Button_Toggle_State(&package->buttons[button_index], false);

		// Make sure at least one button is selected, if required
		Enforce_At_Least_One_Selected(package, prev_selected);
	}
}

void Deselect_All_Buttons(BUTTON_PACKAGE* package) {
	for(uint8_t i = 0; i < package->totalButtons; i++) {
		Deselect_Button(package, i);
	}
}

int8_t Toggled_Button_Index(const BUTTON_PACKAGE* package) {
	for(uint8_t i = 0; i < package->totalButtons; i++) {
		if(package->buttons[i].toggleState) {
			return i;
		}
	}
	return -1;
}

BUTTON Toggled_Button(const BUTTON_PACKAGE* package) {
	int8_t index = Toggled_Button_Index(package);
	if(index >= 0) {
		return package->buttons[index];
	}
	else {
		return {};
	}
}

uint8_t* Toggled_Button_Indeces(const BUTTON_PACKAGE* package,
uint8_t* total_selected) {
	uint8_t* indeces = new uint8_t[package->totalButtons];
	*total_selected = 0;
	for(uint8_t i = 0; i < package->totalButtons; i++) {
		// If this button is toggled, put its index
		// into the list of indeces
		if(package->buttons[i].toggleState) {
			indeces[(*total_selected++)] = i;
		}
	}
	return indeces;
}

BUTTON* Toggled_Buttons(const BUTTON_PACKAGE* package,
uint8_t* total_selected) {
	BUTTON* toggled_buttons = new BUTTON[package->totalButtons];
	*total_selected = 0;
	for(uint8_t i = 0; i < package->totalButtons; i++) {
		// If this button is toggled, put its index
		// into the list of indeces
		if(package->buttons[i].toggleState) {
			toggled_buttons[(*total_selected++)] = package->buttons[i];
		}
	}
	return toggled_buttons;
}

/*
Container functions
*/

void Add_Button(
BUTTON_PACKAGE* package, struct nk_style_button normal,
struct nk_style_button toggled, struct nk_style_button dormant) {
	if (package->totalButtons >= package->buttonCapacity) {
		Resize_Button_Package(package, package->buttonCapacity * 2);
	}

	BUTTON new_button;

	// Initialize button to match package type
	switch (package->type) {
		case BUTTON_PACKAGE_TYPE_PUSH:
			Initialize_Push_Button(&new_button, normal, dormant);
			break;
		case BUTTON_PACKAGE_TYPE_RADIO:
		case BUTTON_PACKAGE_TYPE_SELECT:
			Initialize_Toggle_Button(&new_button, normal, toggled, dormant);
			break;
		default:
			break;
	}

	// Assign new button to end of the array
	package->buttons[package->totalButtons++] = new_button;
}

void Add_Button(
BUTTON_PACKAGE* package, struct nk_style_button normal,
struct nk_style_button other) {
	Add_Button(package, normal, other, other);
}

void Add_Button(BUTTON_PACKAGE* package, struct nk_style_button normal) {
	Add_Button(package, normal, normal);
}

void Resize_Button_Package(BUTTON_PACKAGE* package, uint8_t new_cap) {
	if (new_cap > package->buttonCapacity) {
		BUTTON* new_buttons = new BUTTON[new_cap];

		for (uint8_t i = 0; i < package->totalButtons; i++) {
			new_buttons[i] = package->buttons[i];
		}

		delete [] package->buttons;
		package->buttons = new_buttons;
		package->buttonCapacity = new_cap;
	}
}

// BUTTON PACKAGE HELPERS
void Next_Button(BUTTON_PACKAGE* package) {
	package->currentButton = (package->currentButton + 1) %
		package->totalButtons;
}

void Update_Button_Toggle_States(
BUTTON_PACKAGE* package, uint8_t button_selected) {
	if(package->type == BUTTON_PACKAGE_TYPE_RADIO) {
		// Untoggle all buttons besides the one selected
		for(uint8_t i = 0; i < package->totalButtons; i++) {
			if(i != button_selected) {
				Set_Button_Toggle_State(&package->buttons[i], false);
			} // endif
		} // end for
	} // end if
}

void Enforce_At_Least_One_Selected(
BUTTON_PACKAGE* package, uint8_t prev_selected) {
	int8_t current_selected = Toggled_Button_Index(package);

	if(current_selected < 0 && package->requireOneSelected) {
		Select_Button(package, prev_selected);
	}
}

bool Check_And_Run_Button_Clicked(BUTTON_PACKAGE* package, bool clicked) {
	// If this button is clicked, invoke the event
	if(clicked) {
		Update_Button_Toggle_States(package, package->currentButton);
		Invoke_All_Function_Pointers(&package->buttonClickedEvent, package);
	}
	return clicked;
}
