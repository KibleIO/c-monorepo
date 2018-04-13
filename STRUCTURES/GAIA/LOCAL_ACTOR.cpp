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

	Remove_Callback_Block_Event(calling_block->On_Unemploy, Unexpected_Block_Event);
	Add_Callback_Block_Event(calling_block->On_Unemploy, Unemploy_Gaia_Root_Local_Actor);
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


// Aliases
inline void              Gaia_Local_Actor_Employ             (DATA_BLOCK* const block_passer)
{
	Employ_Gaia_Local_Actor(block_passer);
}
inline void              Gaia_Local_Actor_Unemploy           (DATA_BLOCK* const block_passer)
{
	Unemploy_Gaia_Local_Actor(block_passer);
}

inline unsigned long     Get_Block_Capacity_Gaia_Local_Actor (GAIA_LOCAL_ACTOR* const gaia_local_actor)
{
	return Get_Block_Capacity_Gaia(gaia_local_actor->Gaia);
}
inline unsigned long     Gaia_Local_Actor_Get_Block_Capacity (GAIA_LOCAL_ACTOR* const gaia_local_actor)
{
	return Get_Block_Capacity_Gaia(gaia_local_actor->Gaia);
}
inline void              Set_Block_Capacity_Gaia_Local_Actor (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long new_capacity, const bool allow_shrink)
{
	Set_Block_Capacity_Gaia(gaia_local_actor->Gaia, new_capacity, allow_shrink);
	Queue_Latest_Event(gaia_local_actor->Latest_Block_Employ);
}
inline void              Gaia_Local_Actor_Set_Block_Capacity (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long new_capacity, const bool allow_shrink)
{
	Set_Block_Capacity_Gaia(gaia_local_actor->Gaia, new_capacity, allow_shrink);
	Queue_Latest_Event(gaia_local_actor->Latest_Block_Employ);
}

inline CONCURRENT_BLOCK* Get_Block_Gaia_Local_Actor          (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long block_index)
{
	return Get_Block_Gaia(gaia_local_actor->Gaia, block_index);
}
inline CONCURRENT_BLOCK* Gaia_Local_Actor_Get_Block          (GAIA_LOCAL_ACTOR* const gaia_local_actor, const unsigned long block_index)
{
	return Get_Block_Gaia(gaia_local_actor->Gaia, block_index);
}

inline CONCURRENT_BLOCK* Gaia_Local_Actor_Employ_Block       (const GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor, const unsigned long block_index)
{
	return Employ_Block_Gaia_Local_Actor(gaia_local_actor, block_descriptor, block_index);
}
inline CONCURRENT_BLOCK* Gaia_Local_Actor_Employ_Idle_Block  (GAIA_LOCAL_ACTOR* const gaia_local_actor, const BLOCK_DESCRIPTOR* const block_descriptor)
{
	return Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, block_index);
}
inline CONCURRENT_BLOCK* Gaia_Local_Actor_Unemploy_Block     (const GAIA_LOCAL_ACTOR* const gaia_local, const unsigned long block_index)
{
	return Employ_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_IDLE_BLOCK_DESCRIPTOR, block_index);
}
