#include "GAIA_REMOTE_ACTOR.h"

void Employ_Gaia_Remote_Actor    (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	if (calling_block->Gaia_Block != NULL)
	{
		Add_Callback_Block_Event   (calling_block->On_Employ, gaia_local_block->Gaia_Block->On_Employ);
		Remove_Callback_Block_Event(calling_block->On_Employ, Employ_Default_Local_Block);
		Remove_Callback_Block_Event(calling_block->On_Employ, Employ_Default_Remote_Block);
	}
	Add_Callback_Block_Event(calling_block->On_Employ, Employ_Default_Remote_Block);

	GAIA_REMOTE_ACTOR* gaia_remote_actor;

	gaia_remote_actor                      = new GAIA_REMOTE_ACTOR;
	gaia_remote_actor->Gaia                = Construct_Gaia();
	//gaia_remote_actor->Command_Context     = new GAIA_COMMAND_CONTEXT;
	gaia_remote_actor->Sequential_Commands = Construct_Sequential_Block_Event();

	Set_Actor_Concurrent_Block(gaia_remote_block, (void*)gaia_remote_actor);
}
void Unemploy_Gaia_Remote_Actor  (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	GAIA_REMOTE_ACTOR* gaia_remote_actor;

	gaia_remote_actor = (GAIA_REMOTE_ACTOR*) Get_Actor_Concurrent_Block(calling_block);

	Deconstruct_Sequential_Block_Event(gaia_remote_actor->Sequential_Commands);
	Deconstruct_Gaia                  (gaia_remote_actor->Gaia               );

	//delete gaia_remote_actor->Command_Context;
	delete gaia_remote_actor;
}
