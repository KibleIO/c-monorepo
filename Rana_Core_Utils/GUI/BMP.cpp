//PLATFORMS: Linux, Windows, OSX

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "BMP.h"

void Initialize_BMP(BMP* bmp, string loc) {
	if (bmp->Data || bmp->W || bmp->H || bmp->Transparent) {
		log_err("bmp struct for " + loc + " not properly nullified");
		return;
	}

	int x,y,n;
	unsigned char *data = stbi_load(loc.c_str(), &x, &y, &n, 4);

	bmp->W = x;
	bmp->H = y;
	bmp->name = loc;

	uint8_t valid_image = true;
	if (!data) {
		log_err("no image file exists at " + loc);
		valid_image = false;
	} else if (n != 4) {
		log_err(string("File ") + loc + " is wrong format");
		valid_image = false;
	}

	if (!valid_image) {
		bmp->W = 32;
		bmp->H = 32;
		uint32_t* missing_buffer = new uint32_t[bmp->W * bmp->H]{0xffff00ff};
		bmp->Data = (char*)missing_buffer;
		return;
	}

	log_dbg(loc + " " + to_string(x) + " " + to_string(y));

	char* temp_Data = new char[bmp->W * bmp->H * 4];

	for (int x = 0; x < bmp->W; x++) {
  	for (int y = 0; y < bmp->H; y++) {
			((int*)temp_Data)[y * bmp->W + x] = ((int*)data)[y * bmp->W + x];
  	}
  }

	stbi_image_free(data);

	bmp->Data = temp_Data;

	int work;

	for (int x = 0; x < bmp->W; x++) {
		for (int y = 0; y < bmp->H; y++) {
			work = ((int*)bmp->Data)[y * bmp->W + x];

			work = ((work & 0xff000000)) | //______AA
	        	   ((work & 0x00ff0000) >> 16) | //____RR__
	        	   ((work & 0x0000ff00)) | //__GG____
	        	   ((work & 0x000000ff) << 16);

			((int*)bmp->Data)[y * bmp->W + x] = work;

			//if (loc.find(".bmp") != string::npos) {
			//	((int*)bmp->Data)[y * bmp->W + x] |= 0xff000000;
			//}
		}
	}

	bmp->Transparent = false;
}


void Initialize_BMP(BMP* bmp, string loc, int w, int h) {
	if (bmp->Data || bmp->W || bmp->H || bmp->Transparent) {
		log_err("bmp struct not " + loc + " properly nullified");
		return;
	}

	string resized_file = loc + "resized" + to_string(w) + "x" + to_string(h);

	if (file_exists(resized_file)) {
		log_dbg("loading resized version of image " + loc);
		ifstream inp(resized_file, ios::in | ios::binary);
		bmp->Data = new char[w * h * 4];
		inp.read(bmp->Data, w * h * 4);
		bmp->W = w;
		bmp->H = h;
		bmp->name = loc;
		return;
	} else {
		log_dbg("resized image not found, resizing");
	}

	int x,y,n;

	unsigned char *data = stbi_load(loc.c_str(), &x, &y, &n, 4);

	bmp->W = x;
	bmp->H = y;
	bmp->name = loc;

	uint8_t valid_image = true;
	if (!data) {
		log_err("no image file exists at " + loc);
		valid_image = false;
	} else if (n != 4) {
		log_err(string("File ") + loc + " is wrong format");
		valid_image = false;
	}

	if (!valid_image) {
		bmp->W = w;
		bmp->H = h;
		uint32_t* missing_buffer = new uint32_t[w * h]{0xffff00ff};
		bmp->Data = (char*)missing_buffer;
		return;
	}

	log_dbg(loc + " " + to_string(x) + " " + to_string(y));

	int* resized_buffer = new int[w * h];

	//char* temp_Data = new char[(bmp->W + 1) * (bmp->H + 1) * 4];

	//for (int x = 0; x < bmp->W; x++) {
	//	for (int y = 0; y < bmp->H; y++) {
	//		((int*)temp_Data)[y * (bmp->W + 1) + x] = ((int*)data)[y * bmp->W + x];
	//	}
	//}

	bmp->Data = (char*)data;

	avir::CImageResizer<> air(8);

	//resizeBilinear((int*) bmp->Data, (int*) resized_buffer, bmp->W + 1, bmp->H + 1, w, h);
	air.resizeImage((uint8_t*)bmp->Data, bmp->W, bmp->H, 0, (uint8_t*)resized_buffer, w, h, 4, 0);

	stbi_image_free(data);

	//delete [] bmp->Data;
	bmp->Data = (char*) resized_buffer;

	bmp->W = w;
	bmp->H = h;

	int work;

	for (int x = 0; x < bmp->W; x++) {
		for (int y = 0; y < bmp->H; y++) {
			work = ((int*)bmp->Data)[y * bmp->W + x];
			work = ((work & 0xff000000)) | //______AA
	        	   ((work & 0x00ff0000) >> 16) | //____RR__
	        	   ((work & 0x0000ff00)) | //__GG____
	        	   ((work & 0x000000ff) << 16);

			((int*)bmp->Data)[y * bmp->W + x] = work;

			//if (loc.find(".bmp") != string::npos) {
			//	((int*)bmp->Data)[y * bmp->W + x] |= 0xff000000;
			//}
		}
	}

	bmp->Transparent = false;

	ofstream out(resized_file, ios::out | ios::binary);
	out.write(bmp->Data, w * h * 4);
}

