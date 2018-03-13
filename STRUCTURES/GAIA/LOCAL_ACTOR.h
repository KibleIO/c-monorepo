#ifndef GAIA_LOCAL_ACTOR_H_
#define GAIA_LOCAL_ACTOR_H_

#include "GAIA.h"

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


// Aliases
inline void              Gaia_Local_Actor_Employ             (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
inline void              Gaia_Local_Actor_Unemploy           (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

inline unsigned long     Get_Block_Capacity_Gaia_Local_Actor (GAIA_LOCAL_ACTOR* const gaia_local_actor);
inline unsigned long     Gaia_Local_Actor_Get_Block_Capacity (GAIA_LOCAL_ACTOR* const gaia_local_actor);
inline void              Set_Block_Capacity_Gaia_Local_Actor (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long new_capacity, const bool allow_shrink);
inline void              Gaia_Local_Actor_Set_Block_Capacity (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long new_capacity, const bool allow_shrink);

inline CONCURRENT_BLOCK* Get_Block_Gaia_Local_Actor          (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long block_index);
inline CONCURRENT_BLOCK* Gaia_Local_Actor_Get_Block          (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long block_index);

inline CONCURRENT_BLOCK* Gaia_Local_Actor_Employ_Block       (const GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor, const unsigned long block_index);
inline CONCURRENT_BLOCK* Gaia_Local_Actor_Employ_Idle_Block  (GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor);
inline CONCURRENT_BLOCK* Gaia_Local_Actor_Unemploy_Block     (const GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long block_index);

#endif // GAIA_LOCAL_ACTOR_H_
