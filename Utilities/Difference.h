#ifndef DIFFERENCE_H_
#define DIFFERENCE_H_

#define SIZE_OF_DELTA_HEADER sizeof(long)

#include <iostream>

using namespace std;

unsigned int Encode_Difference_File(unsigned char *, unsigned char *, unsigned char *, unsigned int);

unsigned int Encode_Difference_File_spec(unsigned char *, unsigned char *, unsigned char *, unsigned int, int);

unsigned int Encode_Difference_File_spec(unsigned char *, unsigned char *, unsigned char *, unsigned int);

void Decode_Difference_File(unsigned char *, unsigned int, unsigned char *);

void Decode_Difference_File_spec(unsigned char *, unsigned int, unsigned char *, unsigned char **, unsigned int , unsigned int);

#endif /* DIFFERENCE_H_ */