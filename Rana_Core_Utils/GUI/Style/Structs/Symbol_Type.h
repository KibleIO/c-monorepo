#ifndef SYMBOL_TYPE_H_
#define SYMBOL_TYPE_H_

#include "../../NK_BASE.h"

struct SYMBOL_TYPE_TRIO {
	enum nk_symbol_type normal;
	enum nk_symbol_type hover;
	enum nk_symbol_type active;
};

SYMBOL_TYPE_TRIO Symbol_Type_Trio_Uniform(enum nk_symbol_type);
SYMBOL_TYPE_TRIO Symbol_Type_Trio(
enum nk_symbol_type, enum nk_symbol_type, enum nk_symbol_type);

#endif
