#include "HERMES.h"

// bandwidth is bytes per microsecond
// times are all in microseconds

// MOVE
/*
struct STAGED_TRANSFER
{
	DATA_BLOCK*   Data;
	unsigned long Delivery_Time;
	unsigned long Offset;
	unsigned long Capacity;
};
*/

struct HERMES_LOCAL_ACTOR
{
	HERMES_LOCAL_ACTOR* volatile Previous;
	HERMES_LOCAL_ACTOR* volatile Next;

	HERMES*                      Hermes;             // const
	CONCURRENT_BLOCK*            Source_Block;       // const
	DATA_BLOCK*         volatile Last_Transfer_Data;

	STAGED_TRANSFER*             Staged_Transfers;   // const []
	unsigned long       volatile Queue_Index;
	unsigned long       volatile Dequeue_Index;
	unsigned long                Capacity;           // cosnt

	BLOCK_DESCRIPTOR             Descriptor;

	mutex                        Mutex;
};

unsigned long Calculate_Urgency_Staged_Transfer (STAGED_TRANSFER* staged_transfer, double bandwidth);
// END MOVE

// local_gaia's mirrored block's on_commit event encodes changes                (see Commit_Block_Hermes)
// command is staged and added to hermes transmission queue, ordered by urgency (see Queue_Transmission_Hermes)
// determines network device with soonest delivery date                         (see Send_Loop_Hermes)
// encrypts packet to network device                                            (see Encrypt_Hermes_Actor)

void Employ_Local_Block_Hermes_Actor  (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemply_Local_Block_Hermes_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);

void Encode_Local_Block_Hermes_Actor  (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Local_Loop_Hermes_Actor          (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
