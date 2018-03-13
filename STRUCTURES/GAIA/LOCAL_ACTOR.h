#ifndef GAIA_LOCAL_ACTOR_H_
#define GAIA_LOCAL_ACTOR_H_

#include "GAIA.h"
#include "ROOT/LOCAL_ACTOR.h"

struct GAIA_LOCAL_ACTOR
{
	GAIA*                  Gaia;
	GAIA_ROOT_LOCAL_ACTOR* Root_Actor;
};

void                     Employ_Gaia_Local_Actor            (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void                     Unemploy_Gaia_Local_Actor          (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

CONCURRENT_BLOCK*        Employ_Block_Gaia_Local_Actor      (const GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor, const unsigned long block_index);
CONCURRENT_BLOCK*        Employ_Idle_Block_Gaia_Local_Actor (GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor, const bool force);
inline CONCURRENT_BLOCK* Unemploy_Block_Gaia_Local_Actor    (const GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long block_index);

#endif // GAIA_LOCAL_ACTOR_H_
