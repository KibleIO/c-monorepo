#ifndef IDLE_ACTOR_H_
#define IDLE_ACTOR_H_

#include "../../CONCURRENT_BLOCK.h"

struct IDLE_LOCAL_ACTOR;

void Employ_Idle_Actor   (CONCURRENT_BLOCK* const gaia_local_block, DATA_BLOCK* const context_data);
void Unemploy_Idle_Actor (CONCURRENT_BLOCK* const gaia_local_block, DATA_BLOCK* const context_data);

#endif // IDLE_ACTOR_H_
