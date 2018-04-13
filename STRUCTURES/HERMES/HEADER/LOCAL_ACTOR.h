#ifndef HERMES_HEADER_LOCAL_ACTOR
#define HERMES_HEADER_LOCAL_ACTOR

#include "../HERMES.h"

// SHOULD ONLY BE USED BY HOST!

// Initializes a Hermes instance and maintains it's HERMES_HEADER_BLOCK to reflect
// block indexes of the current Hermes components

void Employ_Hermes_Header_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Hermes_Header_Local_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

// Updates BLOCK to latest indexes
void Load_Hermes_Header_Local_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // HERMES_HEADER_LOCAL_ACTOR
