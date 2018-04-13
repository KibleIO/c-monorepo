#include "HERMES.h"

struct HERMES
{
	DATA_BLOCK*             Context_Data;
	CONCURRENT_BLOCK*       Device_Block;              // Source_Block

	CONCURRENT_BLOCK*       Local_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Transfer_Log_Block;  // Not implemented

	CONCURRENT_BLOCK*       Remote_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Transfer_Log_Block; // Not implemented

	CONCURRENT_BLOCK*       Packet_Pool_Block;         // Recycles data for packet use

	// Sending (Local)
	const void            (*Send_Callback)(CONCURRENT_BLOCK*, DATA_BLOCK*);
	SEQUENTIAL_BLOCK_EVENT* Sequential_Send_Event;

	// Recieving (Remote)
	ORDERED_BLOCK_EVENT*    Ordered_Decrypted_Packets;
};

void Employ_Hermes_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
void Unemploy_Hermes_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data);
