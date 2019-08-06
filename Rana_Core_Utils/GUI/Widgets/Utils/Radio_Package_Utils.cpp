#include "Radio_Package_Utils.h"

int16_t Index_Of_Selected(const bool* bools[], uint8_t size) {
	for (uint8_t i = 0; i < size; i++) {
		if (bools[i]) {
			return i;
		}
	}
	return -1;
}

bool Select(bool* bools[], uint8_t size, uint8_t selection) {
	if (selection < size) {
		bool previous = *bools[selection];
		Deselect_All(bools, size);
		*bools[selection] = true;

		// Return true if not previously selected
		return !previous;
	}
	// If index out of range, no selection occurs
	else {
		return false;
	}
}

bool Deselect(
bool* bools[], uint8_t size, uint8_t deselection, bool require_one_selected) {
	if (deselection < size) {
		// If bool is true, deselect it
		// only if the package is allowed to have none selected
		if (*bools[deselection]) {
			*bools[deselection] = require_one_selected;
			return !require_one_selected;
		}
		// If bool was already false, no deselection occurs
		else {
			return false;
		}
	}
	// If deselection out of bounds, no deselection occurs
	else {
		return false;
	}
}

void Deselect_All(bool* bools[], uint8_t size, bool require_one_selected) {
	uint8_t selected = Index_Of_Selected((const bool**)bools, size);

	for (uint8_t i = 0; i < size; i++) {
		if (i != selected || !require_one_selected) {
			*bools[i] = false;
		}
	}
}
