#ifndef FRAME_DEVICE_lOCAL_ACTOR_H_
#define FRAME_DEVICE_lOCAL_ACTOR_H_

#include "../../CONCURRENT_BLOCK.h"

struct FRAME_BUFFER_LOCAL_ACTOR
{

};

void Employ_Frame_Buffer_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Frame_Buffer_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

BLOCK_LOCAL_EMPLOYMENT_ROSTER[BLOCK_TYPE_FRAME_BUFFER_LOCAL] = Employ_Frame_Buffer_Local_Actor;


// Aliases
inline void Frame_Buffer_Local_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
inline void Frame_Buffer_Local_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // FRAME_DEVICE_lOCAL_ACTOR_H_
