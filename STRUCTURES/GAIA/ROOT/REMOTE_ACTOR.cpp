#include "REMOTE_ACTOR.h"

void Employ_Gaia_Root_Remote_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_ROOT_REMOTE_ACTOR* gaia_root_remote_actor;

	gaia_root_remote_actor                = new GAIA_ROOT_REMOTE_ACTOR;
	gaia_root_remote_actor->Latest_Commit = Construct_Latest_Block_Event(Load_Gaia_Root_Remote_Actor, calling_block);

	Set_Actor_Concurrent_Block(calling_block, (void*)gaia_root_remote_actor);

	Add_Callback_Block_Event(calling_block->On_Unemploy    , Unemploy_Gaia_Root_Remote_Actor);
	Add_Callback_Block_Event(calling_block->On_Add         , Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Remove      , Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Clear       , Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Capture_Idle, Unexpected_Block_Event); // Commit only
	Add_Callback_Block_Event(calling_block->On_Commit      , Commit_Gaia_Root_Remote_Actor);
}
void Unemploy_Gaia_Root_Remote_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Remove_Callback_Block_Event(calling_block->On_Commit, On_Commit_Gaia_Remote_Actor);

	GAIA_ROOT_REMOTE_ACTOR* gaia_root_remote_actor;

	gaia_root_remote_actor = (GAIA_ROOT_REMOTE_ACTOR*) Get_Actor_Concurrent_Block(calling_block);

	Deconstruct_Latest_Block_Event(gaia_root_remote_actor->Latest_Commit);
	delete gaia_root_remote_actor;
}

void Load_Gaia_Root_Remote_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	DATA_BLOCK*        commited_root_data;
	GAIA_REMOTE_ACTOR* gaia_remote_actor;
	BLOCK_DESCRIPTOR*  descriptor_iterator;
	BLOCK_DESCRIPTOR*  descriptor_terminator;
	unsigned long      descriptor_capacity;
	CONCURRENT_BLOCK*  block_iterator;
	CONCURRENT_BLOCK*  block_terminator;
	unsigned long      block_capacity;
	thread**           thread_pool;
	thread**           thread_iterator;
	thread**           thread_terminator;

	commited_root_data = Capture_Latest_Concurrent_Block(calling_block);
	if (commited_root_data == NULL)
	{
		// Warning
		return;
	}
	descriptor_terminator = (BLOCK_DESCRIPTOR* )Access_Captured_Data_Block(commited_root_data, (char**)&descriptor_iterator);
	gaia_remote_actor     = (GAIA_REMOTE_ACTOR*)Get_Actor_Concurrent_Block(calling_block->Gaia_Block);

	// Ensure Gaia capacity can recieve commited block definition
	descriptor_capacity = (unsigned long)(descriptor_terminator - descriptor_iterator);
	if (descriptor_capacity > Get_Block_Capacity_Gaia_Remote_Actor(gaia_remote_actor))
	{
		Set_Block_Capacity_Gaia_Remote_Actor(gaia_remote_actor, descriptor_capacity, false);
	}

	gaia_remote_actor->Gaia->Mutex.lock();

	block_iterator   = gaia_remote_actor->Gaia->Block_Array;      // Prevents redundant memory access from volatile hint
	block_terminator = gaia_remote_actor->Gaia->Block_Terminator; // Prevents redundant memory access from volatile hint
	block_capacity   = (unsigned long)(block_terminator - block_iterator);

	if (descriptor_capacity >= block_capacity)
	{
		thread_pool = new thread*[descriptor_capacity];
	}
	else // (descriptor_capacity < block_capacity)
	{
		thread_pool = new thread*[block_capacity];
	}
	thread_iterator = thread_pool;

	while (descriptor_iterator < descriptor_terminator)
	{
		*thread_iterator = new thread(Employ_Concurrent_Block, block_iterator, descriptor_iterator);
		thread_iterator++;
		block_iterator++;
		descriptor_iterator++;
	}
	while (block_iterator < block_terminator)
	{
		*thread_iterator = new thread(Employ_Concurrent_Block, block_iterator, DEFAULT_IDLE_BLOCK_DESCRIPTOR);
		thread_iterator++;
		block_iterator++;
	}

	gaia_remote_actor->Gaia->Mutex.unlock();

	// Clean up threads
	thread_terminator = thread_iterator;
	thread_iterator   = thread_pool;

	while (thread_iterator < thread_terminator)
	{
		(*thread_iterator)->join();
		delete *thread_iterator;
		thread_iterator++;
	}
	delete [] thread_pool;

	Release_Data_Block(commited_gaia_data);
}
void Commit_Gaia_Root_Remote_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_ROOT_REMOTE_ACTOR* gaia_root_remote_actor;

	gaia_root_remote_actor = (GAIA_ROOT_REMOTE_ACTOR*) Get_Actor_Concurrent_Block(calling_block);

	Queue_Latest_Block_Event(gaia_root_remote_actor->Latest_Commit);
}
