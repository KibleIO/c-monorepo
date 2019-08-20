#include "Texture.h"

#include <iostream>
using namespace std;

TEXTURE Texture(string path, uint32_t width, uint32_t height) {
	TEXTURE texture;
	texture.path = path;
	texture.width = width;
	texture.height = height;
	Load_TEXTURE(&texture);
	return texture;
}

void Load_TEXTURE(TEXTURE* texture) {
	if (!texture->loaded) {
		texture->data = Load_Image_NK_GEN(
		texture->path, texture->width, texture->height);
		texture->loaded = true;
	}
}

void Resize_TEXTURE(TEXTURE* texture, uint32_t width, uint32_t height) {
	if (texture->loaded) {
		Resize_Image_NK_GEN(&texture->data, width, height);
		texture->width = width;
		texture->height = height;
	}
}

void Free_TEXTURE(TEXTURE* texture) {
	if (texture->loaded) {
		Free_Image_NK_GEN(&texture->data);
		texture->loaded = false;
	}
}

void Delete_TEXTURE(TEXTURE* texture) {
	Free_TEXTURE(texture);
}
