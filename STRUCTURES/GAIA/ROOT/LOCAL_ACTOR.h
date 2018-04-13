#ifndef GAIA_ROOT_LOCAL_ACTOR_H_
#define GAIA_ROOT_LOCAL_ACTOR_H_

#include "DATA_BLOCK.h"
#include "LOCAL_ACTOR.h"

struct GAIA_ROOT_LOCAL_ACTOR
{
	LATEST_BLOCK_EVENT* Latest_Employ;
};

void Employ_Gaia_Root_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Gaia_Root_Local_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

void Load_Gaia_Root_Local_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);


// Aliases
inline void Gaia_Root_Local_Actor_Employ   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
inline void Gaia_Root_Local_Actor_Unemploy (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

inline void Gaia_Root_Local_Actor_Load     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // GAIA_ROOT_LOCAL_ACTOR_H_
