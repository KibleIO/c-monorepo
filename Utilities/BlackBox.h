#ifndef BLACKBOX_H_
#define BLACKBOX_H_

#include <iostream>
#include <cstring>

using namespace std;

int* count_num_diff (char**, int*, int);

int  find_smallest  (char**, int*, int);

int  get_color      (int, char**, int, int *);

int  handle_section (char**, int*, int, int, int, int, char*, int, char*, int, char**);

//So this basically just takes in a few difference buffers and combines them into one difference buffer
int  black_box      (char**, int*, char**, int, char*);

int  handle_section (char**, int*, int, int, int, int, char*, int, char*, int);

//So this basically just takes in a few difference buffers and combines them into one difference buffer
int  black_box      (char**, int*, int, char*);

#endif /* BLACKBOX_H_ */