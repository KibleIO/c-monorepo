#include "utilities.h"
using namespace std;

// Given a single list separated by punctuation marks, this function returns
// an array where each element is each part in the string
// that was separated by the punctuation marks
string* Parse_Punctuated_List(const string list, int& size, const char punctuation) {
	// cout << "Parsing list: " << list << endl;
	// cout << "Allocating delimeter" << endl;
	// const char* delim = new char[2] { punctuation, '\0' };
	// cout << "Allocating list data" << endl;
	// char* list_dat = new char[list.length()];	// Copy of string data
	// char* token;	// Token obtained from strtok
	// vector<string> token_vec;	// Vector with each token in the list
	// string* token_ar;	// Array with each token, copied from vector
	// // Copy string data into local c_str
	// copy(list.begin(), list.end(), list_dat);
	// // Grab tokens with "strtok" until no tokens are left
	// token = strtok(list_dat, delim);
	// while(token != NULL) {
	// 	token_vec.push_back(string(token));
	//
	// 	token = strtok(NULL, delim);
	// }
	// // Copy the vector into the array
	// size = token_vec.size();
	// token_ar = new string[size];
	// copy(token_vec.begin(), token_vec.end(), token_ar);
	// cout << "Returning list" << endl;
	// return token_ar;

	// Version of the list actually used in the function.
	// Start by trimming off leading and trailing whitespace
	string curatedList = trimmed(list);
	string* arToReturn; // Pointer to the block of strings to return
	int currentFound; // Index of the current punctuation mark in the list
	int previousFound = -1; // Index of the previous punctuation mark in the list
	// Set size of the string list by finding all punctuation marks
	size = find_all(list, punctuation) + 1;
	// Check to make sure that the user did not accidentally put a punctuation mark at the end
	// if they did, decrement the number of items and pop it off the end of the list string
	if(curatedList[curatedList.size() - 1] == punctuation) {
	  size--;
	  curatedList.pop_back();
	}
	// Allocate memory for the array
	arToReturn = new string[size];
	for(int index = 0; index < size; index++) {
 		// currentFound = list.find(punctuation, previousFound + 1);
		// if(currentFound > previousFound + 1) {
		// 	arToReturn[index] = list.substr(previousFound + 1, )
		// }

		// If this is not the last item, find the next punctuation
		// and take the substring from previous punctuation to the next
		if(index < size - 1) {
		  // Find the next punctuation mark after the index of the previous one
		  currentFound = curatedList.find(punctuation, previousFound + 1);
		  // Add the substring from the previous punctuation
		  // up to the current punctuation
		  arToReturn[index] = curatedList.substr(previousFound + 1, currentFound - (previousFound + 1));
		  // Trim whitespace off of the ends of the string
		  arToReturn[index] = trimmed(arToReturn[index]);
		  // Previous is now current
		  previousFound = currentFound;
		}
		// If this is the last item, take the substring from the previous punctuation to the end of the string
		else {
		  arToReturn[index] = curatedList.substr(previousFound + 1, curatedList.length());
		  // Trim whitespace off of the ends of the string
		  arToReturn[index] = trimmed(arToReturn[index]);
		}
	}
	return arToReturn;
}

// Overload of Parse_Punctuated_List that does not reference an outside size
string* Parse_Punctuated_List(const string list, const char punctuation) {
  int size = 0;
  return Parse_Punctuated_List(list, size, punctuation);
}

// Find all instances of ch in the string and return the number of instances found
int find_all(const string s, const char ch) {
  int instances = 0;
  int32_t indexFound = -1; // Index of the character in the string

  // Loop until the character cannot be found in the remainder of the string
  do {
    // Find the character in the string after the index at which the previous character was found
    indexFound = s.find(ch, indexFound + 1);

    // If the character was found, increment instances of the character
    if(indexFound != (int32_t)string::npos) {
      instances++;
    }
  } while(indexFound != (int32_t)string::npos);

  return instances;
}

string trimmed(const string s, const char ch) {
  int pos = s.find_first_not_of(ch);
  int count = s.find_last_not_of(ch) - pos + 1;
  return s.substr(pos, count);
}

// Remove all whitespace from a string
void Remove_Whitespace(string* str) {
	// Move all whitespace down to the end of the string,
	// and get an iterator to the beginning of the area to erase
	auto remove_begin = remove_if(str->begin(), str->end(),
		[](unsigned char x) { return isspace(x); });
	// Erase all whitespace elements in the string
	str->erase(remove_begin, str->end());
}

void Scale(float* ar, uint32_t arlen, float target) {
	// Sum up all elements and find the ratio between current sum and target
	float total = accumulate(ar, ar + arlen, 0.0);
	float ratio = target / total;
	// Multiply every element in the function by the ratio
	auto mult_by_ratio = [ratio](float& f) { f *= ratio; };
	for_each(ar, ar + arlen, mult_by_ratio);
}

struct nk_vec2 Center(struct nk_rect parent, struct nk_vec2 child_dimensions) {
	struct nk_vec2 child_coords;
	child_coords.x = parent.x + ((parent.w - child_dimensions.x) / 2);
	child_coords.y = parent.y + ((parent.h - child_dimensions.y) / 2);
	return child_coords;
}

bool Vec2_Cmp(struct nk_vec2 a, struct nk_vec2 b) {
	return a.x == b.x && a.y == b.y;
}

string Rect_Str(struct nk_rect rect) {
	return to_string(rect.x) + ", " + to_string(rect.y) + ", " +
	to_string(rect.w) + ", " + to_string(rect.h);
}

string Vec2_Str(struct nk_vec2 vec) {
	ostringstream sout;
	sout << '(' << vec.x << ", " << vec.y << ')';
	return sout.str();
}

//string Color_Str(struct nk_color color) {
//	ostringstream sout;
//	sout << hex;
//	sout << "(" << (int)color.r << ", " << (int)color.g << ", "
//	<< (int)color.b << ", " << (int)color.a << ")";
//	return sout.str();
//}

bool Valid_IP(string ip) {
	(void)ip;	// Bypass compiler warning
	uint8_t totalChunks = 0;
	//string* numberChunks = Parse_Punctuated_List(ip, totalChunks, '.');
	// Ip address must have 4 number chunks in it
	if(totalChunks != 4) {
		return false;
	}
	uint32_t intConverted;
	for(uint8_t i = 0; i < totalChunks; i++) {
		intConverted = 0;//stoi(numberChunks[i]);
		// Each number chunk in the IP address must be between 0 and 255
		if(intConverted > 255) {
			return false;
		}
	}
	// If we got to this point, each chunk in the ip is valid
	return true;
}

string Full_Sytem_IP() {
	// Linux specific includes {{{
	#ifdef __linux__
	string ip = system_output("hostname -I");
	if (ip.find("\n") != std::string::npos) {
		ip = ip.substr(0, ip.find("\n"));
	}
	return ip;
	#endif
	// }}} Windows specific includes {{{
	#ifdef _WIN64
	return System_IP_V4();
	#endif
}

string System_IP_V4() {
	// Linux specific includes {{{
	#ifdef __linux__
	string ip = system_output("hostname -I");
	if (ip.find(' ') != string::npos) {
		ip = ip.substr(0, ip.find(' '));
	}
	return ip;
	#endif
	// }}} Windows specific includes {{{
	#ifdef _WIN64
	return system_output("ipconfig | grep IPv4 | awk '{print $14}'");
	#endif
	// }}}
}

void Delete_Thread(thread** th) {
	(*th)->join();
	delete *th;
	*th = NULL;
}
