#ifndef GAIA_H_
#define GAIA_H_

#define GAIA_BOUNDS_CHECKING

#include <Gaia/CONCURRENT_BLOCK.h> // Includes BLOCK_EVENT and <mutex>

/* GAIA
 * Defines a concurrent data structure composed of CONCURRENT_BLOCKs
 * sizeof(GAIA) == 16 bytes (64 bit machine, bit aligned on any machine)
 */
struct GAIA
{
	const CONCURRENT_BLOCK* const* volatile Block_Array;
	const CONCURRENT_BLOCK* const* volatile Block_Terminator;
	const CONCURRENT_BLOCK*                 Root_Block;
	mutex                                   Mutex;
};

void              Initialize_Gaia         (GAIA* const gaia, const CONCURRENT_BLOCK* const source_block);
void              Uninitialize_Gaia       (GAIA* const gaia);

GAIA*             Construct_Gaia          (const CONCURRENT_BLOCK* const source_block);
void              Deconstruct_Gaia        (GAIA* const gaia);

unsigned long     Get_Block_Capacity_Gaia (GAIA* const gaia);
void              Set_Block_Capacity_Gaia (GAIA* const gaia, const unsigned long new_capacity, const bool allow_shrink);

CONCURRENT_BLOCK* Get_Block_Gaia          (GAIA* const gaia, const unsigned long block_index);


// Aliases
inline void              Gaia_Initialize         (GAIA* const gaia, const CONCURRENT_BLOCK* const source_block);
inline void              Gaia_Uninitialize       (GAIA* const gaia);
inline void              Gaia_Delete             (GAIA* const gaia);
inline void              Delete_Gaia             (GAIA* const gaia);

inline GAIA*             Gaia_Construct          (const CONCURRENT_BLOCK* const source_blockt);
inline void              Gaia_Deconstruct        (GAIA* const gaia);

inline unsigned long     Gaia_Get_Block_Capacity (GAIA* const gaia);
inline void              Gaia_Set_Block_Capacity (GAIA* const gaia, const unsigned long destination_block_capacity, const bool allow_shrink);

inline CONCURRENT_BLOCK* Gaia_Get_Block          (GAIA* const gaia, const unsigned long block_index);

#endif // GAIA_H_
