#ifndef NK_WINDOW_HEADER_STYLE_H_
#define NK_WINDOW_HEADER_STYLE_H_

#include "../NK_BASE.h"
#include "palette.h"
#include "Nk_Button_Style.h"

#define NK_WINDOW_HEADER_DEFAULT_PADDING nk_vec2(10, 0)
#define NK_WINDOW_HEADER_DEFAULT_LABEL_PADDING nk_vec2(10, 0)

struct nk_style_window_header Nk_Window_Header_Style();
struct nk_style_window_header Nk_Window_Header_Style(
struct nk_color background, struct nk_color foreground);

void Set_Nk_Window_Header_Style(
struct nk_style_window_header*, struct nk_color background,
struct nk_color foreground);
void Set_Nk_Window_Header_Style(
struct nk_style_window_header*, struct nk_color background,
struct nk_style_button close_button, struct nk_style_button minimize_button,
struct nk_color label_color);
void Set_Nk_Window_Header_Style_Defaults(struct nk_style_window_header*);

#endif
