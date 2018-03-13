#include "LOCAL_ACTOR.h"

void Employ_Network_Device_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{

}
void Unemploy_Network_Device_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{

}


// Aliases
inline void Network_Device_Local_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{
	Employ_Network_Device_Local_Actor(employ_block, context_data);
}
inline void Network_Device_Local_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data)
{
	Unemploy_Network_Device_Local_Actor(employ_block, context_data);
}
