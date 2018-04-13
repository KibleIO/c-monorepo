#include "LOCAL_ACTOR.h"

void Employ_Frame_Device_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{

}
void Unemploy_Frame_Device_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{

}


// Aliases
inline void Frame_Device_Local_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{
	Employ_Frame_Device_Local_Actor(employ_block, context_data);
}
inline void Frame_Device_Local_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{
	Unemploy_Frame_Device_Local_Actor(employ_block, context_data);
}
