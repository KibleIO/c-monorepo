#include "REMOTE_ACTOR.h"

void Employ_Frame_Header_Remote_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Remove_Callback_Block_Event(calling_block->On_Unemploy, Unexpected_Block_Event);
	Add_Callback_Block_Event   (calling_block->On_Unemploy, Unemploy_Frame_Remote_Actor);

	FRAME_REMOTE_ACTOR* frame_remote_actor;

	frame_remote_actor               = new FRAME_REMOTE_ACTOR;
	frame_remote_actor->Context_Data = Construct_Data_Block(sizeof(FRAME_REMOTE_ACTOR));

	Capture_Data_Block(frame_remote_actor->Context_Data);
	Write_Captured_Data_Block(frame_remote_actor->Context_Data, (char*)&frame_remote_actor, ((char*)&frame_remote_actor) + sizeof(FRAME_REMOTE_ACTOR));

	Set_Actor_Concurrent_Block(calling_block, (void*)frame_header_local_actor);
}
void Unemploy_Frame_Remote_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	FRAME_REMOTE_ACTOR* frame_remote_actor;

	frame_remote_actor = (FRAME_REMOTE_ACTOR*)Get_Actor_Concurrent_Block(calling_block);
	Release_Data_Block(frame_remote_actor->Context_Data);

	delete frame_remote_actor;
}

// probably don't need these
void Load_Frame_Header_Remote_Actor      (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	FRAME_HEADER_REMOTE_ACTOR* frame_header_remote_actor;
	DATA_BLOCK*                frame_header_data;

	frame_header_remote_actor = (FRAME_HEADER_REMOTE_ACTOR*)Get_Actor_Concurrent_Block(calling_block);
	frame_header_data         = Capture_Latest_Concurrent_Block(calling_block);

	if (frame_header_data == NULL)
	{
		frame_header_local_actor->Mutex.lock();

		frame_header_local_actor->Device_Block = NULL;
		frame_header_local_actor->Buffer_Block = NULL;

		frame_header_local_actor->Mutex.unlock();
	}
	else
	{
		GAIA_LOCAL_ACTOR*   gaia_local_actor;
		FRAME_HEADER_BLOCK* frame_header_block;
		CONCURRENT_BLOCK*   device_block;
		CONCURRENT_BLOCK*   buffer_block;

		Capture_Data_Block(frame_header_data);

		Access_Captured_Data_Block(frame_header_data, (char**)&frame_header_block);

		gaia_local_actor = (GAIA_LOCAL_ACTOR*)Get_Actor_Concurrent_Block(calling_block->Gaia_Block);
		device_block     = Get_Block_Local_Gaia(gaia_local_actor, frame_header_block->Device_Index);
		buffer_block     = Get_Block_Local_Gaia(gaia_local_actor, frame_header_block->Buffer_Index);

		frame_header_local_actor->Mutex.lock();

		frame_header_local_actor->Device_Block = device_block;
		frame_header_local_actor->Buffer_Block = buffer_block;

		frame_header_local_actor->Mutex.unlock();

		Release_Data_Block(frame_header_data);
	}
}
void On_Commit_Frame_Header_Remote_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	FRAME_HEADER_REMOTE_ACTOR* frame_header_remote_actor;

	frame_header_remote_actor = (FRAME_HEADER_REMOTE_ACTOR*)Get_Actor_Concurrent_Block(calling_block);

	Queue_Latest_Block_Event(frame_header_remote_actor->Load_Latest_Event);
}


// Aliases
inline void Frame_Header_Remote_Actor_Employ   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Employ_Frame_Header_Remote_Actor(calling_block, context_data);
}
inline void Frame_Header_Remote_Actor_Unemploy (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Unemploy_Frame_Header_Remote_Actor(calling_block, context_data);
}
