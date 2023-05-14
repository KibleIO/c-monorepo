//PLATFORMS: Linux, Windows, OSX

#ifndef INDEX_QUEUE_H
#define INDEX_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <mutex>

using namespace std;

//alec <3 2013-2018 you will be missed
struct INDEX_QUEUE {
	uint8_t*	occupied;
	uint32_t	index;
	uint32_t	minimum;
	uint32_t	capacity;
	mutex		mtx;
};

bool		Initialize_INDEX_QUEUE(INDEX_QUEUE*, uint32_t, uint32_t);
void		Release_All_INDEX_QUEUE(INDEX_QUEUE*);
uint32_t	Capture_INDEX_QUEUE(INDEX_QUEUE*);
uint8_t		Capture_INDEX_QUEUE(INDEX_QUEUE*, uint32_t);
uint8_t		Is_Captured_INDEX_QUEUE(INDEX_QUEUE*, uint32_t);
void		Release_INDEX_QUEUE(INDEX_QUEUE*, uint32_t);
void		Delete_INDEX_QUEUE(INDEX_QUEUE*);

#endif
