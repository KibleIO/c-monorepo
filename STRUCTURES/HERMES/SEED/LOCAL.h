#ifndef HERMES_SEED_LOCAL_H_
#define HERMES_SEED_LOCAL_H_

// ONLY USED BY HOST

#include "../HERMES.h"

void Employ_Hermes_Seed_Local     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Hermes_Seed_Local   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // HERMES_SEED_LOCAL_H_
