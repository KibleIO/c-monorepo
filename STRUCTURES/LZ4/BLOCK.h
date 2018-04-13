#ifndef LZ4_BLOCK_H_
#define LZ4_BLOCK_H_

#include "BLOCK_DESCRIPTOR.h"

struct LZ4_BLOCK; // Add any parameters needed to initialize LZ4 encoder or decoder

const struct BLOCK_DESCRIPTOR DEFAULT_LZ4_BLOCK_DESCRIPTOR =
{
	BLOCK_TYPE_LZ4,
	BLOCK_OPTION_NONE,
	BLOCK_LATENCY_NEVER
};

#endif // LZ4_BLOCK_H_
