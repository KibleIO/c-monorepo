#include "Texture_Manager.h"

TEXTURE* Load_Texture_TEXTURE_MANAGER(
TEXTURE_MANAGER* manager, string path, uint32_t width, uint32_t height) {
	int32_t index = Texture_With_Path_TEXTURE_MANAGER(manager, path);

	// If the texture exists, reload it
	if (index >= 0) {
		Resize_TEXTURE(&manager->textures[index], width, height);
		return &manager->textures[index];
	}
	// If not, add a new texture
	else {
		if (
		Add_Texture_TEXTURE_MANAGER(manager, Texture(path, width, height))) {
			return &manager->textures[manager->size - 1];
		}
		else {
			return NULL;
		}
	}
}

bool Add_Texture_TEXTURE_MANAGER(TEXTURE_MANAGER* manager, TEXTURE texture) {
	if (manager->size < TEXTURE_MANAGER_MAX_TEXTURES) {
		manager->textures[manager->size++] = texture;
		return true;
	}
	else {
		log_err("The texture manager cannot hold any more textures");
		return false;
	}
}

void Load_All_Textures_TEXTURE_MANAGER(TEXTURE_MANAGER* manager) {
	for (uint32_t i = 0; i < manager->size; i++) {
		Load_TEXTURE(&manager->textures[i]);
	}
}

void Free_All_Textures_TEXTURE_MANAGER(TEXTURE_MANAGER* manager) {
	for (uint32_t i = 0; i < manager->size; i++) {
		Free_TEXTURE(&manager->textures[i]);
	}
}

int32_t Texture_With_Path_TEXTURE_MANAGER(
TEXTURE_MANAGER* manager, string path) {
	for (uint32_t i = 0; i < manager->size; i++) {
		if (manager->textures[i].path == path) {
			return i;
		}
	}
	return -1;
}
