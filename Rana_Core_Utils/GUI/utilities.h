#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "NK_BASE.h"
#include "../Utilities/Utils.h"

#include <stdint.h>
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <vector>
#include <sstream>
#include <thread>

/*
STRING
*/

string* Parse_Punctuated_List(const string, int& size, const char punctuation = ',');
string* Parse_Punctuated_List(const string, const char punctuation = ',');
string Curate_Punctuated_List(const string, const char punctuation = ',');
int find_all(string, const char);
string trimmed(string, const char = ' ');
void Remove_Whitespace(string*);

/*
FLOAT / INT
*/

void Scale(float* ar, uint32_t arlen, float target);

float Get_Interpolator(int32_t a, int32_t b, int32_t c);
float Get_Interpolator(float a, float b, float c);

int32_t Lerp(int32_t a, int32_t b, float t);
float Lerp(float a, float b, float t);
struct nk_vec2 Lerp(struct nk_vec2 a, struct nk_vec2 b, float t);

/*
NK
*/

// Return the x-y coordinates needed to center a rectangle with the given
// dimensions inside the rectangle given
struct nk_vec2 Center(struct nk_rect parent, struct nk_vec2 child_dimensions);

/*
IP
*/

bool Valid_IP(string ip);
string Full_Sytem_IP();
string System_IP_V4();

/*
THREAD
*/

void Delete_Thread(thread**);

#endif //UTILITIES_H_
