#ifndef BLOCK_EMPLOYMENT_ROSTER_H_
#define BLOCK_EMPLOYMENT_ROSTER_H_

#include <Gaia/CONCURRENT_BLOCK.h>
#include <Gaia/BLOCK_DESCRIPTOR.h>
#include <Gaia/BLOCK_EVENTS/BLOCK_EVENT.h>
#include "IDLE/ACTOR.h"
#include "GAIA/LOCAL_ACTOR.h"
#include "GAIA/REMOTE_ACTOR.h"
#include "GAIA/ROOT/LOCAL_ACTOR.h"
#include "GAIA/ROOT/REMOTE_ACTOR.h"

BLOCK_EVENT** LOCAL_BLOCK_EMPLOYMENT_ROSTER;
BLOCK_EVENT** REMOTE_BLOCK_EMPLOYMENT_ROSTER;

void Initialize_Default_Employment ();
void Unitialize_Default_Employment ();

void Employ_Default_Local_Block    (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Employ_Default_Remote_Block   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // BLOCK_EMPLOYMENT_ROSTER_H_
