#include "Checkbox.h"

void Initialize_Checkbox(CHECKBOX* checkbox, struct nk_color background_color,
struct nk_color check_color, struct nk_color text_color) {
	Initialize_Checkbox(
	checkbox,
	Nk_Toggle_Style(background_color, check_color, text_color));
}

void Initialize_Checkbox(CHECKBOX* checkbox, struct nk_style_toggle style) {
	checkbox->style = style;
	checkbox->value = 0;
}

bool Render_Checkbox(CHECKBOX* checkbox, struct nk_context* ctx,
const char* label) {
	// Store previous value so we can check when it changes
	int prev_value = checkbox->value;

	ctx->style.checkbox = checkbox->style;
	nk_checkbox_label(ctx, label, &checkbox->value);

	// Checkbox was clicked if the value changed
	return prev_value != checkbox->value;
}

bool Render_Checkbox_With_Buffer(CHECKBOX* checkbox, struct nk_context* ctx,
const char* label) {
	nk_label(ctx, "", 0);
	return Render_Checkbox(checkbox, ctx, label);
}

void Delete_Checkbox(CHECKBOX* checkbox) {
	(void)checkbox;	// Bypass compiler warning
}

bool Checkbox_State(const CHECKBOX* checkbox) {
	return checkbox->value != 0;
}

void Set_Checkbox_State(CHECKBOX* checkbox, bool state) {
	checkbox->value = state;
}
