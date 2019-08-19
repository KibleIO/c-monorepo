#ifndef FUNCTION_POINTERS_H_
#define FUNCTION_POINTERS_H_

#include <cstdint>
#include "LOGGING.h"

#define DEFAULT_INITIAL_FUNCTION_CAPACITY 8

// A single function pointer which has data from the subscriber
// and data from the caller
typedef void (*function_pointer)(void* subscriber_data, void* caller_data);

// Encapsulates a function pointer paired
// with a pointer to the subscribing object
struct FUNCTION_POINTER {
	function_pointer ptr;
	void* subscriberData;
};

FUNCTION_POINTER Function_Pointer(
void(*ptr)(void*, void*), void* subscriber_data);
void Delete_Function_Pointer(FUNCTION_POINTER*);

void Invoke_Function_Pointer(FUNCTION_POINTER*, void* caller_data);

// A managed list of function pointers allows any number of clients to
// subscribe any number of function pointers to the event
struct MULTICAST_FUNCTION_POINTER {
	// Functions pointers take any pointer type as arguments
	// Client code must be careful to re-interpret the void pointer
	// to the correct event data structure
	FUNCTION_POINTER* functions;
	int totalFunctions;
	int functionCapacity;
};

void Initialize_Multicast_Function_Pointer(
MULTICAST_FUNCTION_POINTER*,
int init_capacity = DEFAULT_INITIAL_FUNCTION_CAPACITY);

void Add_Function_Pointer(
MULTICAST_FUNCTION_POINTER*, void(*function)(void*, void*),
void* subscriber_data);
void Add_Function_Pointer(MULTICAST_FUNCTION_POINTER*, FUNCTION_POINTER);

void Invoke_All_Function_Pointers(
MULTICAST_FUNCTION_POINTER*, void* caller_data);
void Reserve_Function_Pointers(MULTICAST_FUNCTION_POINTER*, int);

void Delete_Multicast_Function_Pointer(MULTICAST_FUNCTION_POINTER*);

#endif // FUNCTION_POINTERS_H_
