#ifndef GAIA_REMOTE_ACTOR_H_
#define GAIA_REMOTE_ACTOR_H_

#include "GAIA.h"

struct GAIA_REMOTE_ACTOR
{
	GAIA*                   Gaia;
	GAIA_COMMAND_CONTEXT*   Command_Context;
	SEQUENTIAL_BLOCK_EVENT* Sequential_Commands;
};

void Employ_Gaia_Remote_Actor    (CONCURRENT_BLOCK* const gaia_remote_block, DATA_BLOCK* const context_data);
void Unemploy_Gaia_Remote_Actor  (CONCURRENT_BLOCK* const gaia_remote_block, DATA_BLOCK* const context_data);


// Aliases
inline void              Gaia_Remote_Actor_Employ             (CONCURRENT_BLOCK* const gaia_remote_block, DATA_BLOCK* const context_data);
inline void              Gaia_Remote_Actor_Unemploy           (CONCURRENT_BLOCK* const gaia_remote_block, DATA_BLOCK* const context_data);

inline unsigned long     Get_Block_Capacity_Gaia_Remote_Actor (GAIA_REMOTE_ACTOR* const GAIA_REMOTE_ACTOR);
inline unsigned long     Gaia_Remote_Actor_Get_Block_Capacity (GAIA_REMOTE_ACTOR* const gaia_remote_actor);
inline void              Set_Block_Capacity_Gaia_Remote_Actor (GAIA_REMOTE_ACTOR* const GAIA_REMOTE_ACTOR, const unsigned long new_capacity);
inline void              Gaia_Remote_Actor_Set_Block_Capacity (GAIA_REMOTE_ACTOR* const GAIA_REMOTE_ACTOR, const unsigned long new_capacity);

inline CONCURRENT_BLOCK* Get_Block_Gaia_Remote_Actor          (GAIA_REMOTE_ACTOR* const GAIA_REMOTE_ACTOR, const unsigned long block_index);
inline CONCURRENT_BLOCK* Gaia_Remote_Actor_Get_Block          (GAIA_REMOTE_ACTOR* const GAIA_REMOTE_ACTOR, const unsigned long block_index);

#endif // GAIA_REMOTE_ACTOR_H_
