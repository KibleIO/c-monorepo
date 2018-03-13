#include "ACTOR.h"

void Employ_Idle_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	IDLE_LOCAL_ACTOR* idle_local_actor;

	idle_local_actor = new IDLE_LOCAL_ACTOR;

	Set_Actor_Concurrent_Block(calling_block, (void*)idle_local_actor);

	Add_Callback_Block_Event(calling_block->On_Unemploy      , Unemploy_Local_Idle_Actor);
	Add_Callback_Block_Event(calling_block->On_Add           , Unexpected_Block_Event);
	Add_Callback_Block_Event(calling_block->On_Remove        , Unexpected_Block_Event);
	Add_Callback_Block_Event(calling_block->On_Clear         , Unexpected_Block_Event);
	Add_Callback_Block_Event(calling_block->On_Capture_Idle  , Unexpected_Block_Event);
	Add_Callback_Block_Event(calling_block->On_Capture_Latest, Unexpected_Block_Event);
	Add_Callback_Block_Event(calling_block->On_Commit        , Unexpected_Block_Event);
}
void Unemploy_Idle_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	IDLE_LOCAL_ACTOR* idle_local_actor;

	idle_local_actor = (IDLE_LOCAL_ACTOR*) Get_Actor_Concurrent_Block(calling_block);

	delete idle_local_actor;
}
