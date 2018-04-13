#include "LOCAL_ACTOR.h"

void Employ_Gaia_Root_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_ROOT_LOCAL_ACTOR* gaia_root_local_actor;

	gaia_root_local_actor                = new GAIA_ROOT_LOCAL_ACTOR;
	gaia_root_local_actor->Latest_Employ = Construct_Latest_Block_Event(Load_Gaia_Root_Local_Actor, calling_block);

	Set_Actor_Concurrent_Block(calling_block, (void*)gaia_root_LOCAL_actor);

	Add_Callback_Block_Event(calling_block->On_Unemploy      , Unemploy_Gaia_Root_Local_Actor);
	Add_Callback_Block_Event(calling_block->On_Add           , Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Remove        , Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Clear         , Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Capture_Idle  , Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Capture_Latest, Unexpected_Block_Event); // Suppressed by Hermes employment
}
void Unemploy_Gaia_Root_Local_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_ROOT_LOCAL_ACTOR* gaia_root_local_actor;

	gaia_root_local_actor = (GAIA_ROOT_LOCAL_ACTOR*) Get_Actor_Concurrent_Block(calling_block);

	Deconstruct_Latest_Block_Event(gaia_root_local_actor->Latest_Employ);
	delete gaia_root_local_actor;
}

void Load_Gaia_Root_Local_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_LOCAL_ACTOR*              gaia_local_actor;
	const CONCURRENT_BLOCK* const* block_iterator;
	const CONCURRENT_BLOCK* const* block_terminator;
	DATA_BLOCK*                    write_block;
	BLOCK_DESCRIPTOR*              descriptor_iterator;

	// Read GAIA_LOCAL_ACTOR* from CONCURRENT_BLOCK
	gaia_local_actor = (GAIA_LOCAL_ACTOR*)Get_Actor_Concurrent_Block(calling_block->Gaia_Block);

	gaia_local_actor->Gaia->Mutex.lock();

	block_iterator   = gaia_local_actor->Gaia->Block_Array;      // Prevents redundant memory access from volatile hint
	block_terminator = gaia_local_actor->Gaia->Block_Terminator; // Prevents redundant memory access from volatile hint
	write_block      = Construct_Data_Block((block_terminator - block_iterator) * sizeof(BLOCK_DESCRIPTOR));

	Access_Captured_Data_Block(write_block, &descriptor_iterator);

	while (block_iterator < block_terminator)
	{
		*descriptor_iterator = Get_Descriptor_Concurrent_Block(*block_iterator);
		descriptor_iterator++;
		block_iterator++;
	}

	gaia_local_actor->Gaia->Mutex.unlock();

	Commit_Concurrent_Block (gaia_block, write_block);
}
