#include "Abstract_Selectable_Radio.h"

void Initialize_ABSTRACT_SELECTABLE_RADIO_Uniform(
ABSTRACT_SELECTABLE_RADIO* radio, bool require_one_selected, uint8_t size,
ABSTRACT_SELECTABLE selectable) {
	ABSTRACT_SELECTABLE selectables[ABSTRACT_SELECTABLE_RADIO_MAX_SELECTABLES];
	for (uint8_t i = 0; i < size; i++) {
		selectables[i] = selectable;
	}
	Initialize_ABSTRACT_SELECTABLE_RADIO_Array(
	radio, require_one_selected, size, selectables);
}

void Initialize_ABSTRACT_SELECTABLE_RADIO_Vararg(
ABSTRACT_SELECTABLE_RADIO* radio, bool require_one_selected, uint8_t size,
/*ABSTRACT_SELECTABLE*/...) {
	ABSTRACT_SELECTABLE selectables[ABSTRACT_SELECTABLE_RADIO_MAX_SELECTABLES];
	va_array(selectables, size, ABSTRACT_SELECTABLE);
	Initialize_ABSTRACT_SELECTABLE_RADIO_Array(
	radio, require_one_selected, size, selectables);
}

void Initialize_ABSTRACT_SELECTABLE_RADIO_Array(
ABSTRACT_SELECTABLE_RADIO* radio, bool require_one_selected, uint8_t size,
ABSTRACT_SELECTABLE array[]) {
	for (uint8_t i = 0; i < size; i++) {
		radio->selectables[i] = array[i];
	}

	radio->size = size;
	radio->requireOneSelected = require_one_selected;
	radio->rendering = -1;
}

bool Begin_Selectable_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO* radio, struct nk_context* ctx, const char* title,
nk_flags flags, uint8_t index) {
	if (index < radio->size) {
		// Render the selectable at the index
		ABSTRACT_SELECTABLE* selectable = &radio->selectables[index];
		bool begun = Begin_ABSTRACT_SELECTABLE(selectable, ctx, title, flags);

		if (begun) {
			radio->rendering = index;
			Update_Selected_ABSTRACT_SELECTABLE_RADIO(radio);
		}
		else {
			radio->rendering = -1;
		}

		return begun;
	}
	else {
		return false;
	}
}

bool Begin_Selectable_ABSTRACT_SELECTABLE_RADIO_With_Buffer(
ABSTRACT_SELECTABLE_RADIO* radio, struct nk_context* ctx, const char* title,
nk_flags flags, uint8_t index) {
	nk_label(ctx, "", 0);
	return Begin_Selectable_ABSTRACT_SELECTABLE_RADIO(
	radio, ctx, title, flags, index);
}

void End_Selectable_ABSTRACT_SELECTABLE_RADIO(struct nk_context* ctx) {
	nk_group_end(ctx);
}

// ACCESSORS
int16_t Index_Of_Selected(ABSTRACT_SELECTABLE_RADIO* radio) {
	bool** refs = Selectable_State_Refs_ABSTRACT_SELECTABLE_RADIO(radio);
	int16_t ret = Index_Of_Selected((const bool**)refs, radio->size);
	delete [] refs;
	return ret;
}

bool Selectable_Clicked_ABSTRACT_SELECTABLE_RADIO(
const ABSTRACT_SELECTABLE_RADIO* radio) {
	const ABSTRACT_SELECTABLE* rendering =
	Selectable_Rendering_ABSTRACT_SELECTABLE_RADIO(radio);

	if (rendering != NULL) {
		return ABSTRACT_SELECTABLE_Clicked(rendering);
	}
	else {
		return false;
	}
}

const ABSTRACT_SELECTABLE* Selectable_Rendering_ABSTRACT_SELECTABLE_RADIO(
const ABSTRACT_SELECTABLE_RADIO* radio) {
	if (radio->rendering >= 0 && radio->rendering < radio->size) {
		return &radio->selectables[radio->rendering];
	}
	else {
		return NULL;
	}
}

// HELPERS
void Update_Selected_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO* radio) {
	bool** refs = Selectable_State_Refs_ABSTRACT_SELECTABLE_RADIO(radio);

	Enforce_Radio_On_Clicked(
	refs, radio->size, radio->rendering, radio->requireOneSelected,
	Selectable_Clicked_ABSTRACT_SELECTABLE_RADIO(radio));

	delete [] refs;
}

void Select_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO* radio, uint8_t index) {
	bool** refs = Selectable_State_Refs_ABSTRACT_SELECTABLE_RADIO(radio);

	Radio_Select(
	refs, radio->size, index, radio->requireOneSelected);

	delete [] refs;
}

void Deselect_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO* radio, uint8_t index) {
	bool** refs = Selectable_State_Refs_ABSTRACT_SELECTABLE_RADIO(radio);

	Radio_Deselect(
	refs, radio->size, index, radio->requireOneSelected);

	delete [] refs;
}

bool** Selectable_State_Refs_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO* radio) {
	bool** refs = new bool*[radio->size];
	for (uint8_t i = 0; i < radio->size; i++) {
		refs[i] = &radio->selectables[i].selected;
	}
	return refs;
}
