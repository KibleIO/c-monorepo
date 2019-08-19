#ifndef LAYOUT_UTILITIES_H_
#define LAYOUT_UTILITIES_H_

#include "../NK_BASE.h"

/*
PANEL SIZING: used to get the size of the current panel and properly
calculate ratios and exact sizes
*/

// Usable height/width of the current "panel" being laid out
// A panel could be a window or a subgroup in a window, such as a
// menu, tooltip, or popup
float Current_Usable_Panel_Height(const struct nk_context*);
float Current_Usable_Panel_Width(const struct nk_context*);
float Remaining_Usable_Panel_Height(const struct nk_context*);
float Current_Row_Height(const struct nk_context*);

float Usable_Panel_Height(
const struct nk_style*, enum nk_panel_type, int total_height);
float Usable_Panel_Width(
const struct nk_style*, enum nk_panel_type, int total_width);
float Row_Height(const struct nk_panel*);

struct nk_vec2 Get_Panel_Padding(const struct nk_style*, enum nk_panel_type);
float Get_Panel_Border(const struct nk_style*, enum nk_panel_type);

#endif
