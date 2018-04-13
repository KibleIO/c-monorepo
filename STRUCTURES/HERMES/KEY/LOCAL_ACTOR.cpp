#include "LOCAL_ACTOR.h"

/*
struct KEY_LOCAL_ACTOR
{
	GAIA*                  Gaia;
	unsigned long volatile Available_Index;
	unsigned long volatile Available_Capacity;
	unsigned long volatile Replenish_Index;
	unsigned long volatile Replenish_Capacity;
};
*/

void Employ_Key_Local_Actor            (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{

}
void Unemploy_Key_Local_Actor          (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{

}

DATA_BLOCK* Capture_Available_Pad_Key_Local_Actor(KEY_LOCAL_ACTOR* const actor)
{
	const CONCURRENT_BLOCK* const* block_array;
	const DATA_BLOCK*              pad_data;
	unsigned long                  pad_capacity;
	unsigned long                  pad_index;

	pad_data     = NULL;

	actor->Gaia->Mutex.lock();

	if (actor->Available_Capacity == 0)
	{
		actor->Gaia->Mutex.unlock();

		// WARNING - key exhausted!
		return;
	}

	pad_index    = actor->Available_Index;
	block_array  = actor->Gaia->Block_Array;
	pad_capacity = (unsigned long)(actor->Gaia->Block_Terminator - block_array);

	loop:{
		pad_data  = Capture_Latest_Concurrent_Block((CONCURRENT_BLOCK* const)*(block_array + pad_index));
		if (pad_data != NULL)
		{
			Commit_Concurrent_Block((CONCURRENT_BLOCK* const)*(block_array + pad_index), NULL);
			actor->Available_Index = pad_index;
			actor->Available_Capacity--;
		}
		else
		{
			pad_index = (pad_index + 1) % pad_capacity;
			goto loop;
		}
	}

	actor->Gaia->Mutex.unlock();

	return available_pad;
}
void Encrypt_Message_Key_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{

}
void Encrypt_Replenish_Key_Local_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{

}
