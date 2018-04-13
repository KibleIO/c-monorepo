#include "GAIA.h"

void              Initialize_Gaia         (GAIA* const gaia, const CONCURRENT_BLOCK* const source_block)
{
	CONCURRENT_BLOCK** block_array;

	block_array  = new CONCURRENT_BLOCK* [1];
	*block_array = Construct_Concurrent_Block(source_block, 0);

	Employ_Concurrent_Block(*block_array, &DEFAULT_GAIA_ROOT_BLOCK_DESCRIPTOR);

	gaia->Block_Array      = (const CONCURRENT_BLOCK* const*)(block_array + 0);
	gaia->Block_Terminator = (const CONCURRENT_BLOCK* const*)(block_array + 1);
	gaia->Source_Block     = source_block;
	gaia->Root_Block       = *block_array;
}
void              Uninitialize_Gaia       (GAIA* const gaia)
{
	Set_Block_Capacity_Gaia(gaia, 0, true);

	delete [] gaia->Block_Array;
}

GAIA*             Construct_Gaia          (const CONCURRENT_BLOCK* const source_block)
{
	GAIA* const gaia = new GAIA;
	Initialize_Gaia(gaia, source_block);
	return gaia;
}
void              Deconstruct_Gaia        (GAIA* const gaia)
{
	Uninitialize_Gaia(gaia);
	delete gaia;
}

unsigned long     Get_Block_Capacity_Gaia (GAIA* const gaia)
{
	unsigned long block_capacity;

	gaia->Mutex.lock();

	block_capacity = (unsigned long)(gaia->Block_Terminator - gaia->Block_Array);

	gaia->Mutex.unlock();

	return block_capacity;
}
void              Set_Block_Capacity_Gaia (GAIA* const gaia, const unsigned long new_block_capacity, const bool allow_shrink)
{
	const CONCURRENT_BLOCK* const* current_block_array;
	const CONCURRENT_BLOCK* const* current_terminator_array;
	const CONCURRENT_BLOCK**       new_block_array;
	unsigned long                  current_block_capacity;

	new_block_array = new const CONCURRENT_BLOCK*[new_block_capacity]; // [TO-DO] Check for out of memory exception

	gaia->Mutex.lock();

	current_block_array    = gaia->Block_Array; // Prevents redundant memory reads from volatile hint
	current_block_capacity = (unsigned long)(current_block_array - gaia->Block_Terminator);

	if      (new_block_capacity > current_block_capacity)
	{
		copy(current_block_array, current_block_array + current_block_capacity, new_block_array);
		for (unsigned long block_index = current_block_capacity - 1; block_index < new_block_capacity; block_index++)
		{
			new_block_array[block_index] = Construct_Concurrent_Block(gaia->Source_Block, block_index);
		}

		gaia->Block_Array      = (const CONCURRENT_BLOCK* const*)(new_block_array                     );
		gaia->Block_Terminator = (const CONCURRENT_BLOCK* const*)(new_block_array + new_block_capacity);
	}
	else if (allow_shrink && new_block_capacity < current_block_capacity)
	{
		copy(current_block_array, current_block_array + new_block_capacity, new_block_array);
		for (unsigned long block_index = new_block_capacity - 1; block_index < current_block_capacity; block_index++)
		{
			Deconstruct_Concurrent_Block((CONCURRENT_BLOCK*)current_block_array[block_index]);
		}

		gaia->Block_Array      = (const CONCURRENT_BLOCK* const*)(new_block_array                     );
		gaia->Block_Terminator = (const CONCURRENT_BLOCK* const*)(new_block_array + new_block_capacity);
	}
	else // if (new_block_capacity == current_block_capacity || (!allow_shrink && new_block_capacity < current_block_capacity))
	{
		current_block_array = new_block_array;
	}

	gaia->Mutex.unlock();

	delete [] current_block_array;
}

CONCURRENT_BLOCK* Get_Block_Gaia          (GAIA* const gaia, const unsigned long block_index)
{
	const CONCURRENT_BLOCK* const* block_iterator;

	gaia->Mutex.lock();

	block_iterator = gaia->Block_Array + block_index;

	#ifdef GAIA_BOUNDS_CHECKING
	if (block_iterator >= gaia->Block_Terminator)
	{
		ostringstream message;
		message << "@GAIA::Get_Block_Gaia(" <<
			"gaia"        << ": " << (void*)gaia  << ", " <<
			"block_index" << ": " <<  block_index << ") " <<
			"Parameter 'block_index' does not reference an allocated block within 'gaia'" << " - " <<
			"Returning NULL";
		Warning(message.str());

		block_iterator = NULL;
	}
	#endif // GAIA_BOUNDS_CHECKING

	gaia->Mutex.unlock();

	return (CONCURRENT_BLOCK*)*block_iterator;
}


// Aliases
inline void              Gaia_Initialize         (GAIA* const gaia, const CONCURRENT_BLOCK* const source_block)
{
	Initialize_Gaia(gaia, source_block);
}
inline void              Gaia_Uninitialize       (GAIA* const gaia)
{
	Uninitialize_Gaia(gaia);
}
inline void              Gaia_Delete             (GAIA* const gaia)
{
	Uninitialize_Gaia(gaia);
}
inline void              Delete_Gaia             (GAIA* const gaia)
{
	Uninitialize_Gaia(gaia);
}

inline GAIA*             Gaia_Construct          (const CONCURRENT_BLOCK* const source_block)
{
	return Construct_Gaia(source_block);
}
inline void              Gaia_Deconstruct        (GAIA* const gaia)
{
	Deconstruct_Gaia(gaia);
}

inline unsigned long     Gaia_Get_BLock_Capacity (GAIA* const gaia)
{
	return Get_Block_Capacity_Gaia(gaia);
}

inline CONCURRENT_BLOCK* Gaia_Get_Block          (GAIA* const gaia, const unsigned long block_index)
{
	return Get_Block_Gaia(gaia, block_index);
}
