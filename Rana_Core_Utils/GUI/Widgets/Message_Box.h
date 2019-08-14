#ifndef MESSAGE_BOX_H_
#define MESSAGE_BOX_H_

#include "Utils/Message_Box_Args.h"
#include "Panel.h"
#include "Label.h"
#include "Button.h"
#include "../NK_BASE.h"
#include "../../Utilities/vararg_ext.h"

#define MAX_MESSAGE_BOX_BUTTON_LABEL_LENGTH 64
#define MESSAGE_BOX_NO_BUTTON_CLICKED	-1

struct MESSAGE_BOX {
	PANEL panel;
	struct nk_style_text textStyle;
	struct nk_style_text informativeTextStyle;
	struct nk_style_button defaultButtonStyle;
	struct nk_style_button miscButtonStyle;
};

MESSAGE_BOX Message_Box(
PANEL, struct nk_style_text text_style,
struct nk_style_text informative_text_style,
struct nk_style_button default_button, struct nk_style_button misc_button);
MESSAGE_BOX Message_Box(
PANEL, struct nk_style_text text_style,
struct nk_style_button default_button, struct nk_style_button misc_button);
MESSAGE_BOX Message_Box(
PANEL, struct nk_style_text text_style,
struct nk_style_text informative_text_style, struct nk_style_button button);
MESSAGE_BOX Message_Box(
PANEL, struct nk_style_text text_style,
struct nk_style_button button);

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
int8_t Render_MESSAGE_BOX(MESSAGE_BOX*, MESSAGE_BOX_ARGS);

void Delete_MESSAGE_BOX(MESSAGE_BOX*);

// Standalone functions - specify everything
int8_t Render_Message(
PANEL, struct nk_style_button default_button_style,
struct nk_style_button misc_button_style, struct nk_style_text text_style,
struct nk_style_text informative_text_style, MESSAGE_BOX_ARGS);

#endif
