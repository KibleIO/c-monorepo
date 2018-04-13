#ifndef BLOCK_TYPES_FRAME_DEVICE_BLOCK_H_
#define BLOCK_TYPES_FRAME_DEVICE_BLOCK_H_

#include "BLOCK_DESCRIPTOR.h"

struct FRAME_DEVICE_BLOCK
{
	unsigned int  Width;
	unsigned int  Height;
	unsigned long Remote_ID;
};

const struct BLOCK_DESCRIPTOR DEFAULT_FRAME_HEADER_BLOCK_DESCRIPTOR =
{
	BLOCK_TYPE_FRAME_DEVICE_LOCAL,
	BLOCK_OPTION_SEND,
	BLOCK_LATENCY_MODERATE
};

#endif // BLOCK_TYPES_FRAME_DEVICE_BLOCK_H_
