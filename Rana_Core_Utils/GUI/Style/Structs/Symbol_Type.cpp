#include "Symbol_Type.h"

SYMBOL_TYPE_TRIO Symbol_Type_Trio_Uniform(enum nk_symbol_type symbol) {
	return Symbol_Type_Trio(symbol, symbol, symbol);
}
SYMBOL_TYPE_TRIO Symbol_Type_Trio(
enum nk_symbol_type normal, enum nk_symbol_type hover,
enum nk_symbol_type active) {
	SYMBOL_TYPE_TRIO trio;
	trio.normal = normal;
	trio.hover = hover;
	trio.active = active;
	return trio;
}
