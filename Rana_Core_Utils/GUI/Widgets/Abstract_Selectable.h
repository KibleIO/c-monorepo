#ifndef ABSTRACT_SELECTABLE_H_
#define ABSTRACT_SELECTABLE_H_

#include "../Style/Abstract_Selectable_Style.h"
#include "Utils/Abstract_Button_Utils.h"

struct ABSTRACT_SELECTABLE {
	uint8_t state;
	bool clicked;
	bool selected;
	ABSTRACT_SELECTABLE_STYLE style;
};

void Initialize_ABSTRACT_SELECTABLE(
ABSTRACT_SELECTABLE*, ABSTRACT_SELECTABLE_STYLE);

bool Begin_ABSTRACT_SELECTABLE(
ABSTRACT_SELECTABLE*, struct nk_context*, const char*, nk_flags);
bool Begin_ABSTRACT_SELECTABLE_With_Buffer(
ABSTRACT_SELECTABLE*, struct nk_context*, const char*, nk_flags);
void End_ABSTRACT_SELECTABLE(struct nk_context*);

// ACCESSORS
bool ABSTRACT_SELECTABLE_Clicked(const ABSTRACT_SELECTABLE*);

#endif
