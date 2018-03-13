#ifndef HERMES_DEVICE_LOCAL_ACTOR_H_
#define HERMES_DEVICE_LOCAL_ACTOR_H_

#include "../../CONCURRENT_BLOCK.h"

struct HERMES_DEVICE_LOCAL_ACTOR
{
};

void Employ_Hermes_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Hermes_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

// Aliases
inline void Hermes_Local_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
inline void Hermes_Local_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // HERMES_DEVICE_LOCAL_ACTOR_H_
