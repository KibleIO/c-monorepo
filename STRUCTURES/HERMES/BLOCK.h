#ifndef HERMES_HEADER_BLOCK_H_
#define HERMES_HEADER_BLOCK_H_

#include "BLOCK_DESCRIPTOR.h"

struct HERMES_HEADER_BLOCK
{
	unsigned long  Device_Index;

	unsigned long* Local_Keys_Gaia_Index;
	unsigned long* Local_Gaia_Index;
	unsigned long* Local_Transfer_Log_Index;

	unsigned long* Remote_Keys_Gaia_Index;
	unsigned long* Remote_Gaia_Index;
	unsigned long* Remote_Transfer_Log_Index;
};

const struct BLOCK_DESCRIPTOR DEFAULT_HERMES_HEADER_BLOCK_DESCRIPTOR =
{
	BLOCK_TYPE_HERMES_HEADER,
	BLOCK_OPTION_SEND,
	BLOCK_LATENCY_MODERATE
};

#endif // HERMES_HEADER_BLOCK_H_
