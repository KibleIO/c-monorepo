#ifndef HERMES_DEVICE_LOCAL_ACTOR
#define HERMES_DEVICE_LOCAL_ACTOR

#include "../HERMES.h"

// Spawns a network connection with the machine and protocol specified by its HERMES_DEVICE_BLOCK

struct HERMES_DEVICE_LOCAL_ACTOR
{
	CONCURRENT_BLOCK* Outbound_Packet_Block;
};

void Employ_Hermes_Device_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Hermes_Device_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // HERMES_DEVICE_LOCAL_ACTOR
