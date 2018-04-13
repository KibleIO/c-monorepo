#ifndef FRAME_LOCAL_ACTOR_H_
#define FRAME_LOCAL_ACTOR_H_

#include "../../CONCURRENT_BLOCK.h"

struct FRAME_HEADER_LOCAL_ACTOR
{
	const DATA_BLOCK*       Context_Data;
	const CONCURRENT_BLOCK* Gaia_Block;
	const GAIA_LOCAL_ACTOR* Gaia_Actor;

	const CONCURRENT_BLOCK* Device_Block;
	const CONCURRENT_BLOCK* Buffer_Block;
	mutex                   Mutex;
};

void Employ_Frame_Local_Actor          (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Frame_Local_Actor        (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

// Handle Device Events
void Device_Unemploy_Frame_Local_Actor (CONCURRENT_BLOCK* const device_block, DATA_BLOCK* const context_data);
void Device_Commit_Frame_Local_Actor   (CONCURRENT_BLOCK* const device_block, DATA_BLOCK* const context_data);

// Handle Buffer Events
void Buffer_Unemploy_Frame_Local_Actor (CONCURRENT_BLOCK* const buffer_block, DATA_BLOCK* const context_data);


// Aliases
inline void Frame_Header_Local_Actor_Employ   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
inline void Frame_Header_Local_Actor_Unemploy (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // FRAME_LOCAL_ACTOR_H_
