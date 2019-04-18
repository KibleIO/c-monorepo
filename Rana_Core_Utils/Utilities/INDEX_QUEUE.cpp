//PLATFORMS: Linux, Windows, OSX

#include "INDEX_QUEUE.h"

void Initialize_INDEX_QUEUE(
INDEX_QUEUE* queue, uint32_t minimum, uint32_t maximum) {
	queue->mtx.lock();
	queue->index	= 0;
	queue->minimum	= minimum;
	queue->capacity	= (maximum - minimum) + 1;
	if (queue->capacity > 0 && queue->capacity <= maximum + 1) {
		queue->occupied = new uint8_t[queue->capacity];
	} else {
		queue->occupied = NULL;
	}
	queue->mtx.unlock();
	Release_All_INDEX_QUEUE(queue);
}

void Release_All_INDEX_QUEUE(INDEX_QUEUE* queue) {
	queue->mtx.lock();
	if (queue->occupied == NULL) {
		queue->mtx.unlock();
		return;
	}
	for (uint32_t i = 0; i < queue->capacity; i++){
		queue->occupied[i] = false;
	}
	queue->mtx.unlock();
}

uint32_t Capture_INDEX_QUEUE(INDEX_QUEUE* queue) {
	queue->mtx.lock();
	if (queue->occupied == NULL) {
		queue->mtx.unlock();
		return -1;
	}
	for (
	uint32_t end = queue->index + (queue->capacity * 2); queue->index <= end;
	queue->index++) {
		if (!queue->occupied[queue->index % queue->capacity]) {
			queue->occupied[queue->index % queue->capacity] = true;
			queue->mtx.unlock();
			return queue->minimum + (queue->index % queue->capacity);
		}
	}
	queue->mtx.unlock();
	return -1;
}

uint8_t	Capture_INDEX_QUEUE(INDEX_QUEUE* queue, uint32_t index) {
	queue->mtx.lock();
	if (queue->occupied != NULL && !Is_Captured_INDEX_QUEUE(queue, index)) {
		queue->occupied[index - queue->minimum] = true;
		queue->mtx.unlock();
		return true;
	}
	queue->mtx.unlock();
	return false;
}

uint8_t Is_Captured_INDEX_QUEUE(INDEX_QUEUE* queue, uint32_t index) {
	queue->mtx.lock();
	if (
	queue->occupied != NULL && index >= queue->minimum && index <
	queue->minimum + queue->capacity) {
		queue->mtx.unlock();
		return queue->occupied[index - queue->minimum];
	}
	queue->mtx.unlock();
	return false;
}

void Release_INDEX_QUEUE(INDEX_QUEUE* queue, uint32_t index) {
	queue->mtx.lock();
	if (
	queue->occupied != NULL && index >= queue->minimum && index <
	queue->minimum + queue->capacity) {
		queue->occupied[index - queue->minimum] = false;
	}
	queue->mtx.unlock();
}

void Delete_INDEX_QUEUE(INDEX_QUEUE* queue) {
	queue->mtx.lock();
	if (queue->occupied != NULL) {
		delete queue->occupied;
	}
	queue->mtx.unlock();
}
