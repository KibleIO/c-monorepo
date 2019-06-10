#ifndef CHECKBOX_RADIO_PACKAGE_H_
#define CHECKBOX_RADIO_PACKAGE_H_

#include "Checkbox.h"
#include "../../Utilities/vararg_ext.h"

#define CHECKBOX_RADIO_PACKAGE_DEFAULT_CAPACITY 8

struct CHECKBOX_RADIO_PACKAGE {
	CHECKBOX* checkboxes;
	uint8_t totalCheckboxes;
	uint8_t checkboxCapacity;

	// If true, one checkbox needs to be checked at all times
	bool requireOneSelected;
};

void Initialize_Checkbox_Radio_Package(
CHECKBOX_RADIO_PACKAGE*, struct nk_style_toggle, uint8_t total_checkboxes,
bool require_one_selected = false);

bool Render_Checkbox(
CHECKBOX_RADIO_PACKAGE*, struct nk_context*, const char*, uint8_t index);
bool Render_Checkbox_With_Buffer(
CHECKBOX_RADIO_PACKAGE*, struct nk_context*, const char*, uint8_t index);

void Delete_Checkbox_Radio_Package(CHECKBOX_RADIO_PACKAGE*);

void Select_Checkbox(CHECKBOX_RADIO_PACKAGE*, uint8_t index);

// ACCESSORS
bool Checkbox_State(const CHECKBOX_RADIO_PACKAGE*, uint8_t index);
int8_t Toggled_Checkbox_Index(const CHECKBOX_RADIO_PACKAGE*);

// HELPERS
void Update_Checkbox_States(CHECKBOX_RADIO_PACKAGE*, uint8_t selected_index);
void Enforce_At_Least_One_Selected(
CHECKBOX_RADIO_PACKAGE*, uint8_t prev_selected);

#endif
