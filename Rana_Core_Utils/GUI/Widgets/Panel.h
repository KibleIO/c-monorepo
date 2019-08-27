#ifndef PANEL_H_
#define PANEL_H_

#include "../NK_BASE.h"
#include "../Style.h"
#include "../Layout/Rect_Transform.h"
#include "../Layout/Layout_Engine.h"

struct PANEL_END {
	void (*end)(struct nk_context*);
};
PANEL_END Panel_End(void(*end)(struct nk_context*));

#define TOTAL_PANEL_TYPES 7
static const PANEL_END END_FUNCTIONS[TOTAL_PANEL_TYPES] = {
	Panel_End(nk_end), Panel_End(nk_group_end), Panel_End(nk_popup_end),
	Panel_End(nk_contextual_end), Panel_End(nk_combo_end),
	Panel_End(nk_menu_end), Panel_End(nk_tooltip_end)
};

struct PANEL {
	struct nk_style_window style;
	struct nk_style_scrollbar horizontalScrollbar;
	struct nk_style_scrollbar verticalScrollbar;
};

/*
CONSTRUCT
*/
PANEL Panel(
struct nk_style_window, struct nk_style_scrollbar hbar,
struct nk_style_scrollbar vbar);
PANEL Panel_HBar(struct nk_style_window, struct nk_style_scrollbar hbar);
PANEL Panel_VBar(struct nk_style_window, struct nk_style_scrollbar vbar);
PANEL Panel(struct nk_style_window);

/*
INITIALIZE
*/
void Initialize_Panel(
PANEL*, struct nk_style_window, struct nk_style_scrollbar hbar,
struct nk_style_scrollbar vbar);
void Initialize_Panel_HBar(
PANEL*, struct nk_style_window, struct nk_style_scrollbar hbar);
void Initialize_Panel_VBar(
PANEL*, struct nk_style_window, struct nk_style_scrollbar vbar);
void Initialize_Panel(PANEL*, struct nk_style_window);

/*
WINDOW
*/
int Start_Window(
const PANEL*, struct nk_context*, const char*, struct nk_rect, nk_flags);
int Start_Window_Titled(
const PANEL*, struct nk_context*, const char* name, const char* title,
struct nk_rect, nk_flags);
void End_Window(const PANEL*, struct nk_context*);

/*
GROUP
*/
int Start_Group(const PANEL*, struct nk_context*, const char*, nk_flags);
int Start_Group_With_Buffer(
const PANEL*, struct nk_context*, const char*, nk_flags);

int Start_Transformed_Group(
const PANEL*, struct nk_context*, const char*, nk_flags, RECT_TRANSFORM);
int Start_Transformed_Group_With_Buffer(
const PANEL*, struct nk_context*, const char*, nk_flags, RECT_TRANSFORM);

void End_Group(const PANEL*, struct nk_context*);

/*
POPUP
*/
int Start_Popup(
const PANEL*, struct nk_context *, enum nk_popup_type, const char *title,
nk_flags flags, struct nk_rect rect);
int Start_Popup_With_Buffer(
const PANEL*, struct nk_context *, enum nk_popup_type, const char *title,
nk_flags flags, struct nk_rect rect);
void End_Popup(const PANEL*, struct nk_context*);

// Delete
void Delete_Panel(PANEL*);

/*
HELPERS
*/

void End_Panel(const PANEL*, struct nk_context*, enum nk_panel_type);
void End_Panel(
const PANEL*, struct nk_context*, void(*end_fctn)(struct nk_context*));

// Setup style on the context using the panel's style
void Setup_Style(const PANEL*, struct nk_context*);

#endif
