#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "NK_BASE.h"

struct TEXTURE {
	string path;
	uint32_t width;
	uint32_t height;
	bool loaded = false;
	struct nk_image data;
};

TEXTURE Texture(string path, uint32_t width, uint32_t height);

// Load up the nk_image data using local width/height
void Load_TEXTURE(TEXTURE*);
// Resize the nk_image data without invalidating the internal pointer
void Resize_TEXTURE(TEXTURE*, uint32_t width, uint32_t height);
// Free the nk_image data
void Free_TEXTURE(TEXTURE*);

void Delete_TEXTURE(TEXTURE*);

#endif
