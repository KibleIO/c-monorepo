#ifndef PANEL_H_
#define PANEL_H_

#include "../GUI.h"
#include "../Style.h"

struct PANEL {
	struct nk_style_window style;
	struct nk_style_scrollbar horizontalScrollbar;
	struct nk_style_scrollbar verticalScrollbar;
};

PANEL Panel(
struct nk_style_window, struct nk_style_scrollbar hbar,
struct nk_style_scrollbar vbar);
PANEL Panel_HBar(struct nk_style_window, struct nk_style_scrollbar hbar);
PANEL Panel_VBar(struct nk_style_window, struct nk_style_scrollbar vbar);
PANEL Panel(struct nk_style_window);

void Initialize_Panel(
PANEL*, struct nk_style_window, struct nk_style_scrollbar hbar,
struct nk_style_scrollbar vbar);
void Initialize_Panel_HBar(
PANEL*, struct nk_style_window, struct nk_style_scrollbar hbar);
void Initialize_Panel_VBar(
PANEL*, struct nk_style_window, struct nk_style_scrollbar vbar);
void Initialize_Panel(PANEL*, struct nk_style_window);

int Start_Window(
PANEL*, struct nk_context*, const char*, struct nk_rect, nk_flags);
int Start_Window_Titled(
PANEL*, struct nk_context*, const char* name, const char* title,
struct nk_rect, nk_flags);

int Start_Group(PANEL*, struct nk_context*, const char*, nk_flags);
int Start_Group_With_Buffer(PANEL*, struct nk_context*, const char*, nk_flags);

void Delete_Panel(PANEL*);

// Setup style on the context using the panel's style
void Setup_Style(PANEL*, struct nk_context*);

#endif
