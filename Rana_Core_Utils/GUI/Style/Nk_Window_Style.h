#ifndef NK_WINDOW_STYLE_H_
#define NK_WINDOW_STYLE_H_

#include "Nk_Window_Header_Style.h"

struct nk_style_window Nk_Window_Style();
struct nk_style_window Nk_Window_Style(struct nk_color);
struct nk_style_window Nk_Window_Style(struct nk_color, float rounding);
struct nk_style_window Nk_Window_Style(
struct nk_color, struct nk_vec2 scrollbar_size);
struct nk_style_window Nk_Window_Style(
struct nk_color, struct nk_style_window_header);
struct nk_style_window Nk_Window_Style(
struct nk_color, struct nk_vec2 scrollbar_size,
struct nk_style_window_header header);

void Set_Nk_Window_Style(struct nk_style_window*);
void Set_Nk_Window_Style(struct nk_style_window*, struct nk_color);
void Set_Nk_Window_Style(
struct nk_style_window*, struct nk_color, float rounding);
void Set_Nk_Window_Style(
struct nk_style_window*, struct nk_color, struct nk_vec2 scrollbar_size);
void Set_Nk_Window_Style(
struct nk_style_window*, struct nk_color, struct nk_vec2 scrollbar_size,
struct nk_style_window_header header);
void Set_Nk_Window_Style_Defaults(struct nk_style_window*);

#endif
