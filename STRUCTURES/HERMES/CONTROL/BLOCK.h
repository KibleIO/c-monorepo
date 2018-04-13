#ifndef HERMES_CONTROL_BLOCK_H_
#define HERMES_CONTROL_BLOCK_H_

#include "BLOCK_DESCRIPTOR.h"

// Only present not using a controlled protocol

struct HERMES_CONTROL_BLOCK
{
	unsigned long Packet_Index;
	unsigned long Delivered_At;
};

const struct BLOCK_DESCRIPTOR DEFAULT_HERMES_CONTROL_DESCRIPTOR =
{
	BLOCK_TYPE_HERMES_CONTROL,
	BLOCK_OPTION_SEND,
	BLOCK_LATENCY_IMMEDIATE
};

#endif // HERMES_DEVICE_BLOCK_H_
