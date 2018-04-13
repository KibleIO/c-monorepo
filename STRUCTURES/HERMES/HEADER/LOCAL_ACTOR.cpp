#include "LOCAL_ACTOR.h"

// SHOULD ONLY BE USED BY HOST!

void Employ_Hermes_Header_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	HERMES*           hermes;
	GAIA_LOCAL_ACTOR* gaia_local;

	hermes     = new HERMES;
	gaia_local = (GAIA_LOCAL_ACTOR*) Get_Actor_Concurrent_Block(calling_block);

	hermes->Local_Device_Block       = Employ_Idle_Block_Gaia_Local_Actor(gaia_local, DEFAULT_HERMES_DEVICE_BLOCK_DESCRIPTOR, true);
	hermes->Local_Key_Gaia_Block     = Employ_Idle_Block_Gaia_Local_Actor(gaia_local, DEFAULT_GAIA_LOCAL_BLOCK_DESCRIPTOR   , true);
	hermes->Local_Mirror_Gaia_Block  = Employ_Idle_Block_Gaia_Local_Actor(gaia_local, DEFAULT_GAIA_LOCAL_BLOCK_DESCRIPTOR   , true);
	hermes->Local_Control_Block      = NULL;

	hermes->Remote_Device_Block      = Employ_Idle_Block_Gaia_Local_Actor(gaia_local, DEFAULT_HERMES_DEVICE_BLOCK_DESCRIPTOR, true);
	hermes->Remote_Key_Gaia_Block    = Employ_Idle_Block_Gaia_Local_Actor(gaia_local, DEFAULT_GAIA_REMOTE_BLOCK_DESCRIPTOR  , true);
	hermes->Remote_Mirror_Gaia_Block = Employ_Idle_Block_Gaia_Local_Actor(gaia_local, DEFAULT_GAIA_REMOTE_BLOCK_DESCRIPTOR  , true);
	hermes->Local_Control_Block      = NULL;

	Initialize_Hermes(hermes);

	Remove_Callback_Block_Event(calling_block->On_Unemploy, Unexpected_Block_Event);
	Add_Callback_Block_Event   (calling_block->On_Unemploy, Unemploy_Hermes_Header_Local_Actor, hermes->Context_Data);
}
void Unemploy_Hermes_Header_Local_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	if (context_data == NULL)
	{
		// Warning
		return;
	}
	if (Byte_Capacity_Data_Block(context_data) != sizeof(HERMES*))
	{
		// Warning
		return;
	}

	HERMES* hermes;

	Access_Captured_Data_Block(context_data, (char**)&hermes);

	Uninitialize_Hermes(hermes);
	delete hermes;
}
