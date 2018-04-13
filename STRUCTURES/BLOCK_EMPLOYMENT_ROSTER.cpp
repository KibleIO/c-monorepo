#include "BLOCK_EMPLOYMENT_ROSTER.h"

void Initialize_Default_Employment ()
{
	LOCAL_BLOCK_EMPLOYMENT_ROSTER  = new const DATA_BLOCK* [BLOCK_TYPE_TERMINATOR];
	REMOTE_BLOCK_EMPLOYMENT_ROSTER = new const DATA_BLOCK* [BLOCK_TYPE_TERMINATOR];

	for (unsigned long block_type = 0; block_type < BLOCK_TYPE_TERMINATOR; block_type++)
	{
		LOCAL_BLOCK_EMPLOYMENT_EVENTS  [block_type] = Construct_Block_Event();
		REMOTE_BLOCK_EMPLOYMENT_EVENTS [block_type] = Construct_Block_Event();
	}

	// IDLE
	Add_Callback_Event(LOCAL_BLOCK_EMPLOYMENT_EVENTS  [BLOCK_TYPE_IDLE], Employ_Idle_Actor);
	Add_Callback_Event(REMOTE_BLOCK_EMPLOYMENT_EVENTS [BLOCK_TYPE_IDLE], Employ_Idle_Actor);

	// GAIA
	Add_Callback_Event(LOCAL_BLOCK_EMPLOYMENT_EVENTS  [BLOCK_TYPE_IDLE], Employ_Gaia_Local_Actor);
	Add_Callback_Event(REMOTE_BLOCK_EMPLOYMENT_EVENTS [BLOCK_TYPE_IDLE], Employ_Gaia_Remote_Actor);
	Add_Callback_Event(LOCAL_BLOCK_EMPLOYMENT_EVENTS  [BLOCK_TYPE_IDLE], Employ_Root_Local_Actor);
	Add_Callback_Event(REMOTE_BLOCK_EMPLOYMENT_EVENTS [BLOCK_TYPE_IDLE], Employ_Root_Remote_Actor);
}
void Unitialize_Default_Employment ()
{
	for (unsigned long block_type = 0; block_type < BLOCK_TYPE_TERMINATOR; block_type++)
	{
		Deconstruct_Block_Event(LOCAL_BLOCK_EMPLOYMENT_EVENTS  [block_type]);
		Deconstruct_Block_Event(REMOTE_BLOCK_EMPLOYMENT_EVENTS [block_type]);
	}

	delete [] LOCAL_BLOCK_EMPLOYMENT_ROSTER;
	delete [] REMOTE_BLOCK_EMPLOYMENT_ROSTER;
}

void Employ_Default_Local_Block    (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	BLOCK_DESCRIPTOR descriptor = Get_Descriptor_Concurrent_Block(calling_block);

	if (descriptor.Type_Flag >= BLOCK_TYPE_TERMINATOR)
	{
		Error("");
	}

	Invoke_Block_Event(LOCAL_BLOCK_EMPLOYMENT_ROSTER[descriptor.Type_Flag], calling_block);
}
void Employ_Default_Remote_Block   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	BLOCK_DESCRIPTOR descriptor = Get_Descriptor_Concurrent_Block(calling_block);

	if (descriptor.Type_Flag >= BLOCK_TYPE_TERMINATOR)
	{
		Error("");
	}

	Invoke_Block_Event(REMOTE_BLOCK_EMPLOYMENT_ROSTER[descriptor.Type_Flag], calling_block);
}
