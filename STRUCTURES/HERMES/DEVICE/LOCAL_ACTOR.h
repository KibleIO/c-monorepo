#ifndef NETWORK_DEVICE_lOCAL_ACTOR_H_
#define NETWORK_DEVICE_lOCAL_ACTOR_H_

#include "../../../CONCURRENT_BLOCK.h"

struct HERMES_DEVICE_LOCAL_ACTOR
{
};

void Employ_Network_Device_Local_Actor   (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);
void Unemploy_Network_Device_Local_Actor (CONCURRENT_BLOCK* const employ_block, DATA_BLOCK* const context_data);

#endif // NETWORK_DEVICE_lOCAL_ACTOR_H_
