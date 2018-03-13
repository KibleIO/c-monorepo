#ifndef NETWORK_DEVICE_lOCAL_ACTOR_H_
#define NETWORK_DEVICE_lOCAL_ACTOR_H_

#include "../../../CONCURRENT_BLOCK.h"

struct NETWORK_DEVICE_LOCAL_ACTOR
{
	CONCURRENT_BLOCK* Outbound_Packet_Block;
};

void Employ_Network_Device_Local_Actor     (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Network_Device_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

void Add_Packet_Network_Device_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
{
}

// Aliases
inline void Network_Device_Local_Actor_Employ   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
inline void Network_Device_Local_Actor_Unemploy (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // NETWORK_DEVICE_lOCAL_ACTOR_H_
