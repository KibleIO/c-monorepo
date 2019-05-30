#include "Function_Pointers.h"

/*
FUNCTION_POINTER
----------------
*/

void Invoke_Function_Pointer(FUNCTION_POINTER* function, void* caller_data) {
	function->ptr(function->subscriberData, caller_data);
}
FUNCTION_POINTER* Function_Pointer(void(*function_ptr)(void*, void*),
void* subscriber_data) {
	FUNCTION_POINTER* functor = new FUNCTION_POINTER;
	functor->ptr = function_ptr;
	functor->subscriberData = subscriber_data;
	return functor;
}

/*
MULTICAST_FUNCTION_POINTER
--------------------------
*/

void Initialize_Multicast_Function_Pointer(
MULTICAST_FUNCTION_POINTER* multi_ptr, int init_capacity) {
	multi_ptr->functions = new FUNCTION_POINTER[init_capacity];
	multi_ptr->totalFunctions = 0;
	multi_ptr->functionCapacity = init_capacity;
}

void Add_Function_Pointer(MULTICAST_FUNCTION_POINTER* multi_ptr,
void(*new_function)(void*, void*), void* subscriber_data) {
	Add_Function_Pointer(multi_ptr,
		Function_Pointer(new_function, subscriber_data));
}

void Add_Function_Pointer(MULTICAST_FUNCTION_POINTER* multi_ptr,
FUNCTION_POINTER* functor) {
	// Check and reserve more space for more functions
	if(multi_ptr->totalFunctions >= multi_ptr->functionCapacity) {
		Reserve_Function_Pointers(multi_ptr, multi_ptr->functionCapacity * 2);
	}
	// Add the new function at the end and increment total functions
	multi_ptr->functions[multi_ptr->totalFunctions++] = *functor;
}

void Invoke_All_Function_Pointers(
MULTICAST_FUNCTION_POINTER* multi_ptr, void* caller_data) {
	for(int i = 0; i < multi_ptr->totalFunctions; i++) {
		Invoke_Function_Pointer(&multi_ptr->functions[i], caller_data);
	}
}

void Reserve_Function_Pointers(
MULTICAST_FUNCTION_POINTER* multi_ptr, int new_cap) {
	if(new_cap > multi_ptr->functionCapacity) {
		// Allocate an array of functions with the new capacity
		FUNCTION_POINTER* new_functions = new FUNCTION_POINTER[new_cap];
		// Copy the old functions into the new functions
		for(int i = 0; i < multi_ptr->totalFunctions; i++) {
			new_functions[i] = multi_ptr->functions[i];
		}
		// Delete the old pointer and move the new pointer to it
		delete [] multi_ptr->functions;
		multi_ptr->functions = new_functions;
		multi_ptr->functionCapacity = new_cap;
	}
}

void Delete_Multicast_Function_Pointer(MULTICAST_FUNCTION_POINTER* multi_ptr) {
	delete [] multi_ptr->functions;
}
