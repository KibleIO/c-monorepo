#ifndef SLIDER_H_
#define SLIDER_H_

#include "../NK_BASE.h"
#include "../Style.h"

struct SLIDER {
	int min;
	int max;
	int step;
	int value;

	// Style of the slider
	struct nk_style_slider style;
};

void Initialize_SLIDER(
SLIDER*, int step, int starting_val, struct nk_style_slider);
void Initialize_SLIDER(
SLIDER*, int min, int max, int step, int starting_val, struct nk_style_slider);

// Render the slider. Return true if the slider's value is changed
bool Render_SLIDER(SLIDER*, struct nk_context*);
bool Render_SLIDER_With_Buffer(SLIDER*, struct nk_context*);

void Delete_SLIDER(SLIDER*);

int Get_Value_SLIDER(const SLIDER*);

#endif
