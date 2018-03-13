#include "REMOTE_ACTOR.h"

// decrypts packet to remote_gaia command queue which decodes and applies it (see Decrypt_Hermes_Header_Actor)
void Decrypt_Hermes_Header_Actor  (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const encrypted_data);
