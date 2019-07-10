#include "Message_Box.h"

void Initialize_MESSAGE_BOX(
MESSAGE_BOX* message, PANEL panel, struct nk_style_text text,
struct nk_style_text informative_text, struct nk_style_button default_button,
struct nk_style_button misc_button) {
	message->panel = panel;
	message->textStyle = text;
	message->informativeTextStyle = informative_text;
	message->defaultButtonStyle = default_button;
	message->miscButtonStyle = misc_button;
}
void Initialize_MESSAGE_BOX(
MESSAGE_BOX* message, PANEL panel, struct nk_style_text text_style,
struct nk_style_button default_button, struct nk_style_button misc_button) {
	Initialize_MESSAGE_BOX(
	message, panel, text_style, text_style, default_button, misc_button);
}
void Initialize_MESSAGE_BOX(
MESSAGE_BOX* message, PANEL panel, struct nk_style_text text_style,
struct nk_style_text informative_text_style, struct nk_style_button button) {
	Initialize_MESSAGE_BOX(
	message, panel, text_style, informative_text_style, button, button);
}
void Initialize_MESSAGE_BOX(
MESSAGE_BOX* message, PANEL panel, struct nk_style_text text_style,
struct nk_style_button button) {
	Initialize_MESSAGE_BOX(
	message, panel, text_style, text_style, button, button);
}

int8_t Render_MESSAGE_BOX_With_Default_And_Info(
MESSAGE_BOX* message, struct nk_context* ctx, const char* title,
struct nk_rect bounds, nk_flags flags, bool modal, const char* text,
const char* informative_text, int8_t default_button, uint8_t total_buttons,
/* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, message->panel,
	message->defaultButtonStyle, message->miscButtonStyle, text,
	message->textStyle, informative_text, message->informativeTextStyle,
	default_button, button_labels, total_buttons);
}
int8_t Render_MESSAGE_BOX_With_Default(
MESSAGE_BOX* message, struct nk_context* ctx, const char* title,
struct nk_rect bounds, nk_flags flags, bool modal, const char* text,
int8_t default_button, uint8_t total_buttons, /* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, message->panel,
	message->defaultButtonStyle, message->miscButtonStyle, text,
	message->textStyle, (const char*)NULL, {}, default_button, button_labels,
	total_buttons);
}
int8_t Render_MESSAGE_BOX_With_Info(
MESSAGE_BOX* message, struct nk_context* ctx, const char* title,
struct nk_rect bounds, nk_flags flags, bool modal, const char* text,
const char* informative_text, uint8_t total_buttons, /* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, message->panel,
	message->defaultButtonStyle, message->miscButtonStyle, text,
	message->textStyle, informative_text, message->informativeTextStyle,
	MESSAGE_BOX_NO_DEFAULT_BUTTON, button_labels, total_buttons);
}
int8_t Render_MESSAGE_BOX(
MESSAGE_BOX* message, struct nk_context* ctx, const char* title,
struct nk_rect bounds, nk_flags flags, bool modal, const char* text,
uint8_t total_buttons, /* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, message->panel,
	message->defaultButtonStyle, message->miscButtonStyle, text,
	message->textStyle, (const char*)NULL, {}, MESSAGE_BOX_NO_DEFAULT_BUTTON,
	button_labels, total_buttons);
}

int8_t Render_Message_With_Default_And_Info(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, PANEL panel,
struct nk_style_button default_button_style, struct nk_style_button misc_button,
const char* text, struct nk_style_text text_style, const char* informative_text,
struct nk_style_text informative_text_style, int8_t default_button,
uint8_t total_buttons, /* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, panel, default_button_style, misc_button,
	text, text_style, informative_text, informative_text_style, default_button,
	button_labels, total_buttons);
}

int8_t Render_Message_With_Info(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, PANEL panel, struct nk_style_button button_style,
const char* text, struct nk_style_text text_style,
const char* informative_text, struct nk_style_text informative_text_style,
uint8_t total_buttons, /* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, panel, button_style, button_style, text,
	text_style, informative_text, informative_text_style,
	MESSAGE_BOX_NO_DEFAULT_BUTTON, button_labels, total_buttons);
}

int8_t Render_Message_With_Default(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, PANEL panel,
struct nk_style_button default_button_style, struct nk_style_button misc_button,
const char* text, struct nk_style_text text_style, int8_t default_button,
uint8_t total_buttons, /* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, panel, default_button_style, misc_button,
	text, text_style, (const char*)NULL, {}, default_button, button_labels,
	total_buttons);
}

int8_t Render_Message(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, PANEL panel, struct nk_style_button button_style,
const char* text, struct nk_style_text text_style, uint8_t total_buttons,
/* const char* */...) {
	const char* button_labels[MAX_MESSAGE_BOX_BUTTONS];
	va_array(button_labels, total_buttons, const char*);

	return Render_Message(
	ctx, title, bounds, flags, modal, panel, button_style, button_style, text,
	text_style, (const char*)NULL, {}, MESSAGE_BOX_NO_DEFAULT_BUTTON,
	button_labels, total_buttons);
}

int8_t Render_Message(
struct nk_context* ctx, const char* title, struct nk_rect bounds,
nk_flags flags, bool modal, PANEL panel,
struct nk_style_button default_button_style,
struct nk_style_button misc_button_style, const char* text,
struct nk_style_text text_style, const char* informative_text,
struct nk_style_text informative_text_style, int8_t default_button,
const char* button_labels[], uint8_t total_buttons) {
	LABEL text_widget;
	LABEL informative_text_widget;
	BUTTON default_button_widget;
	BUTTON misc_button_widget;
	BUTTON using_button;
	int8_t ret = -1;

	Initialize_Label(&text_widget, text_style);
	Initialize_Label(&informative_text_widget, informative_text_style);
	Initialize_Push_Button(&default_button_widget, default_button_style);
	Initialize_Push_Button(&misc_button_widget, misc_button_style);

	if (Start_Window(&panel, ctx, title, bounds, flags)) {
		// Layout main text
		Layout_Row_Single_Full(ctx, Breadth(Ratio_Of_Total(0.4)));
		Render_Label_With_Buffer(&text_widget, ctx, text, NK_TEXT_CENTERED);

		// Layout informative text
		Layout_Row_Single(
		ctx, Breadth(Ratio_Of_Total(0.3)),
		Buffer_And_Breadth(Ratio_Of_Total(0.15), Ratio_Of_Total(0.7)));

		if (informative_text != NULL) {
			Render_Label_Wrap_With_Buffer(
			&informative_text_widget, ctx, informative_text);
		}

		// Layout all buttons evenly spaced
		Layout_Row_Evenly_Spaced_Items(
		ctx, Buffer_And_Breadth(Ratio_Of_Total(0.05), Ratio_Of_Total(0.15)),
		Ratio_Of_Total(0.3), total_buttons, true);

		// Render each button
		for (uint8_t i = 0; i < total_buttons; i++) {
			// Setup the current button
			if (i == default_button) {
				using_button = default_button_widget;
			}
			else {
				using_button = misc_button_widget;
			}

			// Render the current button
			if (
			Render_Button_Label_With_Buffer(&using_button, ctx,
			button_labels[i])) {
				ret = i;
			}
		}
	}
	nk_end(ctx);

	// If this is a modal window, set its focus
	if (modal) {
		nk_window_set_focus(ctx, title);
	}

	return ret;
}
