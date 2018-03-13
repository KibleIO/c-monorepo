#include "GAIA_COMMAND.h"

unsigned long Decode_Unsigned_Integer(char** iterator_pointer, char* terminator)
{
	unsigned long integer;
	char*         iterator;

	iterator = *iterator_pointer;

	#ifdef GAIA_COMMAND_BOUNDS_CHECKING
	if (iterator >= terminator)
	{
		*iterator_pointer = terminator;
		return 0;
	}
	#endif

	switch (__builtin_clz(*iterator)) // Returns the number of leading 0-bits in x, starting at the most significant bit position. If x is 0, the result is undefined.
	{
		case 0:  // 1|#######  7 bits            128 Max
		{
			integer   = (unsigned long)(*((unsigned char)*iterator) & 0x7F);
			iterator += CHAR_OCTET_SIZE;
			break;
		}
		case 1:  // 01|###### 14 bits         16,384 Max (+1 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + SHORT_OCTET_SIZE > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			integer   = (unsigned long)(*((unsigned short)*iterator) & 0x3FFF);
			iterator += SHORT_OCTET_SIZE;
			break;
		}
		case 2:  // 001|##### 21 bits      2,097,152 Max (+2 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + (CHAR_OCTET_SIZE + SHORT_OCTET_SIZE) > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			integer   = ((unsigned long)(*((unsigned char)*iterator) & 0x1F)) << SHORT_BIT_SIZE;
			iterator += CHAR_OCTET_SIZE;
			integer  += ((unsigned long)*((unsigned short)*iterator));
			iterator += SHORT_OCTET_SIZE;
			break;
		}
		case 3:  // 0001|#### 28 bits    268,435,456 Max (+3 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + INT_OCTET_SIZE > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			integer   = (unsigned long)(*((unsigned int)*iterator) & 0x0FFFFFFF);000
			iterator += INT_OCTET_SIZE;
			break;
		}
		case 4:  // 00001|### 35 bits 34,359,738,368 Max (+4 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + (CHAR_OCTET_SIZE + INT_OCTET_SIZE) > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			integer   = ((unsigned long)(*((unsigned char)*iterator) & 0x07)) << INT_BIT_SIZE;
			iterator += CHAR_OCTET_SIZE;
			integer  += ((unsigned long)*((unsigned int)*iterator));
			iterator += INT_OCTET_SIZE;
			break;
		}
		case 5:  // 000001|## 42 bits           2^42 Max (+5 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + SHORT_OCTET_SIZE + INT_OCTET_SIZE) > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			integer   = ((unsigned long)(*((unsigned short)*iterator) & 0x03FF)) << INT_BIT_SIZE;
			iterator += SHORT_OCTET_SIZE;
			integer  += ((unsigned long)*((unsigned int)*iterator));
			iterator += INT_OCTET_SIZE;
			break;
		}
		case 6:  // 0000001|# 49 bits           2^49 Max (+6 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + (CHAR_OCTET_SIZE + INT_OCTET_SIZE + SHORT_OCTET_SIZE) > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			integer   = ((unsigned long)(*((unsigned char)*iterator) & 0x01)) << (INT_BIT_SIZE + SHORT_BIT_SIZE);
			iterator += CHAR_OCTET_SIZE;
			integer  += ((unsigned long)*((unsigned int)*iterator)) << SHORT_BIT_SIZE;
			iterator += INT_OCTET_SIZE;
			integer  += ((unsigned long)*((unsigned short)*iterator));
			iterator += SHORT_OCTET_SIZE;
			break;
		}
		case 7:  // 00000001| 56 bits           2^56 Max (+7 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + LONG_OCTET_SIZE > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			integer   = *((unsigned long)*iterator) & 0x00FFFFFFFFFFFFFF;
			iterator += LONG_OCTET_SIZE;
			break;
		}
		default: // 00000000| 64 bits           2^64 Max (+8 octet)
		{
			#ifdef GAIA_COMMAND_BOUNDS_CHECKING
			if (iterator + (CHAR_OCTET_SIZE + LONG_OCTET_SIZE) > terminator)
			{
				*iterator_pointer = terminator;
				return 0;
			}
			#endif
			iterator += CHAR_OCTET_SIZE;
			integer   = *((unsigned long)*iterator);
			iterator += LONG_OCTET_SIZE;
			break;
		}
	}
	*iterator_pointer = iterator;

	return integer;
}
void          Encode_Unsigned_Integer(char** iterator_pointer, char* terminator, unsigned long integer)
{
	char* iterator = *iterator_pointer;

	#ifdef GAIA_COMMAND_BOUNDS_CHECKING
	if (iterator >= terminator)
	{
		*iterator_pointer = terminator;
		return;
	}
	#endif

	if      (integer < 0x0000000000000080) // 1|#######  7 bits            128 Max
	{
		*iterator = 0x80 | integer;
		iterator += CHAR_OCTET_SIZE;
	}
	else if (integer < 0x0000000000004000) // 01|###### 14 bits         16,384 Max (+1 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + SHORT_OCTET_SIZE > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned short)*iterator) = (unsigned short)(0x0000000000004000 | integer);
		iterator                    += SHORT_OCTET_SIZE;
	}
	else if (integer < 0x0000000000200000) // 001|##### 21 bits      2,097,152 Max (+2 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + (SHORT_OCTET_SIZE + CHAR_OCTET_SIZE) > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned short)*iterator) = (unsigned short)((0x0000000000200000 | integer) >> CHAR_BIT_SIZE);
		iterator                    += SHORT_OCTET_SIZE;
		*((unsigned char )*iterator) = (unsigned char )integer;
		iterator                    += CHAR_OCTET_SIZE;
	}
	else if (integer < 0x0000000010000000) // 0001|#### 28 bits    268,435,456 Max (+3 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + (INT_OCTET_SIZE) > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned int)*iterator) = 0x10000000 | (unsigned int)integer;
		iterator                  += INT_OCTET_SIZE;
	}
	else if (integer < 0x0000000800000000) // 00001|### 35 bits 34,359,738,368 Max (+4 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + (INT_OCTET_SIZE + CHAR_OCTET_SIZE) > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned int )*iterator) = (unsigned int )((0x0000000800000000 | integer) >> CHAR_BIT_SIZE);
		iterator                   += INT_OCTET_SIZE;
		*((unsigned char)*iterator) = (unsigned char)integer;
		iterator                   += CHAR_OCTET_SIZE;
	}
	else if (integer < 0x0000040000000000) // 000001|## 42 bits           2^42 Max (+5 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + (INT_OCTET_SIZE + SHORT_OCTET_SIZE) > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned int  )*iterator) = (unsigned int  )((0x0000040000000000 | integer) >> SHORT_BIT_SIZE);
		iterator                    += INT_OCTET_SIZE;
		*((unsigned short)*iterator) = (unsigned short)integer;
		iterator                    += SHORT_OCTET_SIZE;
	}
	else if (integer < 0x0002000000000000) // 0000001|# 49 bits           2^49 Max (+6 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + (INT_OCTET_SIZE + SHORT_OCTET_SIZE + CHAR_OCTET_SIZE) > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned int  )*iterator) = (unsigned int  )((0x0002000000000000 | integer) >> (SHORT_BIT_SIZE + CHAR_BIT_SIZE));
		iterator                    += INT_OCTET_SIZE;
		*((unsigned short)*iterator) = (unsigned short)(integer >> CHAR_OCTET_SIZE);
		iterator                    += SHORT_OCTET_SIZE;
		*((unsigned char )*iterator) = (unsigned char )integer;
		iterator                    += CHAR_OCTET_SIZE;
	}
	else if (integer < 0x0100000000000000) // 00000001| 56 bits           2^56 Max (+7 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + (INT_OCTET_SIZE + CHAR_OCTET_SIZE) > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned long)*iterator) = 0x0100000000000000 | integer;
		iterator                   += LONG_OCTET_SIZE;
	}
	else                                   // 00000000| 64 bits           2^64 Max (+8 octet)
	{
		#ifdef GAIA_COMMAND_BOUNDS_CHECKING
		if (iterator + (LONG_OCTET_SIZE) > terminator)
		{
			*iterator_pointer = terminator;
			return;
		}
		#endif
		*((unsigned char)*iterator) = 0x00;
		iterator                   += CHAR_OCTET_SIZE;
		*((unsigned long)*iterator) = integer;
		iterator                   += LONG_OCTET_SIZE;
	}
	*iterator_pointer = iterator;
}

