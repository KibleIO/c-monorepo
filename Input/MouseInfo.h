#ifndef MOUSEINFO_H_
#define MOUSEINFO_H_

#include <Utilities/Integer.h>
#include <Utilities/DynArr.h>

const Integer MOUSELEFT = {0};
const Integer MOUSEMIDDLE = {1};
const Integer MOUSERIGHT = {2};
const Integer MOUSENOACTION = {3};

struct MouseInfo : public NXTElement {
        Integer x;
        Integer y;
        Integer button;
};

#endif