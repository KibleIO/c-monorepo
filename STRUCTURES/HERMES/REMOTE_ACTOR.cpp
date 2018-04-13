#include "HERMES.h"

// Decrypts packet to remote_gaia command queue which decodes and applies it (see Decrypt_Hermes_Header_Actor)
void Decrypt_Hermes_Header_Actor  (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const encrypted_data)
{
	HERMES_ACTOR*            hermes_actor;

	CONCURRENT_BLOCK*        known_key_block;
	unsigned int             known_key_index;
	DATA_BLOCK*              known_key_data;
	char*                    known_key_iterator;
	char*                    known_key_terminator;

	CONCURRENT_BLOCK*        empty_key_block;
	unsigned int             empty_key_index;
	DATA_BLOCK*              empty_key_data;
	char*                    empty_key_iterator;
	char*                    empty_key_terminator;

	char*                    encrypted_iterator;
	char*                    encrypted_terminator;

	unsigned int             checksum;

	hermes_actor         = (HERMES_ACTOR*) Get_Actor_Concurrent_Block(calling_block);
	encrypted_terminator = Access_Captured_Data_Block(encrypted_data_block, (char**)&encrypted_iterator);
	if ((unsigned long)(encrypted_terminator - encrypted_iterator) != PACKET_BYTE_CAPACITY)
	{
		ostringstream message;
		message << "@HERMES_ACTOR::Decrypt_Hermes_Actor(" <<
			"calling_block"        << ": " << (void*)calling_block        << ", " <<
			"encrypted_data_block" << ": " << (void*)encrypted_data_block << ") " <<
			"Parameter 'encrypted_data_block' is an invalid length";
		Error(message.str());
	}

	known_key_index      = *((unsigned int*)encrypted_iterator);
	encrypted_iterator  += sizeof(unsigned int);

	known_key_block = Get_Block_Local_Gaia(hermes_actor->Remote_Keys_Gaia_Block, known_key_index);
	#ifdef GAIA_BOUNDS_CHECKING
	if (known_key_block == NULL)
	{
		ostringstream message;
		message << "@HERMES_ACTOR::Decrypt_Hermes_Actor(" <<
			"calling_block"        << ": " << (void*)calling_block        << ", " <<
			"encrypted_data_block" << ": " << (void*)encrypted_data_block << ") " <<
			"Specified key index is unknown in key gaia - Check GAIA log and/or minimize packet corruption";
		Warning(message.str());
		return;
	}
	#endif // GAIA_BOUNDS_CHECKING

	known_key_data = Capture_Read_Concurrent_Block(known_key_data);
	if (known_key_data == NULL)
	{
		empty_key_index      = known_key_index;
		empty_key_block      = known_key_block;
		empty_key_data       = Construct_Data_Block(KEY_BYTE_CAPACITY);
		empty_key_terminator = Access_Captured_Data_Block(empty_key_data, (char**)&empty_key_iterator);
		known_key_index      = *((unsigned int*)encrypted_iterator);
		encrypted_iterator  += sizeof(unsigned int);
		known_key_block      = Get_Block_Local_Gaia(hermes_actor->Remote_Keys_Gaia_Block, known_key_index);
		#ifdef GAIA_BOUNDS_CHECKING
		if (known_key_block == NULL)
		{
			ostringstream message;
			message << "@HERMES_ACTOR::Decrypt_Hermes_Actor(" <<
				"calling_block"        << ": " << (void*)calling_block        << ", " <<
				"encrypted_data_block" << ": " << (void*)encrypted_data_block << ") " <<
				"Specified known key index is unknown in key gaia - Check GAIA log and/or minimize packet corruption";
			Warning(message.str());
			return;
		}
		#endif // GAIA_BOUNDS_CHECKING

		known_key_data = Capture_Read_Concurrent_Block(known_key_data);
		if (known_key_data == NULL)
		{
			ostringstream message;
			message << "@HERMES_ACTOR::Decrypt_Hermes_Actor(" <<
				"calling_block"        << ": " << (void*)calling_block        << ", " <<
				"encrypted_data_block" << ": " << (void*)encrypted_data_block << ") " <<
				"Both known key is empty - Check GAIA log and/or minimize packet corruption";
			Warning(message.str());
			return;
		}
	}
	else
	{
		empty_key_block      = NULL;
		empty_key_index      = 0;
		empty_key_data       = NULL;
		empty_key_iterator   = NULL;
		empty_key_terminator = NULL;
	}

	known_key_terminator = Access_Captured_Data_Block(known_key_data, (char**)&known_key_iterator);
	if ((unsigned long)(known_key_terminator - known_key_iterator) != KEY_BYTE_CAPACITY)
	{
		ostringstream message;
		message << "@HERMES_ACTOR::Decrypt_Hermes_Actor(" <<
			"calling_block"        << ": " << (void*)calling_block        << ", " <<
			"encrypted_data_block" << ": " << (void*)encrypted_data_block << ") " <<
			"Key at index " << key_index << " is an invalid length";
		Error(message.str());
	}

	// Encrypt
	checksum = 0;
	while (encrypted_iterator < encrypted_terminator)
	{
		*encrypted_iterator ^= *known_key_iterator;
		checksum            += *encrypted_iterator;
		known_key_iterator++;
		encrypted_iterator++;
	}

	checksum = *((unsigned int*)encrypted_iterator);
	if (checksum == *((unsigned int*)encrypted_iterator))
	{
		if (empty_key_block != NULL)
		{
			copy(encrypted_terminator - KEY_BYTE_CAPACITY, encrypted_terminator, empty_iterator);
			Commit_Concurrent_Block(empty_key_block, empty_key_data);
		}
		else
		{
			DATA_BLOCK*       decrypted_data;
			BLOCK_EVENT_NODE* block_event_node;

			encrypted_iterator += sizeof(unsigned int);
			decrypted_data      = Construct_Data_Block((unsigned long)(encrypted_terminator - encrypted_iterator));
			Write_Captured_Data_Block(decrypted_data, encrypted_iterator, encrypted_terminator);

			block_event_node = Construct_Block_Event_Node(Command_Gaia_Remote_Actor, local_actor->Remote_Gaia_Block, decrypted_data);
			if (!Put_Ordered_Block_Event(hermes_actor->Ordered_Decrypted_Packets, block_event_node, known_key_index))
			{
				// handle
				return;
			}
			Commit_Concurrent_Block(known_key_block, NULL);
		}
	}
	else
	{
		ostringstream message;
		message << "@HERMES_ACTOR::Decrypt_Hermes_Actor(" <<
			"calling_block"        << ": " << (void*)calling_block        << ", " <<
			"encrypted_data_block" << ": " << (void*)encrypted_data_block << ") " <<
			"Decrypted data did not match the checksum - Check for corruption";
		Notice(message.str());
	}

	Release_Data_Block(known_key_data);       // Should cause deconstruction of 'known_key_data'
	Release_Data_Block(encrypted_data_block);
}
