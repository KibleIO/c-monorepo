#ifndef ABSTRACT_SELECTABLE_RADIO_H_
#define ABSTRACT_SELECTABLE_RADIO_H_

#include "Abstract_Selectable.h"
#include "Utils/Radio_Package_Utils.h"
#include "../../Utilities/vararg_ext.h"

#define ABSTRACT_SELECTABLE_RADIO_MAX_SELECTABLES 32

struct ABSTRACT_SELECTABLE_RADIO {
	ABSTRACT_SELECTABLE selectables[ABSTRACT_SELECTABLE_RADIO_MAX_SELECTABLES];
	uint8_t size;
	bool requireOneSelected;

	int16_t rendering;	// Selectable last rendered
};

void Initialize_ABSTRACT_SELECTABLE_RADIO_Uniform(
ABSTRACT_SELECTABLE_RADIO*, bool require_one_selected, uint8_t size,
ABSTRACT_SELECTABLE);
void Initialize_ABSTRACT_SELECTABLE_RADIO_Vararg(
ABSTRACT_SELECTABLE_RADIO*, bool require_one_selected, uint8_t size,
/*ABSTRACT_SELECTABLE*/...);
void Initialize_ABSTRACT_SELECTABLE_RADIO_Array(
ABSTRACT_SELECTABLE_RADIO*, bool require_one_selected, uint8_t size,
ABSTRACT_SELECTABLE[]);

bool Begin_Selectable_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO*, struct nk_context*, const char*, nk_flags,
uint8_t index);
bool Begin_Selectable_ABSTRACT_SELECTABLE_RADIO_With_Buffer(
ABSTRACT_SELECTABLE_RADIO*, struct nk_context*, const char*, nk_flags,
uint8_t index);
void End_Selectable_ABSTRACT_SELECTABLE_RADIO(struct nk_context*);

// ACCESSORS
int16_t Index_Of_Selected(ABSTRACT_SELECTABLE_RADIO*);
bool Selectable_Clicked_ABSTRACT_SELECTABLE_RADIO(
const ABSTRACT_SELECTABLE_RADIO*);
const ABSTRACT_SELECTABLE* Selectable_Rendering_ABSTRACT_SELECTABLE_RADIO(
const ABSTRACT_SELECTABLE_RADIO*);

// HELPERS
void Update_Selected_ABSTRACT_SELECTABLE_RADIO(ABSTRACT_SELECTABLE_RADIO*);
void Select_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO*, uint8_t index);
void Deselect_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO*, uint8_t index);
bool** Selectable_State_Refs_ABSTRACT_SELECTABLE_RADIO(
ABSTRACT_SELECTABLE_RADIO*);

#endif
