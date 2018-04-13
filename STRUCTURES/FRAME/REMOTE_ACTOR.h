#ifndef FRAME_REMOTE_ACTOR_H_
#define FRAME_REMOTE_ACTOR_H_

#include "../../CONCURRENT_BLOCK.h"

struct FRAME_REMOTE_ACTOR
{
	const DATA_BLOCK*       Context_Data;
	const CONCURRENT_BLOCK* Source_Block;

	const GAIA_LOCAL_ACTOR* Gaia_Actor;
	const CONCURRENT_BLOCK* Device_Block;
	const CONCURRENT_BLOCK* Buffer_Block;
	mutex                   Mutex;
};

void Employ_Frame_Header_Remote_Actor    (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Frame_Header_Remote_Actor  (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

void Load_Frame_Header_Remote_Actor      (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void On_Commit_Frame_Header_Remote_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

// Aliases
inline void Frame_Header_Remote_Actor_Employ   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
inline void Frame_Header_Remote_Actor_Unemploy (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // FRAME_REMOTE_ACTOR_H_
