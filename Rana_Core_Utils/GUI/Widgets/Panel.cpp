#include "Panel.h"

PANEL Panel(
struct nk_style_window style, struct nk_style_scrollbar hbar,
struct nk_style_scrollbar vbar) {
	PANEL panel;
	Initialize_Panel(&panel, style, hbar, vbar);
	return panel;
}
PANEL Panel_HBar(struct nk_style_window style, struct nk_style_scrollbar hbar) {
	PANEL panel;
	Initialize_Panel_HBar(&panel, style, hbar);
	return panel;
}
PANEL Panel_VBar(struct nk_style_window style, struct nk_style_scrollbar vbar) {
	PANEL panel;
	Initialize_Panel_VBar(&panel, style, vbar);
	return panel;
}
PANEL Panel(struct nk_style_window style) {
	PANEL panel;
	Initialize_Panel(&panel, style);
	return panel;
}

void Initialize_Panel(PANEL* panel, struct nk_style_window style,
struct nk_style_scrollbar hbar, struct nk_style_scrollbar vbar) {
	panel->style = style;
	panel->horizontalScrollbar = hbar;
	panel->verticalScrollbar = vbar;
}

void Initialize_Panel_HBar(PANEL* panel, struct nk_style_window style,
struct nk_style_scrollbar hbar) {
	Initialize_Panel(panel, style, hbar, hbar);
}

void Initialize_Panel_VBar(PANEL* panel, struct nk_style_window style,
struct nk_style_scrollbar vbar) {
	Initialize_Panel(panel, style, vbar, vbar);
}

void Initialize_Panel(PANEL* panel, struct nk_style_window style) {
	Initialize_Panel(panel, style,
		Nk_Scrollbar_Style(),
		Nk_Scrollbar_Style()
	);
}

int Start_Window(PANEL* panel, struct nk_context* ctx, const char* title,
struct nk_rect rect, nk_flags flags) {
	Setup_Style(panel, ctx);
	return nk_begin(ctx, title, rect, flags);
}

int Start_Window_Titled(
PANEL* panel, struct nk_context* ctx, const char* name, const char* title,
struct nk_rect bounds, nk_flags flags) {
	Setup_Style(panel, ctx);
	return nk_begin_titled(ctx, name, title, bounds, flags);
}

int Start_Group(PANEL* panel, struct nk_context* ctx, const char* title,
nk_flags flags) {
	Setup_Style(panel, ctx);
	return nk_group_begin(ctx, title, flags);
}

int Start_Group_With_Buffer(PANEL* panel, struct nk_context* ctx,
const char* title, nk_flags flags) {
	nk_label(ctx, "", 0);
	return Start_Group(panel, ctx, title, flags);
}

int Start_Transformed_Group(
PANEL* panel, struct nk_context* ctx, const char* title, nk_flags flags,
RECT_TRANSFORM transform) {
	struct nk_rect widget_bounds = nk_widget_bounds(ctx);
	struct nk_rect group_rect = Transform_Rect(widget_bounds, transform);
	float upper_buffer_height = abs(widget_bounds.y - group_rect.y);

	cout << "Group widget bounds:   " << Rect_Str(widget_bounds) << endl;
	cout << "Group rect calculated: " << Rect_Str(group_rect) << endl;

	if (upper_buffer_height > 0.0) {
		Layout_Row_Single(
		ctx, Buffer_And_Breadth(Exact_Size(upper_buffer_height), Exact_Size(
		group_rect.h)), Buffer_And_Breadth(Exact_Size(
		abs(widget_bounds.x - group_rect.x)), Exact_Size(group_rect.w)));
	}
	else {
		Layout_Row_Single(
		ctx, Breadth(Exact_Size(group_rect.h)), Buffer_And_Breadth(
		Exact_Size(abs(widget_bounds.x - group_rect.x)), Exact_Size(
		group_rect.w)));
	}


	nk_label(ctx, "", 0);
	cout << "Group area laid out:  " << Rect_Str(nk_widget_bounds(ctx)) << endl;

	return Start_Group(panel, ctx, title, flags);
}

int Start_Transformed_Group_With_Buffer(
PANEL* panel, struct nk_context* ctx, const char* title, nk_flags flags,
RECT_TRANSFORM transform) {
	nk_label(ctx, "", 0);
	return Start_Transformed_Group(panel, ctx, title, flags, transform);
}

int Start_Popup(
PANEL* panel, struct nk_context* ctx, enum nk_popup_type type,
const char *title, nk_flags flags, struct nk_rect rect) {
	Setup_Style(panel, ctx);
	return nk_popup_begin(ctx, type, title, flags, rect);
}

int Start_Popup_With_Buffer(
PANEL* panel, struct nk_context* ctx, enum nk_popup_type type,
const char *title, nk_flags flags, struct nk_rect rect) {
	nk_label(ctx, "", 0);
	return Start_Popup(panel, ctx, type, title, flags, rect);
}

void Delete_Panel(PANEL* panel) {
	(void)panel; // Void cast to bypass warning
}

void Setup_Style(PANEL* panel, struct nk_context* ctx) {
	ctx->style.window = panel->style;
	ctx->style.scrollh = panel->horizontalScrollbar;
	ctx->style.scrollv = panel->verticalScrollbar;
}
