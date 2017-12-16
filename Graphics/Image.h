#ifndef IMAGE_H_
#define IMAGE_H_

#include <fstream>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;

class Image {
	private:
		unsigned int* pic;
		int width, height;
	public:
	Image(char* str) {
		fstream file;
		file.open(str);
		cout << strerror(errno) << endl;
		file >> width;
		file >> height;
		pic = new unsigned int[width * height];
		for (int X = 0; X < width; X++) {
			for (int Y = 0; Y < height; Y++) {
				file >> pic[Y * width + X];
			}
		}
	}
	void setPriority(int p) {
		p = p << 24;
		for (int X = 0; X < width; X++) {
                        for (int Y = 0; Y < height; Y++) {
                                pic[Y * width + X] = (pic[Y * width + X] & 0x00ffffff) | p;
                        }
                }
	}
	int getARGB(int x, int y) { return pic[y * width + x]; }
	int getHeight() { return height; }
	int getWidth() { return width; }
	~Image() { delete [] pic; }
};

#endif /* IMAGE_H_ */
