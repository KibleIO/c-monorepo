#include "Radio_Package_Utils.h"

/*
FUNCTIONS
*/

void Radio_Select(
bool* refs[], uint8_t size, uint8_t index, bool require_one_selected) {
	Make_Radio_Selection(refs, size, index, require_one_selected, true);
}
void Radio_Deselect(
bool* refs[], uint8_t size, uint8_t index, bool require_one_selected) {
	Make_Radio_Selection(refs, size, index, require_one_selected, false);
}
void Make_Radio_Selection(
bool* refs[], uint8_t size, uint8_t index, bool require_one_selected,
bool selection) {
	if (index < size) {
		*refs[index] = selection;
		Enforce_Radio(refs, size, index, require_one_selected);
	}
}

int16_t Index_Of_Selected(const bool* refs[], uint8_t size) {
	for (uint8_t i = 0; i < size; i++) {
		if (*refs[i]) {
			return i;
		}
	}
	return -1;
}

void Enforce_Radio(
bool* refs[], uint8_t size, int16_t index_clicked, bool require_one_selected) {
	Deselect_All_But_One(refs, size, index_clicked);
	Select_If_None_Selected(
	refs, size, index_clicked, require_one_selected);
}

void Enforce_Radio_On_Clicked(
bool* refs[], uint8_t size, int16_t index_clicked, bool require_one_selected,
bool clicked) {
	if (clicked) {
		Enforce_Radio(refs, size, index_clicked, require_one_selected);
	}
}

/*
HELPERS
*/

void Deselect_All_But_One(bool* refs[], uint8_t size, int16_t ignored) {
	for (uint8_t i = 0; i < size; i++) {
		if (i != ignored) {
			*refs[i] = false;
		}
	}
}

void Select_If_None_Selected(
bool* refs[], uint8_t size, int16_t select, bool require_one_selected) {
	if (require_one_selected) {
		int16_t current_selected = Index_Of_Selected((const bool**)refs, size);

		if (current_selected < 0) {
			*refs[select] = true;
		}
	}
}
