#include "STAGED_TRANSFER.h"

void Initialize_Staged_Transfer (STAGED_TRANSFER* const staged_transfer)
{
	staged_transfer->Data          = NULL;
	staged_transfer->Delivery_Time = 0;
	staged_transfer->Offset        = 0;
	staged_transfer->Capacity      = 0;
}
void Initialize_Staged_Transfer (STAGED_TRANSFER* const staged_transfer, DATA_BLOCK* const data)
{
	staged_transfer->Data          = data;
	staged_transfer->Delivery_Time = 0;
	staged_transfer->Offset        = 0;
	staged_transfer->Capacity      = 0;

	if (data != NULL)
	{
		Capture_Data_Block(data);
	}
}
void Uninitialize_Staged_Transfer(STAGED_TRANSFER* const staged_transfer)
{
	if (staged_transfer->Data != NULL)
	{
		Capture_Data_Block(staged_transfer->Data);
	}
}