void Draw_BMP(BMP* bmp, GRAPHICS* g, int X, int Y) {
	if (bmp->Transparent) {
		for (int x = 0; x < bmp->W; x++) {
    		for (int y = 0; y < bmp->H; y++) {
				DrawPoint_GRAPHICS(g, X + x, Y + y, ((int*)bmp->Data)[y * bmp->W + x]);
    		}
    	}
	} else {
		int temp_width = bmp->W;
		int temp_height = bmp->H;

		Clip_GRAPHICS(g, X, Y, temp_width, temp_height);

		for (int y = 0; y < temp_height; y++) {
			copy(
			(int*)bmp->Data + y * bmp->W,
			(int*)bmp->Data + y * bmp->W + temp_width,
			(int*)g->Buffer + (Y + y) * g->Width + X);
		}
	}
}

void Draw_BMP(BMP* bmp, char* fbp, int fbp_w, int fbp_h, int X, int Y) {
	if (bmp->Transparent) {
		int w = bmp->W;
		int h = bmp->H;
		int x = X;
		int y = Y;

		if (x > fbp_w) return;
		if (y > fbp_h) return;
		if (x < 0) { w += x; x = 0; }
		if (y < 0) { h += y; y = 0; }
		if (w <= 0) return;
		if (h <= 0) return;
		if (x + w > fbp_w) w = fbp_w - x;
		if (y + h > fbp_h) h = fbp_h - y;

		unsigned int fg, bg, alpha, inv_alpha, result;
		for (int _y = 0; _y < h; _y++) {
			for (int _x = 0; _x < w; _x++) {
				fg = *((int*)bmp->Data + _y * bmp->W + _x);
				bg = *((int*)fbp + (_y + y) * fbp_w + _x + x);

				alpha = ((unsigned char*)&fg)[3] + 1;
				inv_alpha = 256 - alpha;

    			((unsigned char*)&result)[0] =
					(unsigned char)((alpha *
					((unsigned char*)&fg)[0] + inv_alpha *
					((unsigned char*)&bg)[0]) >> 8);
    			((unsigned char*)&result)[1] =
					(unsigned char)((alpha *
					((unsigned char*)&fg)[1] + inv_alpha *
					((unsigned char*)&bg)[1]) >> 8);
    			((unsigned char*)&result)[2] =
					(unsigned char)((alpha *
					((unsigned char*)&fg)[2] + inv_alpha *
					((unsigned char*)&bg)[2]) >> 8);
    			((unsigned char*)&result)[3] = 0xff;

				((int*)fbp)[(_y + y) * fbp_w + _x + x] = result;
			}
		}
	} else {
		int w = bmp->W;
		int h = bmp->H;
		int x = X;
		int y = Y;

		if (x > fbp_w) return;
		if (y > fbp_h) return;
		if (x < 0) { w += x; x = 0; }
		if (y < 0) { h += y; y = 0; }
		if (w <= 0) return;
		if (h <= 0) return;
		if (x + w > fbp_w) w = fbp_w - x;
		if (y + h > fbp_h) h = fbp_h - y;

		for (int _y = 0; _y < h; _y++) {
			copy(
			(int*)bmp->Data + _y * w,
			(int*)bmp->Data + (_y + 1) * w,
			(int*)fbp + (y + _y) * fbp_w + x);
		}
	}
}

void Draw_Inverted_Mouse_BMP(BMP* bmp, char* fbp, int fbp_w, int fbp_h, int X, int Y) {
	int w = bmp->W;
	int h = bmp->H;
	int x = X;
	int y = Y;

	if (x > fbp_w) return;
	if (y > fbp_h) return;
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if (w <= 0) return;
	if (h <= 0) return;
	if (x + w > fbp_w) w = fbp_w - x;
	if (y + h > fbp_h) h = fbp_h - y;

	for (int _y = 0; _y < h; _y++) {
		for (int _x = 0; _x < w; _x++) {
			((int*)fbp)[(h - 1 - _y + y) * fbp_w + _x + x] = 
			*((int*)bmp->Data + _y * bmp->W + _x);
		}
	}
}

void Delete_BMP(BMP* bmp) {
	delete [] bmp->Data;
#ifdef GRAPHICS_USING_HARDWARE	
	vgDestroyImage(bmp->img);
#endif
}
