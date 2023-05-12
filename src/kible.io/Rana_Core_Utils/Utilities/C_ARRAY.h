#ifndef _C_ARRAY_H_
#define _C_ARRAY_H_

#include <stdint.h>

#include <cstring>
#include <iostream>
#include <mutex>

using namespace std;

// C_ARRAY stands for circular array
// this class is not thread safe

#define C_ARRAY_SIZE 50000
#define C_ARRAY_QUEUE_SIZE 5

struct C_ARRAY {
	uint8_t data[C_ARRAY_SIZE];
	uint32_t elem_size;
	uint32_t index;
};

bool Initialize_C_ARRAY(C_ARRAY*, uint32_t);
uint8_t* Get_C_ARRAY(C_ARRAY*);
bool Put_C_ARRAY(C_ARRAY*, uint8_t*);
void Delete_C_ARRAY(C_ARRAY*);

#endif
