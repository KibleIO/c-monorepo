#ifndef HERMES_REMOTE_ACTOR_H_
#define HERMES_REMOTE_ACTOR_H_

#include "REMOTE_ACTOR.h"

// EMPLOYMENT, ACTOR, NOR CONTEXT_DATA NEEDED

// decrypts packet to remote_gaia command queue which decodes and applies it (see Decrypt_Hermes_Header_Actor)
void Decrypt_Hermes_Header_Actor  (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const encrypted_data);

#endif // HERMES_REMOTE_ACTOR_H_
