#ifndef AUDIO_DEVICE_REMOTE_ACTOR_H_
#define AUDIO_DEVICE_REMOTE_ACTOR_H_

#include "../../CONCURRENT_BLOCK.h"

struct AUDIO_DEVICE_REMOTE_ACTOR
{
};

void Employ_Audio_Device_Remote_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Audio_Device_Remote_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

BLOCK_REMOTE_EMPLOYMENT_ROSTER[BLOCK_TYPE_AUDIO_DEVICE_REMOTE] = Employ_Audio_Device_Remote_Actor;


// Aliases
inline void Audio_Device_Remote_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
inline void Audio_Device_Remote_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // AUDIO_DEVICE_REMOTE_ACTOR_H_
