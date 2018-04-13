#ifndef STAGED_TRANSFER_H_
#define STAGED_TRANSFER_H_

struct STAGED_TRANSFER
{
	DATA_BLOCK*   Data;
	unsigned long Delivery_Time;
	unsigned long Offset;
	unsigned long Capacity;
};

void Initialize_Staged_Transfer  (STAGED_TRANSFER* const staged_transfer);
void Initialize_Staged_Transfer  (STAGED_TRANSFER* const staged_transfer, DATA_BLOCK* const data);
void Uninitialize_Staged_Transfer(STAGED_TRANSFER* const staged_transfer);

#endif // STAGED_TRANSFER_H_
