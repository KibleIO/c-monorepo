#ifndef MESSAGE_BOX_H_
#define MESSAGE_BOX_H_

#include "Panel.h"
#include "Label.h"
#include "Button.h"
#include "../GUI.h"
#include "../../Utilities/vararg_ext.h"

#define MAX_MESSAGE_BOX_BUTTONS 4
#define MAX_MESSAGE_BOX_BUTTON_LABEL_LENGTH 64
#define MESSAGE_BOX_NO_DEFAULT_BUTTON -1

struct MESSAGE_BOX {
	PANEL panel;
	struct nk_style_text textStyle;
	struct nk_style_text informativeTextStyle;
	struct nk_style_button defaultButtonStyle;
	struct nk_style_button miscButtonStyle;
};

void Initialize_MESSAGE_BOX(
MESSAGE_BOX*, PANEL, struct nk_style_text text_style,
struct nk_style_text informative_text_style,
struct nk_style_button default_button, struct nk_style_button misc_button);
void Initialize_MESSAGE_BOX(
MESSAGE_BOX*, PANEL, struct nk_style_text text_style,
struct nk_style_button default_button, struct nk_style_button misc_button);
void Initialize_MESSAGE_BOX(
MESSAGE_BOX*, PANEL, struct nk_style_text text_style,
struct nk_style_text informative_text_style, struct nk_style_button button);
void Initialize_MESSAGE_BOX(
MESSAGE_BOX*, PANEL, struct nk_style_text text_style,
struct nk_style_button button);

// WARNING: do NOT render a message box between 'nk_begin' and 'nk_end'
// The message box starts its own window, so it cannot be rendered in a window

int8_t Render_MESSAGE_BOX_With_Default_And_Info(
MESSAGE_BOX*, struct nk_context*, const char* title, struct nk_rect bounds,
nk_flags, bool modal, const char* text, const char* informative_text,
int8_t default_button, uint8_t total_buttons, /* const char* */...);
int8_t Render_MESSAGE_BOX_With_Default(
MESSAGE_BOX*, struct nk_context*, const char* title, struct nk_rect bounds,
nk_flags, bool modal, const char* text, int8_t default_button,
uint8_t total_buttons, /* const char* */...);
int8_t Render_MESSAGE_BOX_With_Info(
MESSAGE_BOX*, struct nk_context*, const char* title, struct nk_rect bounds,
nk_flags, bool modal, const char* text, const char* informative_text,
uint8_t total_buttons, /* const char* */...);
int8_t Render_MESSAGE_BOX(
MESSAGE_BOX*, struct nk_context*, const char* title, struct nk_rect bounds,
nk_flags, bool modal, const char* text, uint8_t total_buttons,
/* const char* */...);

void Delete_MESSAGE_BOX(MESSAGE_BOX*);

// Standalone functions - specify everything

int8_t Render_Message_With_Default_And_Info(
struct nk_context*, const char* title, struct nk_rect bounds, nk_flags,
bool modal, PANEL, struct nk_style_button default_button_style,
struct nk_style_button misc_button_style, const char* text,
struct nk_style_text text_style, const char* informative_text,
struct nk_style_text informative_text_style, int8_t default_button,
uint8_t total_buttons, /* const char* */...);
int8_t Render_Message_With_Info(
struct nk_context*, const char* title, struct nk_rect bounds, nk_flags,
bool modal, PANEL, struct nk_style_button button_style, const char* text,
struct nk_style_text text_style, const char* informative_text,
struct nk_style_text informative_text_style, uint8_t total_buttons,
/* const char* */...);
int8_t Render_Message_With_Default(
struct nk_context*, const char* title, struct nk_rect bounds, nk_flags,
bool modal, PANEL, struct nk_style_button default_button_style,
struct nk_style_button button_style, const char* text,
struct nk_style_text text_style, int8_t default_button, uint8_t total_buttons,
/* const char* */...);
int8_t Render_Message(
struct nk_context*, const char* title, struct nk_rect bounds, nk_flags,
bool modal, PANEL, struct nk_style_button misc_button, const char* text,
struct nk_style_text text_style, uint8_t total_buttons, /* const char* */...);

int8_t Render_Message(
struct nk_context*, const char* title, struct nk_rect bounds, nk_flags,
bool modal, PANEL, struct nk_style_button default_button_style,
struct nk_style_button misc_button_style, const char* text,
struct nk_style_text text_style, const char* informative_text,
struct nk_style_text informative_text_style, int8_t default_button,
const char* button_labels[], uint8_t total_buttons);

#endif