/* SIGNED INTEGER - highest bit is 1 for positive and 0 for negative
 * ±|0|000000  6 bits            128 Max
 * ±|10|00000 13 bits         16,384 Max (+1 octet)
 * ±|110|0000 20 bits      1,048,576 Max (+2 octet)
 * ±|1110|000 27 bits    134,217,728 Max (+3 octet)
 * ±|11110|00 34 bits 17,179,869,184 Max (+4 octet)
 * ±|111110|0 41 bits           2^41 Max (+5 octet)
 * ±|1111110| 48 bits           2^48 Max (+6 octet)
 * ±|1111111| 56 bits           2^56 Max (+7 octet)
 */
long         Decode_Signed_Integer  (char** iterator_pointer, char* terminator)
{
	// ERROR
	cout << "@GAIA_PROTOCOL::Decode_Signed_Integer(" <<
	"iterator_pointer" << ": " << (void*)iterator_pointer << ", " <<
	"terminator"       << ": " << (void*)terminator       << ") " <<
	"Unfinished" << endl;
	return 0;
}
void         Encode_Signed_Integer  (char** iterator_pointer, char* terminator, long integer)
{
	// ERROR
	cout << "@GAIA_PROTOCOL::Encode_Signed_Integer(" <<
	"iterator_pointer" << ": " << (void*)iterator_pointer << ", " <<
	"terminator"       << ": " << (void*)terminator       << ", " <<
	"integer"          << ": " << integer                 << ") " <<
	"Unfinished" << endl;
	return;
}

