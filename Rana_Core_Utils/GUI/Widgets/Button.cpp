#include "Button.h"

/*
BUTTON STYLE
*/

BUTTON_STYLE Button_Style(struct nk_style_button normal,
struct nk_style_button toggled, struct nk_style_button dormant) {
	BUTTON_STYLE style;
	style.normal = normal;
	style.toggled = toggled;
	style.dormant = dormant;
	return style;
}

BUTTON_STYLE Button_Style_Dormant(struct nk_style_button normal,
struct nk_style_button dormant) {
	return Button_Style(normal, normal, dormant);
}

BUTTON_STYLE Button_Style_Toggle(struct nk_style_button normal,
struct nk_style_button toggled) {
	return Button_Style(normal, toggled, normal);
}

BUTTON_STYLE Button_Style_Normal(struct nk_style_button normal) {
	return Button_Style(normal, normal, normal);
}

/*
BUTTON
*/

void Initialize_Button(BUTTON* button, uint8_t type, BUTTON_STYLE style) {
	button->type = type;
	button->toggleState = false;
	button->style = style;
	Initialize_Multicast_Function_Pointer(&button->buttonClickedEvent);
}

void Initialize_Toggle_Button(BUTTON* button, BUTTON_STYLE style) {
	Initialize_Button(button, BUTTON_TYPE_TOGGLE, style);
}

void Initialize_Toggle_Button(BUTTON* button, struct nk_style_button normal,
struct nk_style_button toggled, struct nk_style_button dormant) {
	Initialize_Button(button, BUTTON_TYPE_TOGGLE,
		Button_Style(normal, toggled, dormant));
}

void Initialize_Toggle_Button(
BUTTON* button, struct nk_style_button normal, struct nk_style_button toggled) {
	Initialize_Button(
	button, BUTTON_TYPE_TOGGLE, Button_Style_Toggle(normal, toggled));
}

void Initialize_Push_Button(BUTTON* button, BUTTON_STYLE style) {
	Initialize_Button(button, BUTTON_TYPE_PUSH, style);
}

void Initialize_Push_Button(BUTTON* button, struct nk_style_button normal,
struct nk_style_button dormant) {
	Initialize_Button(button, BUTTON_TYPE_PUSH,
		Button_Style(normal, normal, dormant));
}

void Initialize_Push_Button(BUTTON* button, struct nk_style_button normal) {
	Initialize_Button(button, BUTTON_TYPE_PUSH, Button_Style_Normal(normal));
}

bool Render_Button_Label(BUTTON* button, struct nk_context* ctx,
const char* label, bool interactable) {
	// Setup button style
	Setup_Button_Style(button, ctx, interactable);

	// Render the button and grab the result
	bool button_clicked = nk_button_label(
	ctx, Trailing_Label(ctx, label,
	Button_Content_Rect(nk_widget_bounds(ctx), ctx->style.button))) != 0;

	return Check_And_Run_Button_Clicked(button, button_clicked, interactable);
}

bool Render_Button_Image(
BUTTON* button, struct nk_context* ctx, struct nk_image img,
bool interactable) {
	// Setup button style
	Setup_Button_Style(button, ctx, interactable);

	// Render the button and grab the result
	bool button_clicked = nk_button_image(ctx, img) != 0;

	return Check_And_Run_Button_Clicked(button, button_clicked, interactable);
}

bool Render_Button_Label_With_Buffer(BUTTON* button, struct nk_context* ctx,
const char* label, bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Button_Label(button, ctx, label, interactable);
}

bool Render_Button_Label_Buffered(
BUTTON* button, struct nk_context* ctx, const char* label, bool buffered,
bool interactable) {
	if(buffered) {
		return Render_Button_Label_With_Buffer(
		button, ctx, label, interactable);
	}
	else {
		return Render_Button_Label(button, ctx, label, interactable);
	}
}

void Delete_Button(BUTTON* button) {
	Delete_Multicast_Function_Pointer(&button->buttonClickedEvent);
}

void Set_Button_Toggle_State(BUTTON* button, bool state) {
	if(button->type != BUTTON_TYPE_PUSH) {
		button->toggleState = state;
	}
}

void Toggle_Button_State(BUTTON* button) {
	Set_Button_Toggle_State(button, !button->toggleState);
}

void Setup_Button_Style(
BUTTON* button, struct nk_context* ctx, bool interactable) {
	// If button is interactable, set toggled or normal appearance
	if(interactable) {
		if(button->toggleState) {
			ctx->style.button = button->style.toggled;
		}
		else {
			ctx->style.button = button->style.normal;
		}
	}
	// If button is not interactable, set dormant appearance
	else {
		ctx->style.button = button->style.dormant;
	}
}

bool Check_And_Run_Button_Clicked(
BUTTON* button, bool clicked, bool interactable) {
	// If the button is clicked and it is interactable, invoke the event
	if(clicked && interactable) {
		// Switch the button's toggle state
		Toggle_Button_State(button);
		// Invoke the event
		Invoke_All_Function_Pointers(&button->buttonClickedEvent, button);
	}

	return clicked && interactable;
}

/*
BUTTON PACKAGE
*/

