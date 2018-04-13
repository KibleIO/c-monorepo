#include "REMOTE_ACTOR.h"

void Employ_Hermes_Header_Remote_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Remove_Callback_Block_Event(calling_block->On_Unemploy, Unexpected_Block_Event);
	Remove_Callback_Block_Event(calling_block->On_Commit  , Unexpected_Block_Event);

	Add_Callback_Block_Event   (calling_block->On_Unemploy, Unemploy_Hermes_Header_Remote_Actor);
	Add_Callback_Block_Event   (calling_block->On_Commit  , Load_Hermes_Header_Remote_Actor);
}
void Unemploy_Hermes_Header_Remote_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Remove_Callback_Block_Event(calling_block->On_Commit, Load_Hermes_Header_Remote_Actor);

	if (context_data != NULL)
	{
		if (Byte_Capacity_Data_Block(context_data) == sizeof(HERMES*))
		{
			HERMES* hermes;

			Read_Captured_Data_Block(context_data, (char*)&hermes);

			Uninitialize_Hermes(hermes);
			delete hermes;
		}
		else
		{
			// Warning
		}
	}
}

// On_Commit
void Load_Hermes_Header_Remote_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	DATA_BLOCK* header_data;

	header_data = Capture_Latest_Concurrent_Block(calling_block);

	if (header_data != NULL)
	{
		if (Byte_Capacity_Data_Block(header_data) != sizeof(HERMES_HEADER_BLOCK))
		{
			// WARNING - invalid block size
			Release_Data_Block(header_data);
			return;
		}

		Remove_Callback_Block_Event(calling_block->On_Commit  , Load_Hermes_Header_Remote_Actor    ); // this
		Remove_Callback_Block_Event(calling_block->On_Unemploy, Unemploy_Hermes_Header_Remote_Actor);

		HERMES*              hermes;
		GAIA_REMOTE_ACTOR*   gaia_remote;
		HERMES_HEADER_BLOCK* header_block;

		hermes      = new HERMES; // deleted by unemploy
		gaia_remote = (GAIA_REMOTE_ACTOR*) calling_block->Gaia_Block;

		Access_Captured_Data_Block(header_data, (char**)&header_block);

		hermes->Local_Device_Block       = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Local_Device_Index      );
		hermes->Local_Key_Gaia_Block     = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Local_Key_Gaia_Index    );
		hermes->Local_Mirror_Gaia_Block  = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Local_Mirror_Gaia_Index );
		hermes->Local_Control_Block      = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Local_Control_Index     );
		hermes->Remote_Device_Block      = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Remote_Device_Index     );
		hermes->Remote_Key_Gaia_Block    = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Remote_Key_Gaia_Index   );
		hermes->Remote_Mirror_Gaia_Block = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Remote_Mirror_Gaia_Index);
		hermes->Remote_Control_Block     = Get_Block_Gaia_Remote_Actor(gaia_remote, header_block->Remote_Control_Index    );

		Initialize_Hermes(hermes);

		Add_Callback_Block_Event(calling_block->On_Unemploy, Unemploy_Hermes_Header_Remote_Actor, hermes->Context_Data);

		Release_Data_Block(header_data);
	}
}
