#include "Label.h"

void Initialize_Label(LABEL* label, struct nk_style_text style) {
	label->style = style;
}
void Render_Label(LABEL* label, struct nk_context* ctx, const char* text,
nk_flags flags) {
	ctx->style.text = label->style;
	nk_label(ctx, text, flags);
}
void Render_Label_With_Buffer(LABEL* label, struct nk_context* ctx,
const char* text, nk_flags flags) {
	nk_label(ctx, "", flags);
	Render_Label(label, ctx, text, flags);
}
void Render_Label_Buffered(
LABEL* label, struct nk_context* ctx, const char* text, nk_flags flags,
bool buffered) {
	if (buffered) {
		nk_label(ctx, "", 0);
	}
	Render_Label(label, ctx, text, flags);
}
void Delete_Label(LABEL* label) {
	(void)label;	// Bypass compiler warning
}

char* Trailing_Label_In_Current_Rect(
struct nk_context* ctx, const char* original_label) {
	return Trailing_Label(ctx, original_label, nk_widget_bounds(ctx));
}

char* Trailing_Label(
struct nk_context* ctx, const char* original_label, struct nk_rect rect) {
	// Create a copy of the original_label
	// +3 to guarantee that the new label can contain an ellipsis at the end
	// +1 for null terminator
	char* new_label = new char[strlen(original_label) + 4];
	strcpy(new_label, original_label);

	// Figure out the width of the text and the width that it needs to fit in
	float label_width = Font_Text_Width(
	ctx->style.font->userdata, rect.h, original_label, strlen(original_label));

	// If label is too wide, adjust the end of it with an ellipsis
	if(rect.w < label_width) {
		// Get a pointer to the part of the string that needs to be
		// replaced with an ellipsis
		char* ellipsis_cursor = Label_Margin_Cursor(
		ctx, rect.h, new_label,
		Ellipsis_Width(ctx, rect.h), rect.w);

		// Replace the part of the string now pointed to with an ellipsis
		strcpy(ellipsis_cursor, "...");
	}

	return new_label;
}

float Ellipsis_Width(struct nk_context* ctx, float height) {
	return Font_Text_Width(ctx->style.font->userdata, height, "...", 3);
}

char* Label_Margin_Cursor(
struct nk_context* ctx, float height, const char* label,
float margin_width, float container_width) {
	// Start the cursor off at the end of the label
	char* current_cursor = (char*)label + strlen(label);

	// Amount by which the full text exceeds the space
	// it needs to fit in.  Negative indicates the full text
	// does not fill up the whole space
	float excess_width = Font_Text_Width(
	ctx->style.font->userdata, height, label,
	strlen(label)) - container_width;

	// Current width of the text that the cursor is pointing to
	float current_width = Font_Text_Width(
	ctx->style.font->userdata, height,
	current_cursor, strlen(current_cursor));

	while (
	current_cursor != label && current_width < excess_width + margin_width) {
		current_width = Font_Text_Width(
		ctx->style.font->userdata, height,
		current_cursor, strlen(current_cursor));

		// If the current width is still less than
		// the excess plus margin, move it back another character
		if(current_width < excess_width + margin_width) {
			current_cursor--;
		}
	}

	return current_cursor;
}
