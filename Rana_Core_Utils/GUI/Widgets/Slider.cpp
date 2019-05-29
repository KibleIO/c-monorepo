#include "Slider.h"

void Initialize_SLIDER(
SLIDER* slider, int step, int starting_val, struct nk_style_slider style) {
	Initialize_SLIDER(slider, 0, 100, step, starting_val, style);
}

void Initialize_SLIDER(
SLIDER* slider, int min, int max, int step, int starting_val,
struct nk_style_slider style) {
	slider->min = min;
	slider->max = max;
	slider->step = step;
	slider->value = starting_val;
	slider->style = style;
}

bool Render_SLIDER(SLIDER* slider, struct nk_context* ctx) {
	// Assign the slider's style and render it
	ctx->style.slider = slider->style;
	int val_changed = nk_slider_int(
	ctx, slider->min, &slider->value, slider->max, slider->step);
	return val_changed != 0;
}

bool Render_SLIDER_With_Buffer(SLIDER* slider, struct nk_context* ctx) {
	nk_label(ctx, "", 0);
	return Render_SLIDER(slider, ctx);
}

void Delete_SLIDER(SLIDER* slider) {
	(void)slider;	// Suppress arg unused warning
}

int Get_Value_SLIDER(const SLIDER* slider) {
	return slider->value;
}
