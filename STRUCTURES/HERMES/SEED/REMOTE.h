#ifndef HERMES_SEED_REMOTE_H_
#define HERMES_SEED_REMOTE_H_

#include "../HERMES.h"

void Employ_Hermes_Seed_Remote     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Hermes_Seed_Remote   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

void Queue_Load_Hermes_Seed_Remote (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Load_Hermes_Seed_Remote       (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

#endif // HERMES_SEED_REMOTE_H_
