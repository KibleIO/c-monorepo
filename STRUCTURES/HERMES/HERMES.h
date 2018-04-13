#ifndef HERMES_H_
#define HERMES_H_

struct HERMES
{
	DATA_BLOCK*             Context_Data;

	// Key structure
	// A:
	CONCURRENT_BLOCK*       Local_Keys_Gaia_Block;
	// B:
	GAIA*                   Local_Keys_Gaia;
	// C:
	DATA_BLOCK*             Local_Key_Iterator;
	DATA_BLOCK*             Local_Key_Terminator;
	// END Key structure

	CONCURRENT_BLOCK*       Local_Device_Block;
	CONCURRENT_BLOCK*       Local_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Mirror_Gaia_Block;
	CONCURRENT_BLOCK*       Local_Control_Block;      // Not implemented

	CONCURRENT_BLOCK*       Remote_Device_Block;
	CONCURRENT_BLOCK*       Remote_Keys_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Mirror_Gaia_Block;
	CONCURRENT_BLOCK*       Remote_Control_Block;     // Not implemented

	// CONCURRENT_BLOCK*       Packet_Pool_Block;     // Recycles data for packet use

	// Sending (Local)
	const void            (*Send_Callback)(CONCURRENT_BLOCK*, DATA_BLOCK*);
	SEQUENTIAL_BLOCK_EVENT* Sequential_Send_Event;

	// Recieving (Remote)
	ORDERED_BLOCK_EVENT*    Ordered_Decrypted_Packets;
};

/*
	HERMES_DEVICE_BLOCK local_device;
	HERMES_DEVICE_BLOCK remote_device;
	unsigned long       Local_Key_Capacity;       // MTU
	unsigned long       Remote_Key_Capacity;      // MTU
	unsigned long       Local_Key_Seed_Capacity;  // in multiples of Local_Key_Capacity
	unsigned long       Remote_Key_Seed_Capacity; // in multiples of Remote_Key_Capacity
	char[]              Local_Key_Seed_Data;
	char[]              Remote_Key_Seed_Data;
*/

void Initialize_Hermes (HERMES* const hermes, const HERMES_SEED_BLOCK* cosnt seed_block, GAIA_LOCAL_ACTOR* const main_gaia)
{
	hermes->Context_Data             = Construct_Data_Block(sizeof(HERMES*));
	Capture_Data_Block(hermes->Context_Data);
	Write_Captured_Data_Block(hermes->Context_Data, (char*)&hermes, ((char*)&hermes) + sizeof(HEREMES*));

	hermes->Local_Device_Block       = Employ_Idle_Block_Gaia_Local_Actor(main_gaia                       , DEFAULT_HERMES_DEVICE_BLOCK_DESCRIPTOR , true);
	hermes->Local_Keys_Gaia_Block    = Employ_Idle_Block_Gaia_Local_Actor(main_gaia                       , DEFAULT_KEY_LOCAL_BLOCK_DESCRIPTOR     , true);
	hermes->Local_Mirror_Gaia_Block  = Employ_Idle_Block_Gaia_Local_Actor(main_gaia                       , DEFAULT_GAIA_LOCAL_BLOCK_DESCRIPTOR    , true);
	hermes->Local_Control_Block      = Employ_Idle_Block_Gaia_Local_Actor(hermes->Local_Mirror_Gaia_Block , DEFAULT_HERMES_CONTROL_BLOCK_DESCRIPTOR, true);

	hermes->Remote_Device_Block      = Employ_Idle_Block_Gaia_Local_Actor(main_gaia                       , DEFAULT_HERMES_DEVICE_BLOCK_DESCRIPTOR , true);
	hermes->Remote_Keys_Gaia_Block   = Employ_Idle_Block_Gaia_Local_Actor(main_gaia                       , DEFAULT_KEY_REMOTE_BLOCK_DESCRIPTOR    , true);
	hermes->Remote_Mirror_Gaia_Block = Employ_Idle_Block_Gaia_Local_Actor(main_gaia                       , DEFAULT_GAIA_REMOTE_BLOCK_DESCRIPTOR   , true);
	hermes->Remote_Control_Block     = Employ_Idle_Block_Gaia_Local_Actor(hermes->Remote_Mirror_Gaia_Block, DEFAULT_HERMES_CONTROL_BLOCK_DESCRIPTOR, true);


}

void    Initialize_Hermes   (
	HERMES* const     hermes,
	CONCURRENT_BLOCK* local_device,
	CONCURRENT_BLOCK* local_key_seed,
	CONCURRENT_BLOCK* remote_device,
	CONCURRENT_BLOCK* remote_key_seed);
void    Uninitialize_Hermes (HERMES* const hermes);

#endif // HERMES_H_
