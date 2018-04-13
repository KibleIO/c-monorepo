#include "HERMES.h"

/*
struct HERMES
{
	DATA_BLOCK*             Context_Data;

	CONCURRENT_BLOCK*       Local_Device_Block;
	CONCURRENT_BLOCK*       Local_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Mirror_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Control_Block;  // Not implemented

	CONCURRENT_BLOCK*       Remote_Device_Block;
	CONCURRENT_BLOCK*       Remote_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Mirror_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Control_Block; // Not implemented

	// CONCURRENT_BLOCK*       Packet_Pool_Block;         // Recycles data for packet use

	// Sending (Local)
	const void            (*Send_Callback)(CONCURRENT_BLOCK*, DATA_BLOCK*);
	SEQUENTIAL_BLOCK_EVENT* Sequential_Send_Event;

	// Recieving (Remote)
	ORDERED_BLOCK_EVENT*    Ordered_Decrypted_Packets;
};
*/

void    Initialize_Hermes   (HERMES* const hermes)
{
	if (hermes->Local_Device_Block       != NULL &&
	    hermes->Local_Keys_Gaia_Block    != NULL &&
	    hermes->Local_Mirror_Gaia_Block  != NULL)
	{
		// Initialize send
		// const void            (*Send_Callback)(CONCURRENT_BLOCK*, DATA_BLOCK*);
		// SEQUENTIAL_BLOCK_EVENT* Sequential_Send_Event;

		Add_Callback_Block_Event(hermes->Local_Mirror_Gaia_Block->On_Employ, Employ_Local_Block_Hermes_Actor, hermes->Context_Data);
	}
	if (hermes->Remote_Device_Block      != NULL &&
	    hermes->Remote_Keys_Gaia_Block   != NULL &&
	    hermes->Remote_Mirror_Gaia_Block != NULL)
	{
		// Initialize recieve
		// ORDERED_BLOCK_EVENT*    Ordered_Decrypted_Packets;

		Add_Callback_Block_Event(hermes->Local_Mirror_Gaia_Block->On_Employ, Employ_Local_Block_Hermes_Actor, hermes->Context_Data);
	}
}
void    Uninitialize_Hermes (HERMES* const hermes)
{
	// implement when Initialize_Hermes is more developed
}
