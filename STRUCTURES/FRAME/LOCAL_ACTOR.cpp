#include "LOCAL_ACTOR.h"

void Employ_Frame_Local_Actor          (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_LOCAL_ACTOR*  gaia_local_actor;
	CONCURRENT_BLOCK*  frame_gaia_block;
	GAIA_LOCAL_ACTOR*  frame_gaia_actor;
	FRAME_LOCAL_ACTOR* frame_local_actor;

	gaia_local_actor                = (GAIA_LOCAL_ACTOR*)Get_Actor_Concurrent_Block(calling_block->Gaia_Block);
	frame_gaia_block                = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_GAIA_LOCAL_BLOCK_DESCRIPTOR, true);
	frame_gaia_actor                = (GAIA_LOCAL_ACTOR*)Get_Actor_Concurrent_Block(frame_gaia_block);
	frame_local_actor               = new FRAME_LOCAL_ACTOR;

	frame_local_actor->Context_Data = Construct_Data_Block(sizeof(FRAME_LOCAL_ACTOR*));
	frame_local_actor->Gaia_Block   = calling_block;
	frame_local_actor->Gaia_Actor   = frame_gaia_actor;
	frame_local_actor->Device_Block = Employ_Idle_Block_Gaia_Local_Actor(frame_gaia_actor, DEFAULT_FRAME_HEADER_BLOCK_DESCRIPTOR, true);
	frame_local_actor->Buffer_Block = Employ_Idle_Block_Gaia_Local_Actor(frame_gaia_actor, DEFAULT_FRAME_BUFFER_BLOCK_DESCRIPTOR, true);

	Capture_Data_Block(frame_local_actor->Context_Data);
	Write_Captured_Data_Block(frame_local_actor->Context_Data, (char*)&frame_local_actor, (char*)&frame_local_actor) + sizeof(FRAME_LOCAL_ACTOR*));

	Set_Actor_Concurrent_Block(employ_block, (void*)frame_local_actor);

	// Source Block
	Remove_Callback_Block_Event(frame_local_actor->Source_Block->On_Unemploy, Unexpected_Block_Event);
	Add_Callback_Block_Event   (frame_local_actor->Source_Block->On_Unemploy, Unemploy_Frame_Header_Local_Actor, frame_local_actor->Context_Data);

	// Device Block
	Remove_Callback_Block_Event(frame_local_actor->Device_Block->On_Unemploy, Unexpected_Block_Event                                            );
	Add_Callback_Block_Event   (frame_local_actor->Device_Block->On_Unemploy, Device_Unemploy_Frame_Local_Actor, frame_local_actor->Context_Data);
	Remove_Callback_Block_Event(frame_local_actor->Device_Block->On_Commit  , Unexpected_Block_Event                                            );
	Add_Callback_Block_Event   (frame_local_actor->Device_Block->On_Commit  , Device_Commit_Frame_Local_Actor  , frame_local_actor->Context_Data);

	// Buffer Block
	Remove_Callback_Block_Event(frame_local_actor->Buffer_Block->On_Unemploy, Unexpected_Block_Event);
	Add_Callback_Block_Event   (frame_local_actor->Buffer_Block->On_Unemploy, Buffer_Unemploy_Frame_Local_Actor);
}
void Unemploy_Frame_Local_Actor        (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	FRAME_HEADER_LOCAL_ACTOR* frame_header_local_actor;

	gaia_local_actor = (GAIA_LOCAL_ACTOR*)Get_Actor_Concurrent_Block(calling_block->Gaia_Block);

	// Device Block
	Remove_Callback_Block_Event(frame_header_local_actor->Device_Block->On_Unemploy, On_Device_Unemploy_Frame_Header_Local_Actor);
	Remove_Callback_Block_Event(frame_header_local_actor->Device_Block->On_Commit  , On_Device_Commit_Frame_Header_Local_Actor);

	// Buffer Block
	Remove_Callback_Block_Event(frame_header_local_actor->Buffer_Block->On_Unemploy, On_Buffer_Unemploy_Frame_Header_Local_Actor);

	Unemploy_Gaia_Local_Actor(gaia_local_actor, frame_header_local_actor->Device_Block);
	Unemploy_Gaia_Local_Actor(gaia_local_actor, frame_header_local_actor->Buffer_Block);
	Unemploy_Gaia_Local_Actor(gaia_local_actor, frame_header_local_actor->Buffer_Block);
	Release_Data_Block       (frame_header_local_actor->Context_Data);

	delete frame_header_local_actor;
}

