#include "Nk_Debugging.h"

void Output_Debug_Message_On_Key_Released(
ostream& out, const struct nk_context* ctx, enum nk_keys keys) {
	Output_Debug_Message_If(out, nk_input_is_key_released(&ctx->input, keys));
}
