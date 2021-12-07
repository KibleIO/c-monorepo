#include "C_ARRAY.h"

bool Initialize_C_ARRAY(C_ARRAY *array, uint32_t elem_size) {
	array->elem_size = elem_size;
	array->index = C_ARRAY_QUEUE_SIZE;
	memset(array->data, 0, C_ARRAY_SIZE);

	// lets make sure we can fit at least two queue's worth of elements
	// inside the array
	return ((C_ARRAY_SIZE / array->elem_size) / 2) > C_ARRAY_QUEUE_SIZE;
}

uint8_t *Get_C_ARRAY(C_ARRAY *array) {
	int index = array->index - C_ARRAY_QUEUE_SIZE;
	return array->data + (index * array->elem_size);
}

bool Put_C_ARRAY(C_ARRAY *array, uint8_t *payload) {
	int num_slots_left =
		(C_ARRAY_SIZE - (array->index * array->elem_size)) / array->elem_size;

	// cout << "num " << *((int*)payload) << endl;

	if (num_slots_left == 0) {
		array->index = C_ARRAY_QUEUE_SIZE - 1;
	} else if (num_slots_left < C_ARRAY_QUEUE_SIZE) {
		int write_index = C_ARRAY_QUEUE_SIZE - (num_slots_left + 1);
		// cout << "writing to index1 " << write_index << endl;

		memcpy(array->data + (write_index * array->elem_size), payload,
			   array->elem_size);
	}

	// cout << "writing to index2 " << array->index << endl;

	memcpy(array->data + (array->index * array->elem_size), payload,
		   array->elem_size);

	array->index++;

	return true;
}

void Delete_C_ARRAY(C_ARRAY *array) {
	(void)array;  // nop
}
