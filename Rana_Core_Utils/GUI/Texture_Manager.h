#ifndef TEXTURE_MANAGER_H_
#define TEXTURE_MANAGER_H_

#include "NK_BASE.h"
#include "Texture.h"
#include "../Utilities/LOGGING.h"

#define TEXTURE_MANAGER_MAX_TEXTURES 256

struct TEXTURE_MANAGER {
	TEXTURE textures[TEXTURE_MANAGER_MAX_TEXTURES];
	uint32_t size = 0;
};

// Add a texture to the texture manager if it does not exist
// If it does exist, resize it using given parameters
// Returns a pointer to the texture that is loaded/resized
TEXTURE* Load_Texture_TEXTURE_MANAGER(
TEXTURE_MANAGER*, string path, uint32_t width, uint32_t height);
bool Add_Texture_TEXTURE_MANAGER(TEXTURE_MANAGER*, TEXTURE);

// Load all textures using current directories and sizes stored in them
void Load_All_Textures_TEXTURE_MANAGER(TEXTURE_MANAGER*);
// Free dynamically allocated data of all textures
void Free_All_Textures_TEXTURE_MANAGER(TEXTURE_MANAGER*);

// HELPERS
// Find the index of the texture with the given path,
// or -1 if it does not exist
int32_t Texture_With_Path_TEXTURE_MANAGER(TEXTURE_MANAGER*, string path);

#endif
