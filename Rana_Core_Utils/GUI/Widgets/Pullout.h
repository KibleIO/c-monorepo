#ifndef PULLOUT_H_
#define PULLOUT_H_

#include "../GUI.h"
#include "Panel.h"
#include "../Layout/Rect_Transform.h"
#include "../../Utilities/utilities.h"

#define PULLOUT_NOT_VISIBLE	0
#define PULLOUT_VISIBLE		1
#define PULLOUT_HOVERED		2

static const struct nk_rect NULL_RECT = {-8192.0f, -8192.0f, 16384, 16384};
static const RECT_TRANSFORM WIDGET_HOVER_EXPANSION = Rect_Transform(
1.3, 1.3, ALIGN_CENTER, ALIGN_CENTER);

// PULLOUT has two parts:
//	1) Widget: render a group embedded inside of a window. When hovered,
//		the pullout renders another window
//		Render INSIDE nk_begin/nk_end
//	2) Window: rendered when the pullout widget is hovered over
//		Render OUTSIDE nk_begin/nk_end
struct PULLOUT {
	// Control the style of the panel of the pullout
	// for the widget and the window
	PANEL widgetPanel;
	PANEL windowPanel;

	// Area that creates the pullout window when hovered
	struct nk_rect widgetArea;

	// Parameters used to get the hovered area from the default area
	RECT_TRANSFORM widgetToWindow;
	bool hovered;	// True if the pullout area is hovered over
	bool windowActive;	// True if the pullout window is currently active
};

void Initialize_PULLOUT(
PULLOUT*, PANEL widget_panel, PANEL window_panel, RECT_TRANSFORM);

// Begin the widget part of the pullout
// NOTE: you MUST call between nk_begin/nk_end
bool Begin_Widget_PULLOUT(PULLOUT*, struct nk_context*, const char*, nk_flags);
bool Begin_Widget_PULLOUT_With_Buffer(
PULLOUT*, struct nk_context*, const char*, nk_flags);
void End_Widget_PULLOUT(struct nk_context*);

// Begin the window part of the pullout
// NOTE: do NOT call between nk_begin/end
bool Begin_Window_PULLOUT(PULLOUT*, struct nk_context*, const char*, nk_flags);
void End_Window_PULLOUT(struct nk_context*);

void Delete_PULLOUT(PULLOUT*);

// Helpers
bool Widget_Is_Hovered_PULLOUT(struct nk_context*, struct nk_rect);

#endif
