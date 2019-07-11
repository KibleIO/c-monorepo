#include "Checkbox_Radio_Package.h"

void Initialize_Checkbox_Radio_Package(
CHECKBOX_RADIO_PACKAGE* package, struct nk_style_toggle style,
uint8_t totalCheckboxes, bool require_one_selected) {
	if (totalCheckboxes > 0) {
		package->checkboxes = new CHECKBOX[totalCheckboxes];
		package->checkboxCapacity = totalCheckboxes;
	}
	else {
		package->checkboxes =
		new CHECKBOX[CHECKBOX_RADIO_PACKAGE_DEFAULT_CAPACITY];
		package->checkboxCapacity = CHECKBOX_RADIO_PACKAGE_DEFAULT_CAPACITY;
	}

	for (uint8_t i = 0; i < totalCheckboxes; i++) {
		Initialize_Checkbox(&package->checkboxes[i], style);
	}

	package->totalCheckboxes = totalCheckboxes;
	package->requireOneSelected = require_one_selected;

	// If the package always needs one selected,
	// select the first box immediately
	if (require_one_selected) {
		Select_Checkbox(package, 0);
	}
}

bool Render_Checkbox(
CHECKBOX_RADIO_PACKAGE* package, struct nk_context* ctx, const char* label,
uint8_t index) {
	if (index < package->totalCheckboxes) {
		if (Render_Checkbox(&package->checkboxes[index], ctx, label)) {
			Update_Checkbox_States(package, index);
			Enforce_At_Least_One_Selected(package, index);
			return true;
		}
	}
	return false;
}

bool Render_Checkbox_With_Buffer(
CHECKBOX_RADIO_PACKAGE* package, struct nk_context* ctx, const char* label,
uint8_t index) {
	nk_label(ctx, "", 0);
	return Render_Checkbox(package, ctx, label, index);
}

void Delete_Checkbox_Radio_Package(CHECKBOX_RADIO_PACKAGE* package) {
	for (uint8_t i = 0; i < package->totalCheckboxes; i++) {
		Delete_Checkbox(&package->checkboxes[i]);
	}
	delete [] package->checkboxes;
}

void Select_Checkbox(CHECKBOX_RADIO_PACKAGE* package, uint8_t index) {
	if (index < package->totalCheckboxes) {
		Set_Checkbox_State(&package->checkboxes[index], true);
		Update_Checkbox_States(package, index);
		Enforce_At_Least_One_Selected(package, index);
	}
}

bool Checkbox_State(const CHECKBOX_RADIO_PACKAGE* package, uint8_t index) {
	if (index < package->totalCheckboxes) {
		return Checkbox_State(&package->checkboxes[index]);
	}
	else {
		return false;
	}
}

int8_t Toggled_Checkbox_Index(const CHECKBOX_RADIO_PACKAGE* package) {
	for (uint8_t i = 0; i < package->totalCheckboxes; i++) {
		if (Checkbox_State(&package->checkboxes[i])) {
			return i;
		}
	}
	return -1;
}

void Update_Checkbox_States(
CHECKBOX_RADIO_PACKAGE* package, uint8_t selected_index) {
	for (uint8_t i = 0; i < package->totalCheckboxes; i++) {
		if (i != selected_index) {
			Set_Checkbox_State(&package->checkboxes[i], false);
		}
	}
}

void Enforce_At_Least_One_Selected(
CHECKBOX_RADIO_PACKAGE* package, uint8_t prev_selected) {
	if (package->requireOneSelected) {
		int8_t toggled_index = Toggled_Checkbox_Index(package);
		if (toggled_index == -1) {
			Set_Checkbox_State(&package->checkboxes[prev_selected], true);
		}
	}
}
