#ifndef HERMES_H_
#define HERMES_H_

#include "../../CONCURRENT_BLOCK.h"

/*
	DATA_BLOCK*             Context_Data;
	CONCURRENT_BLOCK*       Device_Block;              // Source_Block

	CONCURRENT_BLOCK*       Local_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Transfer_Log_Block;  // Not implemented

	CONCURRENT_BLOCK*       Remote_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Transfer_Log_Block; // Not implemented

	CONCURRENT_BLOCK*       Packet_Pool_Block;         // Recycles data for packet use

	// Sending (Local)
	const void            (*Send_Callback)(CONCURRENT_BLOCK*, DATA_BLOCK*);
	SEQUENTIAL_BLOCK_EVENT* Sequential_Send_Event;

	// Recieving (Remote)
	ORDERED_BLOCK_EVENT*    Ordered_Decrypted_Packets;
*/

void Employ_Hermes_Actor              (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	HERMES_ACTOR*     hermes_actor;
	GAIA_LOCAL_ACTOR* gaia_local_actor;
	DATA_BLOCK*       context_data;

	hermes_actor     = new HERMES_ACTOR;
	gaia_local_actor = (GAIA_LOCAL_ACTOR*)Get_Actor_Concurrent_Block(calling_block->Gaia_Block);
	context_data     = Construct_Data_Block(sizeof(HERMES_ACTOR*));

	Capture_Data_Block(context_data);
	Write_Captured_Data_Block(context_data, &((char*)hermes_header_actor), &((char*)hermes_header_actor) + sizeof(HERMES_HEADER_ACTOR*));

	hermes_actor->Source_Block                = calling_block;

	hermes_actor->Local_Network_Device_Block  = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_NETWORK_DEVICE_LOCAL_BLOCK_DESCRIPTOR, true);
	hermes_actor->Local_Keys_Gaia_Block       = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_GAIA_LOCAL_BLOCK_DESCRIPTOR          , true);
	hermes_actor->Local_Gaia_Block            = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_GAIA_LOCAL_BLOCK_DESCRIPTOR          , true);
	hermes_actor->Local_Transfer_Log_Block    = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_HERMES_TRANSFER_LOG_BLOCK_DESCRIPTOR , true);

	hermes_actor->Remote_Network_Device_Block = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_NETWORK_DEVICE_REMOTE_BLOCK_DESCRIPTOR, true);
	hermes_actor->Remote_Keys_Gaia_Block      = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_GAIA_REMOTE_BLOCK_DESCRIPTOR          , true);
	hermes_actor->Remote_Gaia_Block           = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_GAIA_REMOTE_BLOCK_DESCRIPTOR          , true);
	hermes_actor->Remote_Transfer_Log_Block   = Employ_Idle_Block_Gaia_Local_Actor(gaia_local_actor, DEFAULT_HERMES_TRANSFER_LOG_BLOCK_DESCRIPTOR  , true);

	// hermes_actor->Packet_Pool_Block           = ???

	hermes_actor->Sequential_Send_Event       = Construct_Sequential_Block_Event();
	hermes_actor->Ordered_Decrypted_Packets   = Construct_Ordered_Block_Event();

	Add_Callback_Block_Event(hermes_actor->Local_Gaia_Block->On_Employ, Employ_Local_Block_Hermes_Actor, context_data);
	Set_Actor_Concurrent_Block(calling_block, (void*)hermes_header_actor);
}
void Unemploy_Hermes_Actor            (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	HERMES_ACTOR* hermes_actor;

	hermes_actor = (HERMES_ACTOR*)Get_Actor_Concurrent_Block(calling_block);

	Release_Data_Block(hermes_actor->Context_Data);
	delete hermes_actor;
}
#endif // HERMES_H_
