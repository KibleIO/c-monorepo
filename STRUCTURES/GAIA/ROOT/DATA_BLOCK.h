#ifndef GAIA_ROOT_DATA_BLOCK_H_
#define GAIA_ROOT_DATA_BLOCK_H_

#include <Gaia/BLOCK_DESCRIPTOR.h>
#include <Gaia/BLOCK_OPTION_FLAGS.h>
#include "../../BLOCK_TYPE_FLAGS.h"

struct GAIA_ROOT_DATA_BLOCK
{
	BLOCK_DESCRIPTOR Descriptor; // Array
};

static const struct BLOCK_DESCRIPTOR DEFAULT_GAIA_ROOT_BLOCK_DESCRIPTOR =
{
	BLOCK_TYPE_GAIA_ROOT,
	BLOCK_OPTION_LATEST_COMMAND & BLOCK_OPTION_COMPRESS_COMMANDS,
	BLOCK_LATENCY_INFINITE
};

#endif // GAIA_ROOT_DATA_BLOCK_H_