/* GAIA COMMANDS
 *
 * OFFSET; length; increments current index in the current DATA_BLOCK
 * 10|######  6 bits             64 Max
 * 110|##### 13 bits          8,192 Max (+1 octet)
 * 1110|#### 20 bits      1,048,576 Max (+2 octet)
 * 11110|### 27 bits    134,217,728 Max (+3 octet)
 * 111110|## 34 bits 17,179,869,184 Max (+4 octet)
 * 1111110|# 41 bits           2^41 Max (+5 octet)
 * 11111110| 48 bits           2^48 Max (+6 octet)
 * 11111111| Expecte; length of following octets to be copied to the current index; increments current indexd unsigned number
 *
 * DAMAGE; length of following octets to be copied to the current index; increments current index
 * 010|#####  5 bits             32 Max
 * 0110|#### 12 bits          4,096 Max (+1 octet)
 * 01110|### 19 bits        524,288 Max (+2 octet)
 * 011110|## 26 bits     67,108,864 Max (+3 octet)
 * 0111110|# 33 bits  8,589,934,592 Max (+4 octet)
 * 01111110| 40 bits           2^40 Max (+5 octet)
 * 01111111| Expected unsigned number
 *
 * INTERNAL COPY; length until copy terminator; increments current index
 * 0010|####  4 bits            16 Max
 * 00110|### 11 bits         2,048 Max (+1 octet)
 * 001110|## 18 bits       262,144 Max (+2 octet)
 * 0011110|# 25 bits    33,554,432 Max (+3 octet)
 * 00111110| 32 bits 4,294,967,296 Max (+4 octet)
 * 00111111| Expected unsigned number
 * - Followed by expected signed number; offset of copy source index from current index
 *
 * EXTERNAL COPY; length until copy terminator; increments current index
 * 00010|###  3 bits            16 Max
 * 000110|## 10 bits         2,048 Max (+1 octet)
 * 0001110|# 17 bits       262,144 Max (+2 octet)
 * 00011110| 24 bits    33,554,432 Max (+3 octet)
 * 00011111| Expected unsigned number
 * - Followed by expected unsigned number; copy source sector index
 * - Followed by expected unsigned number; copy source index in different index
 *
 * DIFFERENCE
 * 00001000|  expands 0 bits  -127 to 127  1:1 (no expansion advantage, but may compress better)
 * 00001001|  expands 1 bit    -63 to  63  8:7
 * 00001010|  expands 2 bits   -31 to  31  4:3
 * 00001011|  expands 3 bits   -13 to  13  8:5
 * 00001100|  expands 4 bits    -7 to   7  2:1 (every two bytes encoded in one; ideal expansion)
 * 00001101|  expands 5 bits    -3 to   3  8:3
 * 00001110|  expands 6 bits    -1 to   1  4:1
 * 00001111|  expands 7 bits     0 to   1  8:1 (adds 1 or 0; almost useless)
 * - Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
 *
 * 00000111| SET BLOCK [ UNSIGNED INTEGER block_index ]
 * SUDO:
 *    current_block = Get_Block_Gaia(Gaia, block_index);
 *
 * 00000110| CAPTURE DATA
 * SUDO:
 *    if (current_data != NULL) Release_Data_Block(current_data);
 *    current_data = Capture_Idle_Concurrent_Block(current_block)
 *
 * 00000101| NEW DATA [ UNSIGNED INTEGER byte_length ]
 * SUDO:
 *    if (current_data != NULL) Release_Data_Block(current_data);
 *    current_data = Construct_Data_Block(byte_length);
 *
 * 00000100| ADD DATA
 * SUDO:
 *    Add_Concurrent_Block(current_block, current_data);
 *
 * 00000011| REMOVE DATA
 * SUDO:
 *    Remove_Concurrent_Block(current_block, current_data);
 *
 * 00000010| COMMIT DATA
 * SUDO:
 *    Commit_Concurrent_Block(current_block, current_data);
 *
 * 00000001| FREE
 * SUDO:
 *
 * 00000000| EXTENDED COMMAND [ UNSIGNED INTEGER command_id ]
 * SUDO:
 *    switch (command_id) ...
 *
 *
 bool               Remove_Concurrent_Block           (CONCURRENT_BLOCK*, DATA_BLOCK* remove_data);
 *
 * ADD DATA - Constructs a DATA_BLOCK of the provided byte length, sets the current DATA_BLOCK to it, and adds it to the current CONCURRENT_BLOCK
 * 00000101| - Followed by expected unsigned integer, Gaia index at which to add the current segment
 */

/* From "GAIA_COMMAND.h"
struct COMMAND_EXECUTION_CONTEXT
{
	GAIA*         Gaia;
	SEGMENT*      Segment;
	char*         Iterator;
	unsigned long Gaia_Index;
	unsigned long Offset;
}
*/

