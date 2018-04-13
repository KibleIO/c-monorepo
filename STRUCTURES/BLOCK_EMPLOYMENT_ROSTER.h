#ifndef BLOCK_EMPLOYMENT_ROSTER_H_
#define BLOCK_EMPLOYMENT_ROSTER_H_

#include "../BLOCK_EVENTS/BLOCK_EVENT.h"

const BLOCK_EVENT** const LOCAL_BLOCK_EMPLOYMENT_ROSTER  [BLOCK_TYPE_TERMINATOR];
const BLOCK_EVENT** const REMOTE_BLOCK_EMPLOYMENT_ROSTER [BLOCK_TYPE_TERMINATOR];

void Initialize_Default_Employment ();
void Unitialize_Default_Employment ();

void Employ_Default_Local_Block    (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Employ_Default_Remote_Block   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // BLOCK_EMPLOYMENT_ROSTER_H_
