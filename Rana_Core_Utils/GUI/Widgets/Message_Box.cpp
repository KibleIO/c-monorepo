#include "Message_Box.h"

MESSAGE_BOX Message_Box(
PANEL panel, struct nk_style_text text,
struct nk_style_text informative_text, struct nk_style_button default_button,
struct nk_style_button misc_button) {
	MESSAGE_BOX message;
	Initialize_MESSAGE_BOX(
	&message, panel, text, informative_text, default_button, misc_button);
	return message;
}
MESSAGE_BOX Message_Box(
PANEL panel, struct nk_style_text text, struct nk_style_button default_button,
struct nk_style_button misc_button) {
	MESSAGE_BOX message;
	Initialize_MESSAGE_BOX(&message, panel, text, default_button, misc_button);
	return message;
}
MESSAGE_BOX Message_Box(
PANEL panel, struct nk_style_text text,
struct nk_style_text informative_text, struct nk_style_button misc_button) {
	MESSAGE_BOX message;
	Initialize_MESSAGE_BOX(
	&message, panel, text, informative_text, misc_button);
	return message;
}
MESSAGE_BOX Message_Box(
PANEL panel, struct nk_style_text text,
struct nk_style_button misc_button) {
	MESSAGE_BOX message;
	Initialize_MESSAGE_BOX(&message, panel, text, misc_button);
	return message;
}

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

int8_t Render_MESSAGE_BOX(MESSAGE_BOX* message, MESSAGE_BOX_ARGS args) {
	return Render_Message(
	message->panel, message->defaultButtonStyle, message->miscButtonStyle,
	message->textStyle, message->informativeTextStyle, args);
}

void Delete_MESSAGE_BOX(MESSAGE_BOX* message) {
	Delete_Panel(&message->panel);
}

int8_t Render_Message(
PANEL panel, struct nk_style_button default_button_style,
struct nk_style_button misc_button_style, struct nk_style_text text_style,
struct nk_style_text informative_text_style, MESSAGE_BOX_ARGS args) {
	LABEL text_widget;
	LABEL informative_text_widget;
	BUTTON default_button_widget;
	BUTTON misc_button_widget;
	BUTTON using_button;
	int8_t ret = MESSAGE_BOX_NO_BUTTON_CLICKED;

	Initialize_Label(&text_widget, text_style);
	Initialize_Label(&informative_text_widget, informative_text_style);
	Initialize_Push_Button(&default_button_widget, default_button_style);
	Initialize_Push_Button(&misc_button_widget, misc_button_style);

	if (Start_Window(&panel, args.ctx, args.title, args.bounds, args.flags)) {
		// Layout main text
		Layout_Row_Single_Full(args.ctx, Breadth(Ratio_Of_Total(0.4)));
		Render_Label_With_Buffer(
		&text_widget, args.ctx, args.text, NK_TEXT_CENTERED);

		// Layout informative text
		Layout_Row_Single(
		args.ctx, Breadth(Ratio_Of_Total(0.3)),
		Buffer_And_Breadth(Ratio_Of_Total(0.15), Ratio_Of_Total(0.7)));

		if (args.informativeText != NULL) {
			Render_Label_Wrap_With_Buffer(
			&informative_text_widget, args.ctx, args.informativeText);
		}

		// Layout all buttons evenly spaced
		Layout_Row_Evenly_Spaced_Items(
		args.ctx, Buffer_And_Breadth(Ratio_Of_Total(0.05), Ratio_Of_Total(0.15)),
		Ratio_Of_Total(0.3), args.totalButtons, true);

		// Render each button
		for (uint8_t i = 0; i < args.totalButtons; i++) {
			// Setup the current button
			if (i == args.defaultButton) {
				using_button = default_button_widget;
			}
			else {
				using_button = misc_button_widget;
			}

			// Render the current button
			if (
			Render_Button_Label_With_Buffer(&using_button, args.ctx,
			args.buttonLabels[i], false)) {
				ret = i;
			}
		}
	}
	nk_end(args.ctx);

	// If this is a modal window, set its focus
	if (args.modal) {
		nk_window_set_focus(args.ctx, args.title);
	}

	return ret;
}
