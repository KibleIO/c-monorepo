#ifndef GAIA_REMOTE_ACTOR_H_
#define GAIA_REMOTE_ACTOR_H_

#include <Gaia/BLOCK_EVENTS/SEQUENTIAL_BLOCK_EVENT.h>
#include "GAIA.h"

struct GAIA_REMOTE_ACTOR
{
	GAIA*                   Gaia;
	SEQUENTIAL_BLOCK_EVENT* Sequential_Commands;
};

void Employ_Gaia_Remote_Actor    (CONCURRENT_BLOCK* const gaia_remote_block, DATA_BLOCK* const context_data);
void Unemploy_Gaia_Remote_Actor  (CONCURRENT_BLOCK* const gaia_remote_block, DATA_BLOCK* const context_data);

#endif // GAIA_REMOTE_ACTOR_H_
