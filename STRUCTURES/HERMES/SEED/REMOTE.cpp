#include "REMOTE.h"

void Employ_Hermes_Seed_Remote     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	LATEST_BLOCK_EVENT* latest_block_event;
	DATA_BLOCK*         add_context;

	add_context = Construct_Data_Block(sizeof(LATEST_BLOCK_EVENT*)); // Deconstructed by Unemploy event
	Capture_Data_Block(add_context);

	latest_block_event = Construct_Latest_Block_Event(Load_Hermes_Seed_Remote, add_context);
	Write_Captured_Data_Block(add_context, (char*)&latest_block_event, ((char*)&latest_block_event) + sizeof(LATEST_BLOCK_EVENT*));

	Remove_Callback_Block_Event(calling_block->On_Add     , Unexpected_Block_Event);
	Remove_Callback_Block_Event(calling_block->On_Unemploy, Unexpected_Block_Event);

	Add_Callback_Block_Event(calling_block->On_Add     , Queue_Load_Hermes_Seed_Remote, add_context);
	Add_Callback_Block_Event(calling_block->On_Unemploy, Unemploy_Hermes_Seed_Remote,   add_context);
}
void Unemploy_Hermes_Seed_Remote   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Remove_Callback_Block_Event(calling_block->On_Add, Queue_Load_Hermes_Seed_Remote);

	LATEST_BLOCK_EVENT* latest_block_event;
	Access_Captured_Data_Block(context_data, (char**)&latest_block_event);
	Deconstruct_Latest_Block_Event(latest_block_event);
}

void Queue_Load_Hermes_Seed_Remote (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	LATEST_BLOCK_EVENT* latest_block_event;
	Access_Captured_Data_Block(context_data, (char**)&latest_block_event);
	Queue_Latest_Block_Event(latest_block_event);
}
void Load_Hermes_Seed_Remote       (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	DATA_BLOCK*        seed_data;
	HERMES_SEED_BLOCK* seed_block;

	loop:
	{
		seed_data = Capture_Node_Concurrent_Block(calling_block);

		if (seed_data != NULL)
		{
			Access_Captured_Data_Block(seed_data, (char**)&seed_block);
			Initialize_Hermes         (calling_block->Gaia_Block, seed_block);

			Remove_Concurrent_Block(calling_block, seed_data);
			Erase_Data_Block       (seed_data);
			Release_Data_Block     (seed_data);

			goto loop;
		}
	}
}
