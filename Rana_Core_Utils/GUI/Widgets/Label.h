#ifndef LABEL_H_
#define LABEL_H_

#include "../GUI.h"
#include "../Layout/Layout_Engine.h"
#include "../Style.h"

struct LABEL {
	struct nk_style_text style;
};

void Initialize_Label(LABEL*, struct nk_style_text);
void Render_Label(LABEL*, struct nk_context*, const char*, nk_flags);
void Render_Label_With_Buffer(
LABEL*, struct nk_context*, const char*, nk_flags);
void Delete_Label(LABEL*);

// Get label with trailing ellipsis, using current rect as the rect
char* Trailing_Label_In_Current_Rect(
struct nk_context*, const char* original_label);

// Given an nk_context, a label, and the rect it needs to fit in,
// return a label which may have a trailing ellipsis if the label given
// won't fit inside the rect given
char* Trailing_Label(
struct nk_context*, const char* original_label, struct nk_rect rect);

// Width of an ellipsis on the current nk_context font
float Ellipsis_Width(struct nk_context*, float height);

// Given a null-terminated label, the width it sould fit in,
// and the margin it needs, return a pointer to the part of the string
// right before the margin space
char* Label_Margin_Cursor(
struct nk_context*, float height, const char* label,
float margin_width, float container_width);

#endif
