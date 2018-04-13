#ifndef HERMES_HEADER_BLOCK_H_
#define HERMES_HEADER_BLOCK_H_

#include "../../BLOCK_DESCRIPTOR.h"

// Stores locations of Hermes components

struct HERMES_HEADER_BLOCK
{
	unsigned long Local_Device_Index;
	unsigned long Local_Key_Gaia_Index;
	unsigned long Local_Mirror_Gaia_Index;
	unsigned long Local_Control_Index;

	unsigned long Remote_Device_Index;
	unsigned long Remote_Key_Gaia_Index;
	unsigned long Remote_Mirror_Gaia_Index;
	unsigned long Remote_Control_Index;
};

const struct BLOCK_DESCRIPTOR DEFAULT_HERMES_HEADER_BLOCK_DESCRIPTOR =
{
	BLOCK_TYPE_HERMES_HEADER,
	BLOCK_OPTION_GAY,
	BLOCK_LATENCY_GAY
};

#endif // HERMES_DEVICE_BLOCK_H_
