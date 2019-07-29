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

	// Get the label with a trailing ellipsis
	char* true_label = Trailing_Label(
	ctx, label, Button_Content_Rect(nk_widget_bounds(ctx), ctx->style.button));

	// Render the button and grab the result
	bool button_clicked = nk_button_label(ctx, true_label) != 0;

	// Delete the new label
	delete [] true_label;

	return Check_And_Run_Button_Clicked(button, button_clicked, interactable);
}

bool Render_Button_Image(
BUTTON* button, struct nk_context* ctx, IMAGE* image, bool interactable) {
	// Setup button style
	Setup_Button_Style(button, ctx, interactable);

	// Check to load the image, then render the button with it
	Check_And_Load_Image_In_Current_Rect(image, ctx);
	bool button_clicked = nk_button_image(
	ctx, nk_image_ptr(&image->image)) != 0;

	return Check_And_Run_Button_Clicked(button, button_clicked, interactable);
}

bool Render_Button_Label_With_Buffer(BUTTON* button, struct nk_context* ctx,
const char* label, bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Button_Label(button, ctx, label, interactable);
}

bool Render_Button_Image_With_Buffer(
BUTTON* button, struct nk_context* ctx, IMAGE* image, bool interactable) {
	nk_label(ctx, "", 0);
	return Render_Button_Image(button, ctx, image, interactable);
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

struct nk_rect Button_Content_Rect(
struct nk_rect r, struct nk_style_button style) {
	return nk_rect(
	r.x + style.padding.x + style.border + style.rounding,
	r.y + style.padding.y + style.border + style.rounding,
	r.w - (2 * style.padding.x + style.border + style.rounding * 2),
	r.h - (2 * style.padding.y + style.border + style.rounding * 2));
}
