#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "BMP.h"

void BakeBackground_BMP(BMP* bmp, int color) {
	unsigned int pixel;
	unsigned int result;

	for (int y = 0; y < bmp->H; y++) {
		for (int x = 0; x < bmp->W; x++) {
			pixel = *(((int*)bmp->Data) + y * bmp->W + x);

			unsigned int alpha = ((unsigned char*)&pixel)[3] + 1;
			unsigned int inv_alpha = 256 - alpha;

    		((unsigned char*)&result)[0] = 
				(unsigned char)((alpha * 
				((unsigned char*)&pixel)[0] + inv_alpha * 
				((unsigned char*)&color)[0]) >> 8);
    		((unsigned char*)&result)[1] = 
				(unsigned char)((alpha * 
				((unsigned char*)&pixel)[1] + inv_alpha * 
				((unsigned char*)&color)[1]) >> 8);
    		((unsigned char*)&result)[2] = 
				(unsigned char)((alpha * 
				((unsigned char*)&pixel)[2] + inv_alpha *
				((unsigned char*)&color)[2]) >> 8);
    		((unsigned char*)&result)[3] = 0xff;

			((int*)bmp->Data)[y * bmp->W + x] = result;
		}
	}

	bmp->Transparent = false;
}

void Initialize_BMP(BMP* bmp, string loc) {
	int x,y,n;
  unsigned char *data = stbi_load(loc.c_str(), &x, &y, &n, 4);

	if (n != 4) {
		log_err("File is wrong format");
		return;
	}

	log_dbg(loc + " " + to_string(x) + " " + to_string(y));

	bmp->W = x;
	bmp->H = y;

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

			if (loc.find(".bmp") != string::npos) {
				((int*)bmp->Data)[y * bmp->W + x] |= 0xff000000;
			}
		}
	}

	bmp->Transparent = false;
}

void Initialize_BMP(BMP* bmp, string loc, int w, int h) {
	int x,y,n;
  unsigned char *data = stbi_load(loc.c_str(), &x, &y, &n, 4);

	if (n != 4) {
		log_err("File is wrong format");
		return;
	}

	log_dbg(loc + " " + to_string(x) + " " + to_string(y));

	bmp->W = x;
	bmp->H = y;

	int* resized_buffer = new int[w * h];

	char* temp_Data = new char[(bmp->W + 1) * (bmp->H + 1) * 4];

	for (int x = 0; x < bmp->W; x++) {
		for (int y = 0; y < bmp->H; y++) {
			((int*)temp_Data)[y * (bmp->W + 1) + x] = ((int*)data)[y * bmp->W + x];
		}
	}

	stbi_image_free(data);

	bmp->Data = temp_Data;

	resizeBilinear((int*) bmp->Data, (int*) resized_buffer, bmp->W + 1, bmp->H + 1, w, h);

	delete bmp->Data;
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

			if (loc.find(".bmp") != string::npos) {
				((int*)bmp->Data)[y * bmp->W + x] |= 0xff000000;
			}
		}
	}

	bmp->Transparent = false;
}

void resizeBilinear(int* pixels, int* output, int w, int h, int w2, int h2) {
	int a, b, c, d, x, y, index;
	float x_ratio = ((float)(w-1))/w2 ;
	float y_ratio = ((float)(h-1))/h2 ;
	float x_diff, y_diff, blue, red, green, alpha;
	int offset = 0 ;
	for (int i=0;i<h2;i++) {
		for (int j=0;j<w2;j++) {
			x = (int)(x_ratio * j) ;
			y = (int)(y_ratio * i) ;
			x_diff = (x_ratio * j) - x ;
			y_diff = (y_ratio * i) - y ;
			index = y*w+x ;
			a = pixels[index] ;
			b = pixels[index+1] ;
			c = pixels[index+w] ;
			d = pixels[index+w+1] ;

			// blue element
			// Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
			blue = (a&0xff)*(1-x_diff)*(1-y_diff) + (b&0xff)*(x_diff)*(1-y_diff) +
			(c&0xff)*(y_diff)*(1-x_diff)   + (d&0xff)*(x_diff*y_diff);

			// green element
			// Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
			green = ((a>>8)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>8)&0xff)*(x_diff)*(1-y_diff) +
			((c>>8)&0xff)*(y_diff)*(1-x_diff)   + ((d>>8)&0xff)*(x_diff*y_diff);

			// red element
			// Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
			red = ((a>>16)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>16)&0xff)*(x_diff)*(1-y_diff) +
			((c>>16)&0xff)*(y_diff)*(1-x_diff)   + ((d>>16)&0xff)*(x_diff*y_diff);

			// red element
			// Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
			alpha = ((a>>24)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>24)&0xff)*(x_diff)*(1-y_diff) +
			((c>>24)&0xff)*(y_diff)*(1-x_diff)   + ((d>>24)&0xff)*(x_diff*y_diff);

			output[offset++] = ((int)blue) | (((int)green) << 8) | (((int)red) << 16) | (((int)alpha) << 24);
		}
	}
}

void Draw_BMP(BMP* bmp, GRAPHICS* g, int X, int Y) {
	if (bmp->Transparent) {
		for (int x = 0; x < bmp->W; x++) {
    		for (int y = 0; y < bmp->H; y++) {
				DrawPoint_GRAPHICS(g, X + x, Y + y, ((int*)bmp->Data)[y * bmp->W + x]);
    		}
    	}
	} else {
		for (int y = 0; y < bmp->H; y++) {
			copy((int*)bmp->Data + y * bmp->W, (int*)bmp->Data + (y + 1) * bmp->W - 1, (int*)g->Buffer + (Y + y) * g->Width + X);
		}
	}
}

void Draw_BMP(BMP* bmp, char* fbp, int fbp_w, int X, int Y) {
	if (bmp->Transparent) {
		unsigned int fg, bg, alpha, inv_alpha, result;
		for (int y = 0; y < bmp->H; y++) {
			for (int x = 0; x < bmp->W; x++) {
				fg = *((int*)bmp->Data + y * bmp->W + x);
				bg = *((int*)fbp + (y + Y) * fbp_w + x + X);
				
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

				((int*)fbp)[(y + Y) * fbp_w + x + X] = result;
			}
		}
	} else {
		for (int y = 0; y < bmp->H; y++) {
			copy((int*)bmp->Data + y * bmp->W, (int*)bmp->Data + (y + 1) * bmp->W, (int*)fbp + (Y + y) * fbp_w + X);
		}
	}
}

void Delete_BMP(BMP* bmp) {
	delete bmp->Data;
}
