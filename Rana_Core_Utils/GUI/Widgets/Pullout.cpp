#include "Pullout.h"

void Initialize_PULLOUT(
PULLOUT* pullout, PANEL widget_panel, PANEL window_panel,
RECT_TRANSFORM widget_to_window) {
	pullout->widgetPanel = widget_panel;
	pullout->windowPanel = window_panel;
	pullout->widgetArea = NULL_RECT;
	pullout->widgetToWindow = widget_to_window;
	pullout->hovered = false;
	pullout->windowActive = false;
}

bool Begin_Widget_PULLOUT(
PULLOUT* pullout, struct nk_context* ctx, const char* title, nk_flags flags) {
	pullout->widgetArea = nk_widget_bounds(ctx);
	pullout->hovered = Widget_Is_Hovered_PULLOUT(ctx, pullout->widgetArea) ||
	pullout->windowActive;

	return Start_Group(&pullout->widgetPanel, ctx, title, flags);
}

bool Begin_Widget_PULLOUT_With_Buffer(
PULLOUT* pullout, struct nk_context* ctx, const char* title, nk_flags flags) {
	nk_label(ctx, "", 0);
	return Begin_Widget_PULLOUT(pullout, ctx, title, flags);
}

void End_Widget_PULLOUT(const PULLOUT* pullout, struct nk_context* ctx) {
	End_Group(&pullout->widgetPanel, ctx);
}

bool Begin_Window_PULLOUT(
PULLOUT* pullout, struct nk_context* ctx, const char* title, nk_flags flags) {
	bool window_visible = false;

	// If the pullout widget is hovered, start the window
	if (pullout->hovered) {
		window_visible = Start_Window(
		&pullout->windowPanel, ctx, title, Transform_Rect(pullout->widgetArea,
		pullout->widgetToWindow), flags);

		// If window is rendered, update window active
		if (window_visible) {
			pullout->windowActive = nk_window_is_hovered(ctx);
			nk_window_set_focus(ctx, title);
		}
	}
	// If not, disable the window
	else {
		pullout->windowActive = false;
	}

	return window_visible;
}

void End_Window_PULLOUT(const PULLOUT* pullout, struct nk_context* ctx) {
	End_Window(&pullout->windowPanel, ctx);
}

void Delete_PULLOUT(PULLOUT* pullout) {
	Delete_Panel(&pullout->widgetPanel);
	Delete_Panel(&pullout->windowPanel);
}

struct nk_rect Widget_Area(const PULLOUT* pullout) {
	return pullout->widgetArea;
}

bool PULLOUT_Hovered(const PULLOUT* pullout) {
	return pullout->hovered;
}

bool Widget_Is_Hovered_PULLOUT(
const struct nk_context* ctx, struct nk_rect widget_rect) {
	return nk_input_is_mouse_hovering_rect(
	&ctx->input, Transform_Rect(widget_rect, WIDGET_HOVER_EXPANSION));
}
