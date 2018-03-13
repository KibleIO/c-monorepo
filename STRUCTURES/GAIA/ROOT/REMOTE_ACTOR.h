#ifndef GAIA_ROOT_REMOTE_ACTOR_H_
#define GAIA_ROOT_REMOTE_ACTOR_H_

#include "DATA_BLOCK.h"
#include "REMOTE_ACTOR.h"

struct GAIA_ROOT_REMOTE_ACTOR
{
	LATEST_BLOCK_EVENT* Latest_Commit;
};

void Employ_Gaia_Root_Remote_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Gaia_Root_Remote_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

void Load_Gaia_Root_Remote_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Commit_Gaia_Root_Remote_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);


// Aliases
inline void Gaia_Root_Remote_Actor_Employ   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
inline void Gaia_Root_Remote_Actor_Unemploy (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

inline void Gaia_Root_Remote_Actor_Load     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
inline void Gaia_Root_Remote_Actor_Commit   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // GAIA_ROOT_REMOTE_ACTOR_H_