inline void Execute_Offset_Command       (COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long offset)
{
	command_execution_context->Offset += offset;
}
inline void Execute_Damage_Command       (COMMAND_EXECUTION_CONTEXT* command_execution_context, char* damage_iterator, char* damage_terminator)
{
	#ifdef GAIA_COMMAND_BOUNDS_CHECKING
	register unsigned long write_length = Write_Captured_Segment(
		command_execution_context->Segment,
		damage_iterator,
		damage_terminator,
		command_execution_context->Offset);
	if (write_length != (unsigned long)(damage_terminator - damage_iterator))
	{
		// ERROR
		return;
	}
	command_execution_context->Offset += write_length;
	#else
	command_execution_context->Offset += Write_Captured_Segment(command_execution_context->Segment, damage_iterator, damage_terminator, command_execution_context->Offset);
	#endif
}
inline void Execute_Internal_Copy_Command(COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long copy_length, unsigned long source_index)
{
	char** read_iterator_pointer;
	char*  read_terminator;

	read_terminator = Read_Captured_Segment(command_execution_context->Segment, read_iterator_pointer, source_index, source_index + copy_length);

	#ifdef GAIA_COMMAND_BOUNDS_CHECKING
	if (copy_length != (unsigned long)(read_terminator - *read_iterator_pointer))
	{
		// ERROR
		return;
	}
	#endif

	Execute_Damage_Command(command_execution_context, *read_iterator_pointer, read_terminator);
}
inline void Execute_External_Copy_Command(COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long copy_length, unsigned long source_gaia_index, unsigned long source_index)
{
	SEGEMENT* external_segment;
	char**    read_iterator_pointer;
	char*     read_terminator;

	external_segment = Capture_Read_Gaia(command_execution_context->Gaia, source_gaia_index);

	#ifdef GAIA_COMMAND_BOUNDS_CHECKING
	if (external_segment == NULL)
	{
		// ERROR
		return;
	}
	#endif

	read_terminator = Read_Captured_Segment(external_segment, read_iterator_pointer, source_index, source_index + copy_length);

	#ifdef GAIA_COMMAND_BOUNDS_CHECKING
	if (copy_length != (unsigned long)(read_terminator - *read_iterator_pointer))
	{
		Release_Segment(external_segment);
		// ERROR
		return;
	}
	#endif

	Execute_Damage_Command(command_execution_context, *read_iterator_pointer, read_terminator);
	Release_Segment(external_segment);
}
inline void Execute_Difference_Command   (COMMAND_EXECUTION_CONTEXT* command_execution_context, signed char* difference_iterator, signed char* difference_terminator)
{
	char* iterator;

	iterator = command_execution_context->Iterator;
	#ifdef GAIA_COMMAND_BOUNDS_CHECKING
	if (iterator + (difference_terminator - difference_iterator) >= command_execution_context->Segment->Terminator)
	{
		// ERROR
		return;
	}
	#endif

	while (difference_iterator < difference_terminator)
	{
		*iterator += *difference_iterator;

		iterator           ++;
		difference_iterator++;
	}
}
inline void Execute_New_Command          (COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long length)
{

}
inline void Execute_Add_Command          (COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long index)
{
	Add_Gaia(command_execution_context->Gaia, index, command_execution_context->Segment);
}
inline void Execute_Remove_Command       (COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long index)
{
	if (!Remove_Gaia(command_execution_context->Gaia, index, command_execution_context->Segment)
	{
		// ERROR
	}
}
inline void Execute_Resize_Command       (COMMAND_EXECUTION_CONTEXT* command_execution_context, long length)
{
	if (length > 0)
	{
		Append_Write_Captured_Segment (command_execution_context->Segment, command_execution_context->Offset, (unsigned long)length);
	}
	else if (length != 0)
	{
		Delete_Write_Captured_Segment (command_execution_context->Segment, command_execution_context->Offset, (unsigned long)-length);
	}
	else // (length == 0)
	{
		// ERROR
	}
}
inline void Execute_Capture_Read_Command (COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long index)
{
	if (command_execution_context->Segment != NULL)
	{
		Execute_Release_Command(command_execution_context, command_execution_context->Gaia_Index);
		if (command_execution_context->Segment != NULL)
		{
			// ERROR
			return;
		}
	}

	command_execution_context->Segment = Capture_Read_Gaia(command_execution_context->Gaia, segment_index);

	if (command_execution_context->Segment == NULL)
	{
		// ERROR
	}
}
inline void Execute_Capture_Write_Command(COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long index)
{
	if (command_execution_context->Segment != NULL)
	{
		Execute_Release_Command(command_execution_context, command_execution_context->Gaia_Index);
		if (command_execution_context->Segment != NULL)
		{
			// ERROR
			return;
		}
	}

	command_execution_context->Segment = Capture_Write_Gaia(command_execution_context->Gaia, segment_index);

	if (command_execution_context->Segment == NULL)
	{
		// ERROR
	}
}
inline void Execute_Release_Command      (COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long index)
{
	if (command_execution_context->Segment == NULL)
	{
		// ERROR
		return;
	}

	Release_Gaia(command_execution_context->Gaia, segment_index);
	command_execution_context->Segment = NULL;
}
inline void Execute_Commit_Command       (COMMAND_EXECUTION_CONTEXT* command_execution_context, unsigned long index)
{
	Commit_Gaia(command_execution_context->Gaia, command_execution_context->Segment, segment_index);
}

/* Execute_Encoded_Commands
 *
 * If the contents from "iterator" to "terminator" contain valid GAIA COMMAND(s), the GAIA COMMAND(s) are applied and the applied SEGMENT returned.
 * Else if any part of the contents from "iterator" to "terminator" contain invalid GAIA COMMAND(s), no GAIA COMMAND(s) are applied to the target and NULL is returned.
 */
void Execute_Encoded_Commands(COMMAND_EXECUTION_CONTEXT* command_execution_context)
{
	char*         iterator;
	unsigned long length;
	unsigned long index;

	iterator = command_execution_context->Iterator;

	while (iterator < command_execution_context->Terminator)
	{
		switch (*iterator)
		{
			case 0b00001000: // DIFFERENCE; Expands 0 bits  -127 to 127  1:1  (no expansion advantage, but may compress better)
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, command_execution_context->Terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00001001: // DIFFERENCE; Expands 1 bit    -63 to  63  8:7
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00001010: // DIFFERENCE; Expands 2 bits   -31 to  31  4:3
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00001011: // DIFFERENCE; Expands 3 bits   -13 to  13  8:5
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00001100: // DIFFERENCE; Expands 4 bits    -7 to   7  2:1  (every two bytes encoded in one; ideal expansion)
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00001101: // DIFFERENCE; Expands 5 bits    -3 to   3  8:3
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00001110: // DIFFERENCE; Expands 6 bits    -1 to   1  4:1
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00001111: // DIFFERENCE; Expands 7 bits     0 to   1  8:1  (adds 1 or 0; almost useless)
			{
				iterator++;
				// Followed by expected signed number; length of following signed octets to be expanded, then added to the current index; increments current index
				length = Decode_Unsigned_Integer(&iterator, terminator);
				Execute_Difference_Command(command_execution_context, iterator, iterator + length);
				break;
			}
			case 0b00000111: // NEW GAIA - Allocates a GAIA of provided size, deleting or adding new VOLATILE_SEGMENTs to the tail.
			{
				iterator++;

				// Followed by expected unsigned integer, length of SEGMENT to be constructed; sets constructed segment to current
				length = Decode_Unsigned_Integer(&iterator, terminator);

				Execute_New_Command(command_execution_context, length);
				break;
			}
			case 0b00000110: // NEW SEGMENT - Creates a new SEGMENT of the length provided and is set to the current index
			{
				iterator++;

				// Followed by expected unsigned integer, Gaia index at which to add the current segment
				index = Decode_Unsigned_Integer(&iterator, terminator);

				Execute_Add_Command(command_execution_context, index);
				break;
			}
			case 0b00000101: // ADD SEGMENT - Adds the current SEGMENT to the VOLATILE_SEGMENT loop at the provided index
			{
				iterator++;

				// Followed by expected unsigned integer, Gaia index at which to remove the current segment
				index = Decode_Unsigned_Integer(&iterator, terminator);

				Execute_Remove_Command(command_execution_context, index);
				break;
			}
			case 0b00000100: // REMOVE SEGMENT - Removes the current SEGMENT to the VOLATILE_SEGMENT loop at the provided index
			{
				iterator++;

				// Followed by expected signed integer, length from offset to append or delete; positive appends, negative deletes
				length = Decode_Signed_Integer(&iterator, terminator);

				Execute_Remove_Command(command_execution_context, length);
				break;
			}
			case 0b00000011: // REMOVE ALL SEGMENTs - Removes all SEGMENTs from the VOLATILE_SEGMENT loop at the provided index
			{
				iterator++;

				// Followed by expected unsigned integer, Gaia index of segment to capture for reading
				index = Decode_Unsigned_Integer(&iterator, terminator);

				Execute_Capture_Read_Command(command_execution_context, index);
				break;
			}
			case 0b00000010: // RESIZE SEGMENT - Resizes the current segment by adding or deleting memory from the current offset (SEGMENT is reallocated)
			{
				iterator++;

				// Followed by expected unsigned integer, Gaia index of segment to capture for writing
				index = Decode_Unsigned_Integer(&iterator, terminator);

				Execute_Capture_Write_Command(command_execution_context, index);
				break;
			}
			case 0b00000001: // CAPTURE SEGMENT - Captures the latest SEGMENT at the provided index
			{
				iterator++;

				// Followed by expected unsigned integer, Gaia index of segment to be captured
				index = Decode_Unsigned_Integer(&iterator, terminator);

				Execute_Release_Command(command_execution_context, index);
				break;
			}
			case 0b00000000: // COMMIT SEGMENT - Queue the commit of the current SEGMENT to the index provided at the time provided
			{
				iterator++;

				unsigned long commit_time;

				// Followed by expected unsigned integer, Gaia index to commit the current segment to commit
				index        = Decode_Unsigned_Integer(&iterator, terminator);

				// Followed by expected unsigned integer, UTC time commit should go into effect (if future, retain this SEGMENT for future reads)
				commit_time  = Decode_Unsigned_Integer(&iterator, terminator);

				Execute_Commit_Command(command_execution_context, index, commit_time);
				break;
			}
			default:
			{
				/* OFFSET; length; increments current index
				 * 10|######  6 bits             64 Max
				 * 110|##### 13 bits          8,192 Max (+1 octet)
				 * 1110|#### 20 bits      1,048,576 Max (+2 octet)
				 * 11110|### 27 bits    134,217,728 Max (+3 octet)
				 * 111110|## 34 bits 17,179,869,184 Max (+4 octet)
				 * 1111110|# 41 bits           2^41 Max (+5 octet)
				 * 11111110| 48 bits           2^48 Max (+6 octet)
				 * 11111111| Expecte; length of following octets to be copied to the current index; increments current indexd unsigned number
				 *
				 * DAMAGE; length of following octets to be copied to the current index; increments current index
				 * 010|#####  5 bits             32 Max
				 * 0110|#### 12 bits          4,096 Max (+1 octet)
				 * 01110|### 19 bits        524,288 Max (+2 octet)
				 * 011110|## 26 bits     67,108,864 Max (+3 octet)
				 * 0111110|# 33 bits  8,589,934,592 Max (+4 octet)
				 * 01111110| 40 bits           2^40 Max (+5 octet)
				 * 01111111| Expected unsigned number
				 *
				 * INTERNAL COPY; length until copy terminator; increments current index
				 * 0010|####  4 bits            16 Max
				 * 00110|### 11 bits         2,048 Max (+1 octet)
				 * 001110|## 18 bits       262,144 Max (+2 octet)
				 * 0011110|# 25 bits    33,554,432 Max (+3 octet)
				 * 00111110| 32 bits 4,294,967,296 Max (+4 octet)
				 * 00111111| Expected unsigned number
				 * - Followed by expected signed number; offset of copy source index from current index
				 *
				 * EXTERNAL COPY; length until copy terminator; increments current index
				 * 00010|###  3 bits            16 Max
				 * 000110|## 10 bits         2,048 Max (+1 octet)
				 * 0001110|# 17 bits       262,144 Max (+2 octet)
				 * 00011110| 24 bits    33,554,432 Max (+3 octet)
				 * 00011111| Expected unsigned number
				 * - Followed by expected unsigned number; copy source sector index
				 * - Followed by expected unsigned number; copy source index in different index
				 */
				if      (*iterator & 0b10000000 == 0b10000000) // OFFSET; length; increments current index
				{
					switch (*iterator)
					{
						case 0b11111111:                            // 11111111| Expected unsigned number
						{
							iterator += CHAR_OCTET_SIZE;
							length    = Decode_Unsigned_Integer(&iterator, terminator);
							break;
						}
						case 0b11111110:                            // 11111110| 48 bits           2^48 Max (+6 octet)
						{
							iterator += CHAR_OCTET_SIZE;
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + (INT_OCTET_SIZE + SHORT_OCTET_SIZE) > terminator)
							{
								*iterator_pointer = terminator;
								return;
							}
							#endif
							length    = ((unsigned long)*((unsigned int  )*iterator)) << SHORT_BIT_SIZE;
							iterator += INT_OCTET_SIZE;
							length   += (unsigned long)*((unsigned short)*iterator);
							iterator += SHORT_OCTET_SIZE;
							break;
						}
						case 0b11111100:
						case 0b11111101:                            // 1111110|# 41 bits           2^41 Max (+5 octet)
						{
							length    = ((unsigned long)(*((unsigned char)*iterator) & 0x01)) << (INT_BIT_SIZE + CHAR_BIT_SIZE);
							iterator += CHAR_OCTET_SIZE;
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + (INT_OCTET_SIZE + CHAR_OCTET_SIZE) > terminator)
							{
								*iterator_pointer = terminator;
								return;
							}
							#endif
							length   += ((unsigned long)*((unsigned int)*iterator)) << CHAR_BIT_SIZE;
							iterator += INT_OCTET_SIZE;
							length   += (unsigned long)*((unsigned char)*iterator);
							iterator += CHAR_OCTET_SIZE;
							break;
						}
						default:
						{
							if      (*iterator & 0b11111000 == 0b11111000) // 111110|## 34 bits 17,179,869,184 Max (+4 octet)
							{
								length    = ((unsigned long)(*((unsigned char)*iterator) & 0x03)) << INT_BIT_SIZE;
								iterator += CHAR_OCTET_SIZE;
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + INT_OCTET_SIZE > terminator)
								{
									*iterator_pointer = terminator;
									return;
								}
								#endif
								length   += (unsigned long)*((unsigned int)*iterator);
								iterator += INT_OCTET_SIZE;
							}
							else if (*iterator & 0b11110000 == 0b11110000) // 11110|### 27 bits    134,217,728 Max (+3 octet)
							{
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + INT_OCTET_SIZE > terminator)
								{
									*iterator_pointer = terminator;
									return;
								}
								#endif
								length    = (unsigned long)(*((unsigned int)*iterator) & 0x07FFFFFF);
								iterator += INT_OCTET_SIZE;
							}
							else if (*iterator & 0b11100000 == 0b11100000) // 1110|#### 20 bits      1,048,576 Max (+2 octet)
							{
								length    = ((unsigned long)(*((unsigned char)*iterator) & 0x0F)) << SHORT_BIT_SIZE;
								iterator += CHAR_OCTET_SIZE;
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + SHORT_OCTET_SIZE > terminator)
								{
									*iterator_pointer = terminator;
									return;
								}
								#endif
								length   += (unsigned long)*((unsigned short)*iterator);
								iterator += SHORT_OCTET_SIZE;
							}
							else if (*iterator & 0b11000000 == 0b11000000) // 110|##### 13 bits          8,192 Max (+1 octet)
							{
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + SHORT_OCTET_SIZE > terminator)
								{
									*iterator_pointer = terminator;
									return;
								}
								#endif
								length    = (unsigned long)(*((unsigned short)*iterator) & 0x1FFF);
								iterator += SHORT_OCTET_SIZE;
							}
							else                               // 10|######  6 bits             64 Max
							{
								length    = (unsigned long)(*((unsigned char)*iterator) & 0x3F);
								iterator += CHAR_OCTET_SIZE;
							}
							break;
						}
					}
					Execute_Offset_Command(command_execution_context, length + 1);
				}
				else if (*iterator & 0b01000000 == 0b01000000) // DAMAGE; length of following octets to be copied to the current index; increments current index
				{
					switch(*iterator)
					{
						case 0b01111111:                           // 01111111| Expected unsigned number
						{
							iterator += CHAR_OCTET_SIZE;
							length    = Decode_Unsigned_Integer(&iterator, terminator);
							break;
						}
						case 0b01111110:                           // 01111110| 40 bits           2^40 Max (+5 octet)
						{
							iterator += CHAR_OCTET_SIZE;
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + (INT_OCTET_SIZE + CHAR_OCTET_SIZE) > terminator)
							{
								iterator = terminator;
								break;
							}
							#endif
							length    = ((unsigned long)*((unsigned int)*iterator)) << CHAR_BIT_SIZE;
							iterator += INT_OCTET_SIZE;
							length   += (unsigned long)*((unsigned char)*iterator);
							iterator += CHAR_OCTET_SIZE;
							break;
						}
						case 0b01111100:                           // 0111110|# 33 bits  8,589,934,592 Max (+4 octet)
						case 0b01111101:
						{
							length    = ((unsigned long)(*((unsigned char)*iterator) & 0x01)) << INT_BIT_SIZE;
							iterator += CHAR_OCTET_SIZE;
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + INT_OCTET_SIZE > terminator)
							{
								iterator = terminator;
								break;
							}
							#endif
							length   += (unsigned long)*((unsigned int)*iterator);
							iterator += INT_OCTET_SIZE;
							break;
						}
						default:
						{
							if      (*iterator & 0b01111000 == 0b01111000) // 011110|## 26 bits     67,108,864 Max (+3 octet)
							{
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + INT_OCTET_SIZE > terminator)
								{
									iterator = terminator;
									break;
								}
								#endif
								length    = (unsigned long)(*((unsigned int)*iterator) & 0x03FFFFFF);
								iterator += INT_OCTET_SIZE;
							}
							else if (*iterator & 0b01110000 == 0b01110000) // 01110|### 19 bits        524,288 Max (+2 octet)
							{
								length    = ((unsigned long)(*((unsigned char)*iterator) & 0x07)) << SHORT_BIT_SIZE;
								iterator += CHAR_OCTET_SIZE;
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + SHORT_OCTET_SIZE > terminator)
								{
									iterator = terminator;
									break;
								}
								#endif
								length   += (unsigned long)*((unsigned short)*iterator);
								iterator += SHORT_OCTET_SIZE;
							}
							else if (*iterator & 0b01100000 == 0b01100000) // 0110|#### 12 bits          4,096 Max (+1 octet)
							{
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + SHORT_OCTET_SIZE > terminator)
								{
									iterator = terminator;
									break;
								}
								#endif
								length    = (unsigned long)(*((unsigned short)*iterator) & 0x0FFF);
								iterator += SHORT_OCTET_SIZE;
							}
							else                               // 010|#####  5 bits             32 Max
							{
								length    = (unsigned long)(*((unsigned char)*iterator) & 0x1F);
								iterator += CHAR_OCTET_SIZE;
							}
							break;
						}
					}
					Execute_Damage_Command(command_execution_context, iterator, iterator + length);
					iterator += length;
				}
				else if (*iterator & 0b00100000 == 0b00100000) // INTERNAL COPY; length until copy terminator; increments current index
				{
					switch (*iterator)
					{
						case 0b00111111:                           // 00111111| Expected unsigned number
						{
							iterator += CHAR_OCTET_SIZE;
							length    = Decode_Unsigned_Integer(&iterator, terminator);
							break;
						}
						case 0b00111110:                           // 00111110| 32 bits 4,294,967,296 Max (+4 octet)
						{
							length    = ((unsigned long)*((unsigned char)*iterator)) << INT_BIT_SIZE;
							iterator += CHAR_OCTET_SIZE;
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + INT_OCTET_SIZE > terminator)
							{
								iterator = terminator;
								break;
							}
							#endif
							length   += (unsigned long)*((unsigned int)*iterator);
							iterator += INT_OCTET_SIZE;
							break;
						}
						case 0b00111100:
						case 0b00111101:                           // 0011110|# 25 bits    33,554,432 Max (+3 octet)
						{
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + INT_OCTET_SIZE > terminator)
							{
								iterator = terminator;
								break;
							}
							#endif
							length    = (unsigned long)(*((unsigned int)*iterator) & 0x01FFFFFF);
							iterator += INT_OCTET_SIZE;
							break;
						}
						default:
						{
							if      (*iterator & 0b00111000 == 0b00111000) // 001110|## 18 bits       262,144 Max (+2 octet)
							{
								length    = ((unsigned long)(*((unsigned char)*iterator) & 0x03)) << SHORT_BIT_SIZE;
								iterator += CHAR_OCTET_SIZE;
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + SHORT_OCTET_SIZE > terminator)
								{
									iterator = terminator;
									break;
								}
								#endif
								length   += (unsigned long)*((unsigned short)*iterator);
								iterator += SHORT_OCTET_SIZE;
							}
							else if (*iterator & 0b00110000 == 0b00110000) // 00110|### 11 bits         2,048 Max (+1 octet)
							{
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + SHORT_OCTET_SIZE > terminator)
								{
									iterator = terminator;
									break;
								}
								#endif
								length    = (unsigned long)(*((unsigned short)*iterator) & 0x07FF);
								iterator += SHORT_OCTET_SIZE;
							}
							else                               // 0010|####  4 bits            16 Max
							{
								length    = (unsigned long)(*((unsigned char)*iterator) & 0x0F);
								iterator += CHAR_OCTET_SIZE;
							}
							break;
						}
					}

					// Followed by expected signed number; offset of copy source index from current index
					index = Decode_Unsigned_Integer(&iterator, terminator);

					Execute_Internal_Copy_Command(command_execution_context, length, index);
					iterator += length;
				}
				else if (*iterator & 0b00010000 == 0b00010000) // EXTERNAL COPY; length until copy terminator; increments current index
				{
					switch (*iterator)
					{
						case 0b00011111: // Expected unsigned number
						{
							iterator += CHAR_OCTET_SIZE;
							length    = Decode_Unsigned_Integer(&iterator, terminator);
							break;
						}
						case 0b00011110:                 //           24 bits    33,554,432 Max (+3 octet)
						{
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + INT_OCTET_SIZE > terminator)
							{
								iterator = terminator;
								break;
							}
							#endif
							length    = (unsigned long)(*((unsigned int)*iterator) & 0x00FFFFFF);
							iterator += INT_OCTET_SIZE;
							break;
						}
						case 0b00011100:
						case 0b00011101:                 //           17 bits       262,144 Max (+2 octet)
						{
							length    = ((unsigned long)(*((unsigned char)*iterator) & 0x01)) << SHORT_BIT_SIZE;
							iterator += CHAR_OCTET_SIZE;
							#ifdef GAIA_COMMAND_BOUNDS_CHECKING
							if (iterator + SHORT_OCTET_SIZE > terminator)
							{
								iterator = terminator;
								break;
							}
							#endif
							length    = (unsigned long)(*((unsigned short)*iterator));
							iterator += SHORT_OCTET_SIZE;
							break;
						}
						default:
						{
							if (*iterator & 0x18 == 0x18) // 000110|## 10 bits         2,048 Max (+1 octet)
							{
								#ifdef GAIA_COMMAND_BOUNDS_CHECKING
								if (iterator + SHORT_OCTET_SIZE > terminator)
								{
									iterator = terminator;
									break;
								}
								#endif
								length    = (unsigned long)(*((unsigned short)*iterator) & 0x03FF);
								iterator += SHORT_OCTET_SIZE;
							}
							else                          // 00010|###  3 bits            16 Max
							{
								length    = (unsigned long)(*((unsigned char)*iterator) & 0x07);
								iterator += CHAR_OCTET_SIZE;
							}
							break;
						}
					}

					// Followed by expected signed number; offset of copy source index from current index
					index = Decode_Unsigned_Integer(&iterator, terminator);

					// Followed by expected signed number; offset of copy source index from current index
					index = Decode_Unsigned_Integer(&iterator, terminator);

					Execute_External_Copy_Command(command_execution_context, length, source_gaia_index, index);
					iterator += length;
				}
			}
		}
	} // END while (iterator < terminator)

	/* SUDO
		if (no errors)
		{
			command_execution_context->Iterator = iterator;
			commit all segments in segment_list
		}

		releases all segments in segment_list
	*/
}

/* Compile_Gaia_Command
 *
 * Writes "command_buffer" with the GAIA COMMANDs to update the known buffer to the modified buffer.
 * Returns the terminator to the GAIA COMMAND within "command_buffer".
 */
char* Compile_Gaia_Command(GAIA* gaia, char* iterator, char* terminator)
{
	// START SUDO CODE
	foreach (segment in gaia)
	{
		switch (sement.new_commands.length)
		{
			case 1:
			{
				iterator = Write_Capture_Command(segment, iterator, terminator);
				copy(sement.new_commands[0], iterator, iterator + sement.new_commands[0].length);
			}
			case 0:
			{
				break;
			}
			default:
			{
				iterator = Write_Capture_Command(segment, iterator, terminator);
				iterator = Compile_Commands     (segment, iterator, terminator);
			}
		}
	}
	// END SUDO CODE
	return terminator;
}
