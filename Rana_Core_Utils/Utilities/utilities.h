#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "../GUI/NK_BASE.h"
#include "Utils.h"

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
FLOAT
*/

void Scale(float* ar, uint32_t arlen, float target);

/*
NK
*/

// Return the x-y coordinates needed to center a rectangle with the given
// dimensions inside the rectangle given
struct nk_vec2 Center(struct nk_rect parent, struct nk_vec2 child_dimensions);

bool Vec2_Cmp(struct nk_vec2, struct nk_vec2);
string Rect_Str(struct nk_rect);
string Vec2_Str(struct nk_vec2);
string Color_Str(struct nk_color);

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
