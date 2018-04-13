#include "HERMES_HEADER_ACTOR.h"

// bandwidth is bytes per microsecond (as consistent with <time.h>)
// times are all in microseconds

/*
struct HERMES_LOCAL_ACTOR
{
	HERMES_LOCAL_ACTOR*    Previous;
	HERMES_LOCAL_ACTOR*    Next;

	HERMES*                Hermes;
	CONCURRENT_BLOCK*      Source_Block;
	DATA_BLOCK*            Last_Transfer_Data;

	STAGED_TRANSFER*       Staged_Transfers;
	unsigned long volatile Queue_Index;
	unsigned long volatile Dequeue_Index;
	unsigned long          Capacity;

	BLOCK_DESCRIPTOR       Descriptor;

	mutex                  Mutex;
};
*/

unsigned long Calculate_Urgency_Staged_Transfer (STAGED_TRANSFER* staged_transfer, double bandwidth)
{
	return staged_transfer->Delivery_Time - ((staged_transfer->Capacity - staged_transfer->Offset) * bandwidth);
}

// Local (Send)
// local_gaia's mirrored block's on_commit event encodes changes                (see Commit_Block_Hermes)
// command is staged and added to hermes transmission queue, ordered by urgency (see Queue_Transmission_Hermes)
// determines network device with soonest delivery date                         (see Send_Loop_Hermes)
// encrypts packet to network device                                            (see Encrypt_Hermes_Actor)

void Employ_Hermes_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	if (context_data == NULL)
	{
		// WARNING
		return;
	}

	BLOCK_DESCRIPTOR descriptor;
	HERMES*          hermes;

	descriptor = Get_Descriptor_Concurrent_Block(calling_block);

	if (Write_Captured_Data_Block(context_data, (char*)hermes, ((char*)hermes) + sizeof(HERMES*)) != sizeof(HERMES*))
	{
		// WARNING
		return;
	}

	switch (descriptor.Type_Flag)
	{
		// Parent
		case BLOCK_TYPE_GAIA_LOCAL:
		{
			Add_Callback_Block_Event(calling_block->Gaia_Block->On_Employ, Employ_Hermes_Local_Actor, context_data);
			break;
		}

		// Ignore
		case BLOCK_TYPE_HERMES:
		case BLOCK_TYPE_IDLE:
		{
			break;
		}

		// Initialize Hermes in child blocks
		default:
		{
			DATA_BLOCK*         context_data;
			HERMES_LOCAL_ACTOR* hermes_local_actor;

			hermes_local_actor                     = new HERMES_LOCAL_ACTOR;

			hermes->Local_Actor_List->Previous     = hermes_local_actor;
			hermes->Local_Actor_List               = hermes_local_actor;
			hermes_local_actor->Previous           = NULL;
			hermes_local_actor->Next               = hermes->Local_Actor_List;

			hermes_local_actor->Hermes             = hermes;
			hermes_local_actor->Source_Block       = calling_block;
			hermes_local_actor->Last_Transfer_Data = NULL;

			if (descriptor.Options_Flag & BLOCK_OPTION_LATEST_COMMAND)
			{
				hermes_local_actor->Capacity = 1;
			}
			else
			{
				hermes_local_actor->Capacity = 100; // Make more dynamic
			}

			hermes_local_actor->Staged_Transfers   = new STAGED_TRANSFER[hermes_local_actor->Capacity];
			for (unsigned long index = 0; index < hermes_local_actor->Capacity; index++)
			{
				Initialize_Staged_Transfer(hermes_local_actor->Staged_Transfers + index);
			}

			hermes_local_actor->Queue_Index        = 0;
			hermes_local_actor->Dequeue_Index      = 0;

			hermes_local_actor->Descriptor         = descriptor;

			Capture_Data_Block(context_data);
			Write_Captured_Data_Block(context_data, &((char*)hermes_local_actor), &((char*)hermes_local_actor) + sizeof(HERMES_LOCAL_ACTOR*));

			Remove_Callback_Block_Event (calling_block->On_Commit  , Unexpected_Block_Event                   );
			Add_Callback_Block_Event    (calling_block->On_Commit  , Encode_Hermes_Local_Actor  , context_data);
			Remove_Callback_Block_Event (calling_block->On_Unemploy, Unexpected_Block_Event                   );
			Add_Callback_Block_Event    (calling_block->On_Unemploy, Unemploy_Hermes_Local_Actor, context_data);
			break;
		}
	}
}
void Unemploy_Hermes_Local_Actor (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	Remove_Callback_Block_Event (calling_block->On_Commit, Encode_Block_Hermes_Actor);
	if (Count_Callback_Block_Event() == 0)
	{
		Remove_Callback_Block_Event (calling_block->On_Commit, Unexpected_Block_Event);
	}
	Remove_Callback_Block_Event (calling_block->On_Unemploy, Unemploy_Local_Block_Hermes_Actor);

	HERMES_LOCAL_ACTOR* hermes_local_actor;

	Write_Captured_Data_Block(context_data, (char*)hermes_local_actor);

	hermes->Mutex.lock();

	if (hermes_local_actor->Previous != NULL)
	{
		hermes_local_actor->Previous->Next = hermes_local_actor->Next;
	}
	if (hermes_local_actor->Next != NULL)
	{
		hermes_local_actor->Next->Previous = hermes_local_actor->Previous;
	}
	if (hermes_local_actor->Previous == NULL &&
	    hermes_local_actor->Next     == NULL &&
	    hermes->Local_Actor_List     == hermes_local_actor) // probably redundant
	{
		hermes->Local_Actor_List = NULL;
	}

	hermes->Mutex.unlock();

	for (unsigned long index = 0; index < hermes_local_actor->Capacity; index++)
	{
		Uninitialize_Staged_Transfer(hermes_local_actor->Staged_Transfers + index);
	}
	delete [] hermes_local_actor->Staged_Transfers;
	if (hermes_local_actor->Last_Transfer_Data != NULL)
	{
		Release_Data_Block(hermes_local_actor->Last_Transfer_Data);
	}
	Release_Data_Block(context_data);
	delete hermes_local_actor;
}

