#ifndef GAIA_COMMAND_H_
#define GAIA_COMMAND_H_

using namespace std;

unsigned long Decode_Unsigned_Integer(char** iterator_pointer, char* terminator);
void          Encode_Unsigned_Integer(char** iterator_pointer, char* terminator, unsigned long integer);

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
long         Decode_Signed_Integer  (char** iterator_pointer, char* terminator);
void         Encode_Signed_Integer  (char** iterator_pointer, char* terminator, long integer);

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

struct GAIA_COMMAND_CONTEXT
{
	GAIA*         Gaia;
	SEGMENT*      Segment;
	char*         Iterator;
	unsigned long Gaia_Index;
	unsigned long Offset;
};

inline void Execute_Offset_Command        (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long offset);
inline void Execute_Damage_Command        (GAIA_COMMAND_CONTEXT* gaia_command_context, char* damage_iterator, char* damage_terminator);
inline void Execute_Internal_Copy_Command (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long copy_length, unsigned long source_index);
inline void Execute_External_Copy_Command (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long copy_length, unsigned long source_gaia_index, unsigned long source_index);
inline void Execute_Difference_Command    (GAIA_COMMAND_CONTEXT* gaia_command_context, signed char* difference_iterator, signed char* difference_terminator);
inline void Execute_New_Command           (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long length);
inline void Execute_Add_Command           (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long index);
inline void Execute_Remove_Command        (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long index);
inline void Execute_Resize_Command        (GAIA_COMMAND_CONTEXT* gaia_command_context, long length);
inline void Execute_Capture_Read_Command  (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long index);
inline void Execute_Capture_Write_Command (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long index);
inline void Execute_Release_Command       (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long index);
inline void Execute_Commit_Command        (GAIA_COMMAND_CONTEXT* gaia_command_context, unsigned long index);

/* Execute_Encoded_Commands
 *
 * If the contents from "iterator" to "terminator" contain valid GAIA COMMAND(s), the GAIA COMMAND(s) are applied and the applied SEGMENT returned.
 * Else if any part of the contents from "iterator" to "terminator" contain invalid GAIA COMMAND(s), no GAIA COMMAND(s) are applied to the target and NULL is returned.
 */
void Execute_Encoded_Commands(GAIA_COMMAND_CONTEXT* gaia_command_context);

/* Compile_Gaia_Command
 *
 * Writes "command_buffer" with the GAIA COMMANDs to update the known buffer to the modified buffer.
 * Returns the terminator to the GAIA COMMAND within "command_buffer".
 */
char* Compile_Gaia_Command(GAIA* gaia, char* iterator, char* terminator);

#endif // GAIA_COMMAND_H_