void Device_Unemploy_Frame_Local_Actor (CONCURRENT_BLOCK* const device_block, DATA_BLOCK* const context_data)
{
	ostringstream message;
	message << "@FRAME_HEADER_LOCAL_ACTOR::On_Device_Unemploy_Frame_Header_Local_Actor(" <<
		"device_block" << ": " << (void*)device_block << ", " <<
		"context_data" << ": " << (void*)context_data << ") " <<
		"An unexpected uiemploy event was raised for a frame device managed by an active frame header.";
	Warning(message.str());

	FRAME_HEADER_LOCAL_ACTOR* header_actor;

	Read_Data_Block(context_data, (char*)&header_actor);

	header_actor->Mutex.lock();

	header_actor->Device_Block = NULL;

	header_actor->Mutex.unlock();
}
void Device_Commit_Frame_Local_Actor   (CONCURRENT_BLOCK* const device_block, DATA_BLOCK* const context_data)
{
	FRAME_HEADER_LOCAL_ACTOR* header_actor;
	DATA_BLOCK*               device_data;
	FRAME_DEVICE_BLOCK*       device;

	Read_Data_Block(context_data, (char*)&header_actor);

	header_actor->Mutex.lock();

	if (header_actor->Buffer_Block != NULL)
	{
		device_data = Capture_Latest_Concurrent_Block(device_block);
		if (device_data == NULL)
		{
			Release_Data_Block(device_data);

			ostringstream message;
			message << "@FRAME_HEADER_LOCAL_ACTOR::On_Device_Commit_Frame_Header_Local_Actor(" <<
				"device_block" << ": " << (void*)device_block << ", " <<
				"context_data" << ": " << (void*)context_data << ") " <<
				"Committed device frame is NULL";
			Warning(message.str());

			return;
		}

		Access_Captured_Data_Block(device_data, (char**)&device);

		Clear_Concurrent_Block(header_actor->Buffer_Block);
		Add_Concurrent_Block(header_actor->Buffer_Block, Construct_Data_Block(device->Width * device->Height * sizeof(unsigned int)));
		Add_Concurrent_Block(header_actor->Buffer_Block, Construct_Data_Block(device->Width * device->Height * sizeof(unsigned int)));
		Add_Concurrent_Block(header_actor->Buffer_Block, Construct_Data_Block(device->Width * device->Height * sizeof(unsigned int)));

		Release_Data_Block(device_data);
	}

	header_actor->Mutex.unlock();
}

void Buffer_Unemploy_Frame_Local_Actor (CONCURRENT_BLOCK* const buffer_block, DATA_BLOCK* const context_data)
{
	ostringstream message;
	message << "@FRAME_HEADER_LOCAL_ACTOR::On_Buffer_Unemploy_Frame_Header_Local_Actor(" <<
		"employ_block" << ": " << (void*)employ_block << ", " <<
		"context_data" << ": " << (void*)context_data << ") " <<
		"An unexpected uiemploy event was raised for a frame buffer managed by an active frame header.";
	Warning(message.str());

	FRAME_HEADER_LOCAL_ACTOR* header_actor;

	Read_Data_Block(context_data, (char*)&header_actor);

	header_actor->Mutex.lock();

	header_actor->Buffer_Block = NULL;

	header_actor->Mutex.unlock();
}

// Aliases
inline void Frame_Local_Actor_Employ   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Employ_Frame_Local_Actor(calling_block, context_data);
}
inline void Frame_Local_Actor_Unemploy (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Unemploy_Frame_Local_Actor(calling_block, context_data);
}
