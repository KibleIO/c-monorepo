#ifndef KEY_LOCAL_ACTOR_H_
#define KEY_LOCAL_ACTOR_H_

#include "../../GAIA/GAIA.h"

// Encodes local messages with an unused, remotely available pad
// Replenishes used pads with a locally generated pad encrypted with an unused, remotely available pad
// On remote verification of reception of transferred messag, disposes pad
// Generates new pads maintains

struct KEY_LOCAL_ACTOR
{
	GAIA*                  Gaia;
	unsigned long volatile Available_Index;
	unsigned long volatile Available_Capacity;
	unsigned long volatile Replenish_Index;
	unsigned long volatile Replenish_Capacity;
};

void Employ_Key_Local_Actor            (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Key_Local_Actor          (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

void Dispose_Pad_Local_Actor           (KEY_LOCAL_ACTOR* const actor, unsigned long const index);

void Encrypt_Message_Key_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Encrypt_Replenish_Key_Local_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // KEY_LOCAL_ACTOR_H_
