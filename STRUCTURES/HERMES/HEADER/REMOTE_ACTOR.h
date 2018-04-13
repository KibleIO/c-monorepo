#ifndef HERMES_HEADER_REMOTE_ACTOR
#define HERMES_HEADER_REMOTE_ACTOR

#include "../HERMES.h"

// Initializes a Hermes and maintains its components with the indexes specified
// in its HERMES_HEADER_BLOCK

void Employ_Hermes_Header_Remote_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Hermes_Header_Remote_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

// Updates Hermes components to athe blocks at the latest specified indexes
void Load_Hermes_Header_Remote_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // HERMES_HEADER_REMOTE_ACTOR