void Encode_Hermes_Local_Actor   (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	BLOCK_DESCRIPTOR    descriptor;
	HERMES_LOCAL_ACTOR* hermes_local_actor;

	Read_Captured_Data_Block(context_data, (char**)hermes_local_actor);
	descriptor   = Get_Descriptor_Concurrent_Block(calling_block);

	if (descriptor->Permitted_Latency < BLOCK_LATENCY_INFINITE)
	{
		/*
		 * #define BLOCK_OPTION_PARALLEL_COMMANDS   0b00000000000000000000000000001000 //  4 - commands are produced/applied concurrently in parallel threads, if unset commands are produced/applied sequentially in a single thread
		 * #define BLOCK_OPTION_ORDER_COMMANDS      0b00000000000000000000000000010000 //  5 - commands are produced/applied in a defined order, if unset commands are produced/applied as they become available
		 * #define BLOCK_OPTION_LATEST_COMMAND      0b00000000000000000000000001000000 //  7 - only the latest commit data is transferred, if unset all commited blocks are sent
		 * #define BLOCK_OPTION_COMPRESS_COMMANDS   0b00000000000000000000000010000000 //  8 - compression is considered based on BLOCK_TYPE for transferred commands, if unset compression is not considered
		 * #define BLOCK_OPTION_LOSSY_COMMANDS
		 */

		unsigned long estimated_latency;        // processing time
		unsigned long estimated_yield_capacity; // estimated byte size of command
		unsigned long permitted_latency;

		estimated_processing_time = command_bandwidth * block_capacity;
		estimated_yield_capacity  = command_yield_ratio * block_capacity;

		if (descriptor->Options_Flag & BLOCK_OPTION_COMPRESS_COMMANDS)
		{
			estimated_processing_time  = compressor[descriptor.Type_Flag].Bandwidth   * block_capacity;
			estimated_yield_capacity   = compressor[descriptor.Type_Flag].Yield_Ratio * block_capacity;
		}
		if (descriptor->Options_Flag & BLOCK_OPTION_LOSSY_COMMANDS)
		{
			estimated_processing_time
		}

		// blah blah blah

		if (descriptor->Options_Flag & BLOCK_OPTION_LATEST_COMMAND)
		{

		}
		else
		{
			if (descriptor->Options_Flag & BLOCK_OPTION_PARALLEL_COMMANDS)
			{

			}
		}


	} // END if (descriptor->Permitted_Latency < BLOCK_LATENCY_INFINITE)
}
void Local_Loop_Hermes_Actor     (CONCURRENT_BLOCK* const calling_block, DATA_BLOCK* const context_data)
{
	NETWORK_DEVICE* network_device_iterator;
	NETWORK_DEVICE* network_device_terminator;
	unsigned long*  estimated_delivery_array;
	unsigned long*  estimated_delivery_iterator;
	unsigned long*  estimated_delivery_terminator;
	unsigned long   network_device_count;
	unsigned long   next_delivery_time;
	unsigned long   next_send_time;
	unsigned long   current_time;
	unsigned long   send_count;

	Hermes->Mutex.lock();

	network_device_array = Hermes->Network_Device_Iterator;
	network_device_count = (unsigned long)(Hermes->Network_Device_Terminator - network_device_iterator);

	if (network_device_count == 0)
	{
		Hermes->Mutex.unlock();
		return;
	}

	estimated_delivery_array = new unsigned long[network_device_count];
	current_time             = Get_UTC_Time_Milliseconds();
	next_send_time           = current_time + 100;
	next_delivery_index      = 0;
	send_count               = 0;

	// Wait to avoid network congestion
	for (unsigned long index = 0; index < network_device_count; index++)
	{
		if (network_device_array[index].Send_Time < next_send_time)
		{
			next_send_time = network_device_array[index].Send_Time;
		}
	}
	while (next_send_time > current_time)
	{
		wait (next_send_time - current_time);
		current_time = Get_UTC_Time_Milliseconds();
	}

	// Initialize network(s) data
	for (int index = 0; index < network_device_count; index++)
	{
		if (network_device_array[index].Send_Time <= current_time)
		{
			network_device_array[index].Send_Time = current_time;
			send_count++;
		}
		else if (network_device_array[index].Send_Time < next_send_time)
		{
			next_send_time = network_device_array[index].Send_Time;
		}

		estimated_delivery[index] = (unsigned long)(network_device_array[index].Send_Time + network_device_array[i].Latency + ((double)network_device_array[i].MTU / network_device_array[i].Bandwidth));
	}

	while (send_count > 0);
	{
		for (int i = 0; i < network_device_count; i++)
		{
			if (estimated_delivery[i] < estimated_delivery[next_delivery_index])
			{
				next_delivery_index = i;
			}
		}
		estimated_delivery[next_delivery_index] -= ((double)network_device[i].MTU / network_device[i].Bandwidth) + 1;
		if (network_device[next_delivery_index].Send_Time == current_time)
		{
			// SUDO
			HERMES_LOCAL_ACTOR* local_actor_iterator;

			Hermes->Mutex.lock();

			local_actor_iterator = Hermes->Local_Actor_List;
			while (local_actor_iterator != NULL)
			{
				local_actor_iterator =
			}
			most_urgent_block_encoding = NULL;
			most_urgent                = BLOCK_LATENCY_NEVER;
			foreach (block_encoding in hermes)
			{
				urgency = delivery_time - (bandwidth * remaining bytes)
				if (urgency < most_urgent)
				{
					most_urgent_block_encoding = block_encoding;
					most_urgent                = urgency;
				}
			}
			if (most_urgent_block_encoding != NULL)
			{
				encrypt (see below)
				sub
			}
			// END SUDO

			// Encrypt
			CONCURRENT_BLOCK* key_block;
			DATA_BLOCK*       key_data;
			const char*       key_iterator;
			char*             payload_iterator;
			DATA_BLOCK*       encrypted_data;
			char*             encrypted_iterator;

			key_block      = Find_Unused_Key(); // to do; at worst could sequentially search calling_block's local_gaia
			key_data       = Capture_Latest_Concurrent_Block(key_block);
			encrypted_data = Construct_Data_Block(sizeof(MTU));
			Access_Captured_Data_Block(key_data,       (char**)&key_iterator      );
			Access_Captured_Data_Block(payload_data  , (char**)&payload_iterator  );
			Access_Captured_Data_Block(encrypted_data, (char**)&encrypted_iterator);

			while (payload_iterator < payload_data->Terminator)
			{
				*encrypted_data = *payload_iterator ^ *key_iterator;
				key_iterator++;
				payload_iterator++;
				encrypted_data++;
			}

			*((unsigned int*)encrypted_iterator) = key_block->Gaia_Index;
			encrypted_iterator                  += sizeof(unsigned int);
			*((unsigned int*)encrypted_iterator) = Checksum(payload_data_block->Iterator, payload_data_block->Terminator);
			encrypted_iterator                  += sizeof(unsigned int);
			copy(payload_data_block->Iterator, payload_data_block->Terminator, encrypted_iterator);

			// network_device[next_delivery_index]
			Queue_Sequential_Block_Event(Sequential_Send_Event, Send_Callback, calling_block, encrypted_data);
			// ENd encrypt

			send_count--;
		}
	}

	Hermes->Mutex.unlock();

	delete [] estimated_delivery;

	Queue_Latest_Block_Event(Hermes->Latest_Send_Event);
}
