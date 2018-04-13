#ifndef AUDIO_DEVICE_lOCAL_ACTOR_H_
#define AUDIO_DEVICE_lOCAL_ACTOR_H_

#include "../../../CONCURRENT_BLOCK.h"

struct AUDIO_DEVICE_LOCAL_ACTOR
{
};

void Employ_Audio_Device_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Audio_Device_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

BLOCK_LOCAL_EMPLOYMENT_ROSTER[BLOCK_TYPE_AUDIO_DEVICE_LOCAL] = Employ_Audio_Device_Local_Actor;


// Aliases
inline void Audio_Device_Local_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
inline void Audio_Device_Local_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // AUDIO_DEVICE_lOCAL_ACTOR_H_
