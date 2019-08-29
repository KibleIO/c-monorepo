#ifndef PULLOUT_H_
#define PULLOUT_H_

#include "../NK_BASE.h"
#include "Panel.h"
#include "../Layout/Rect_Transform.h"
#include "../utilities.h"

#define PULLOUT_NOT_VISIBLE	0
#define PULLOUT_VISIBLE		1
#define PULLOUT_HOVERED		2

static const struct nk_rect NULL_RECT = {-8192.0f, -8192.0f, 16384, 16384};

// Slight expansion of the widget area
// Detects as hovering the widget even if the cursor is slightly off
static const RECT_TRANSFORM WIDGET_HOVER_EXPANSION = Rect_Transform(
Size_Transform(Ratio_Of_Parent(1.3), Ratio_Of_Parent(1.3)), ALIGNMENT_CENTER);

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
void End_Widget_PULLOUT(const PULLOUT*, struct nk_context*);

// Begin the window part of the pullout
// NOTE: do NOT call between nk_begin/end
bool Begin_Window_PULLOUT(PULLOUT*, struct nk_context*, const char*, nk_flags);
void End_Window_PULLOUT(const PULLOUT*, struct nk_context*);

void Delete_PULLOUT(PULLOUT*);

// ACCESSORS
struct nk_rect Widget_Area(const PULLOUT*);
bool PULLOUT_Hovered(const PULLOUT*);

// Helpers
bool Widget_Is_Hovered_PULLOUT(const struct nk_context*, struct nk_rect);

#endif
