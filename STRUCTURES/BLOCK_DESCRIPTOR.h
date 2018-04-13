#ifndef BLOCK_DESCRIPTOR_H_
#define BLOCK_DESCRIPTOR_H_

#include "BLOCK_TYPE_FLAGS.h"
#include "BLOCK_OPTION_FLAGS.h"

struct BLOCK_DESCRIPTOR
{
	unsigned int  Type_Flag;         // Indicates a single block type - See "BLOCK_TYPE_FLAGS.h"
	unsigned int  Permitted_Latency; // Used to determine send priority
	unsigned long Options_Flag;      // Indicates 0 to 32 block options - See "BLOCK_OPTION_FLA GS.h"
};

const BLOCK_DESCRIPTOR DEFAULT_IDLE_BLOCK_DESCRIPTOR =
{
	BLOCK_TYPE_IDLE,
	BLOCK_OPTION_NONE,
	BLOCK_LATENCY_INFINITE
};

#endif // BLOCK_DESCRIPTOR_H_
