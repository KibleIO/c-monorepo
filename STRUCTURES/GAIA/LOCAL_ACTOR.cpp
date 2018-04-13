#include "LOCAL_ACTOR.h"

void                     Employ_Gaia_Local_Actor            (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	if (calling_block->Gaia_Block != NULL)
	{
		Add_Callback_Block_Event   (calling_block->On_Employ, gaia_local_block->Gaia_Block->On_Employ);
		Remove_Callback_Block_Event(calling_block->On_Employ, Employ_Default_Local_Block);
		Remove_Callback_Block_Event(calling_block->On_Employ, Employ_Default_Remote_Block);
	}
	Add_Callback_Block_Event(calling_block->On_Employ, Employ_Default_Local_Block);

	GAIA_LOCAL_ACTOR* gaia_local_actor;

	gaia_local_actor             = new GAIA_LOCAL_ACTOR;
	gaia_local_actor->Gaia       = Construct_Gaia(calling_block);
	gaia_local_actor->Root_Actor = (GAIA_ROOT_LOCAL_ACTOR*)Get_Actor_Concurrent_Block(gaia_local_actor->Gaia->Root_Block);

	Set_Actor_Concurrent_Block(calling_block, (void*)gaia_local_actor);

	Add_Callback_Block_Event(calling_block->On_Unemploy      , Unemploy_Gaia_Root_Local_Actor);
	Add_Callback_Block_Event(calling_block->On_Add           , Unexpected_Block_Event); // Suppressed by Hermes employment
	Add_Callback_Block_Event(calling_block->On_Remove        , Unexpected_Block_Event); // Suppressed by Hermes employment
	Add_Callback_Block_Event(calling_block->On_Clear         , Unexpected_Block_Event); // Suppressed by Hermes employment
	Add_Callback_Block_Event(calling_block->On_Capture_Idle  , Unexpected_Block_Event); // Suppressed by Hermes employment
	Add_Callback_Block_Event(calling_block->On_Capture_Latest, Unexpected_Block_Event);
	Add_Callback_Block_Event(calling_block->On_Commit        , Unexpected_Block_Event); // Suppressed by Hermes employment
}
void                     Unemploy_Gaia_Local_Actor          (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_LOCAL_ACTOR* gaia_local_actor;

	gaia_local_actor = (GAIA_LOCAL_ACTOR*) Get_Actor_Concurrent_Block(calling_block);

	Deconstruct_Gaia(gaia_local_actor->Gaia);

	delete gaia_local_actor;
}

CONCURRENT_BLOCK*        Employ_Block_Gaia_Local_Actor      (const GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor, const unsigned long block_index)
{
	CONCURRENT_BLOCK* const block = Get_Block_Gaia(gaia_local_actor->Gaia, block_index);

	#ifdef GAIA_BOUNDS_CHECKING
	if (block == NULL)
	{
		return NULL;
	}
	#endif // GAIA_BOUNDS_CHECKING

	Employ_Concurrent_Block(block, block_descriptor);
	Queue_Latest_Event     (gaia_local_actor->Root_Actor->Latest_Block_Employ);

	return block;
}
CONCURRENT_BLOCK*        Employ_Idle_Block_Gaia_Local_Actor (GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor, const bool force)
{
	const CONCURRENT_BLOCK* const* block_iterator;
	const CONCURRENT_BLOCK* const* block_terminator;

	search_goto:

	gaia_local_actor->Gaia->Mutex.lock();

	block_iterator   = gaia_local_actor->Gaia->Block_Array;      // Prevents redundant memory reads from volatile hint
	block_terminator = gaia_local_actor->Gaia->Block_Terminator; // Prevents redundant memory reads from volatile hint

	while (block_iterator < block_terminator)
	{
		if ((*block_iterator)->Descriptor.Type_Flag == BLOCK_TYPE_IDLE)
		{
			gaia_local_actor->Gaia->Mutex.unlock();

			Employ_Concurrent_Block((CONCURRENT_BLOCK*)*block_iterator, block_descriptor);
			Queue_Latest_Event(gaia_local_actor->Root_Actor->Latest_Block_Employ);

			return (CONCURRENT_BLOCK*)*block_iterator;
		}
		block_iterator++;
	}

	gaia_local_actor->Gaia->Mutex.unlock();

	if (force)
	{
		Set_Block_Capacity_Gaia(gaia_local_actor->Gaia, (unsigned long)(block_terminator - block_iterator) + 0xFF, false);
		goto search_goto;
	}

	return NULL;
}
inline CONCURRENT_BLOCK* Unemploy_Block_Gaia_Local_Actor    (const GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long block_index)
{
	return Employ_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_IDLE_BLOCK_DESCRIPTOR, block_index);
}