void Initialize_Button_Package(BUTTON_PACKAGE* package, uint8_t type,
BUTTON_STYLE style, uint8_t total_buttons, bool require_one_selected) {
	BUTTON_STYLE* styles = new BUTTON_STYLE[total_buttons];
	for(uint8_t i = 0; i < total_buttons; i++) {
		styles[i] = style;
	}
	Initialize_Button_Package(
	package, type, styles, total_buttons, require_one_selected);
}

void Initialize_Button_Package(BUTTON_PACKAGE* package, uint8_t type,
BUTTON_STYLE* styles, uint8_t total_buttons, bool require_one_selected) {
	// Set button types to toggle or push
	// depending on the pacakge type
	uint8_t button_types = type == BUTTON_PACKAGE_TYPE_PUSH ?
		BUTTON_TYPE_PUSH :
		BUTTON_TYPE_TOGGLE;
	// Allocate space for the buttons
	package->buttons = new BUTTON[total_buttons];
	// Setup each button
	for(uint8_t i = 0; i < total_buttons; i++) {
		Initialize_Button(&package->buttons[i], button_types, styles[i]);
	}

	// Initialize data
	package->type = type;
	package->totalButtons = total_buttons;
	package->buttonCapacity = total_buttons;
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

void Initialize_Button_Package_Radio(BUTTON_PACKAGE* package,
struct nk_style_button normal, struct nk_style_button toggled,
struct nk_style_button dormant, uint8_t total_buttons,
bool require_one_selected) {
	Initialize_Button_Package(package, BUTTON_PACKAGE_TYPE_RADIO,
		Button_Style(normal, toggled, dormant), total_buttons,
		require_one_selected);
}

void Initialize_Button_Package_Radio(BUTTON_PACKAGE* package,
BUTTON_STYLE* styles, uint8_t total_buttons, bool require_one_selected) {
	Initialize_Button_Package(package, BUTTON_PACKAGE_TYPE_RADIO,
		styles, total_buttons, require_one_selected);
}

void Initialize_Button_Package_Select(BUTTON_PACKAGE* package,
struct nk_style_button normal, struct nk_style_button toggled,
struct nk_style_button dormant, uint8_t total_buttons,
bool require_one_selected) {
	Initialize_Button_Package(package, BUTTON_PACKAGE_TYPE_SELECT,
		Button_Style(normal, toggled, dormant), total_buttons,
		require_one_selected);
}

bool Render_Button_Label(BUTTON_PACKAGE* package, struct nk_context* ctx,
const char* label, uint8_t button_index, bool interactable) {
	if(button_index < package->totalButtons) {
		// Store the button previously selected
		uint8_t prev_selected = Toggled_Button_Index(package);

		// Set the current button to the index given
		package->currentButton = button_index;
		bool button_clicked = Render_Button_Label(
			&package->buttons[package->currentButton], ctx, label, interactable);

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
struct nk_image img, uint8_t button_index, bool interactable) {
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

bool Render_Button_Label_With_Buffer(BUTTON_PACKAGE* package,
struct nk_context* ctx, const char* label, uint8_t button_index,
bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Button_Label(package, ctx, label, button_index, interactable);
}

bool Render_Button_Label_Buffered(
BUTTON_PACKAGE* package, struct nk_context* ctx, const char* label,
uint8_t button_index, bool buffered, bool interactable) {
	if(buffered) {
		return Render_Button_Label_With_Buffer(
		package, ctx, label, button_index, interactable);
	}
	else {
		return Render_Button_Label(
		package, ctx, label, button_index, interactable);
	}
}

bool Render_Next_Button_Label(BUTTON_PACKAGE* package, struct nk_context* ctx,
const char* label, bool interactable) {
	// Update current button
	Next_Button(package);
	// Render the current button
	return Render_Button_Label(package, ctx, label,
		package->currentButton, interactable);
}

bool Render_Next_Button_Image(BUTTON_PACKAGE* package, struct nk_context* ctx,
struct nk_image img, bool interactable) {
	Next_Button(package);
	return Render_Button_Image(package, ctx, img, interactable);
}

bool Render_Next_Button_Label_With_Buffer(BUTTON_PACKAGE* package,
struct nk_context* ctx, const char* label, bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Next_Button_Label(package, ctx, label, interactable);
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

void Next_Button(BUTTON_PACKAGE* package) {
	package->currentButton = (package->currentButton + 1) %
		package->totalButtons;
}

void Update_Button_Toggle_States(BUTTON_PACKAGE* package,
uint8_t button_selected) {
	if(package->type == BUTTON_PACKAGE_TYPE_RADIO) {
		// Untoggle all buttons besides the one selected
		for(uint8_t i = 0; i < package->totalButtons; i++) {
			if(i != button_selected) {
				package->buttons[i].toggleState = false;
			} // endif
		} // end for
	} // end if
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

// BUTTON PACKAGE HELPERS
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

struct nk_rect Button_Content_Rect(
struct nk_rect r, struct nk_style_button style) {
	return nk_rect(
	r.x + style.padding.x + style.border + style.rounding,
	r.y + style.padding.y + style.border + style.rounding,
	r.w - (2 * style.padding.x + style.border + style.rounding * 2),
	r.h - (2 * style.padding.y + style.border + style.rounding * 2));
}
