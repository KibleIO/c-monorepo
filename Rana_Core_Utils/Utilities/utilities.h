#ifndef UTILITIES_H_
#define UTILITIES_H_

#include <GUI/GUI.h>
#include <stdint.h>
#include <cstdlib>
#include <numeric>
#include <algorithm>
#include <vector>
#include <sstream>

#include <Utilities/Utils.h>

string* Parse_Punctuated_List(const string, int& size, const char punctuation = ',');
string* Parse_Punctuated_List(const string, const char punctuation = ',');
string Curate_Punctuated_List(const string, const char punctuation = ',');
int find_all(string, const char);
string trimmed(string, const char = ' ');
void Remove_Whitespace(string*);
void Scale(float* ar, uint32_t arlen, float target);

// Return the x-y coordinates needed to center a rectangle with the given
// dimensions inside the rectangle given
struct nk_vec2 Center(struct nk_rect parent, struct nk_vec2 child_dimensions);

string Rect_Str(struct nk_rect);
string Color_Str(struct nk_color);

bool Valid_IP(string ip);
string Full_Sytem_IP();
string System_IP_V4();

#endif //UTILITIES_H_
