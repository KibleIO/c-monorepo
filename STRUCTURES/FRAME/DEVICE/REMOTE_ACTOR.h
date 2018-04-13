#ifndef FRAME_DEVICE_REMOTE_ACTOR_H_
#define FRAME_DEVICE_REMOTE_ACTOR_H_

#include "../../CONCURRENT_BLOCK.h"

struct FRAME_DEVICE_REMOTE_ACTOR
{
};

void Employ_Frame_Device_Remote_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Frame_Device_Remote_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

BLOCK_REMOTE_EMPLOYMENT_ROSTER[BLOCK_TYPE_FRAME_DEVICE_REMOTE] = Employ_Frame_Device_Remote_Actor;


// Aliases
inline void Frame_Device_Remote_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
inline void Frame_Device_Remote_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // FRAME_DEVICE_REMOTE_ACTOR_H_
