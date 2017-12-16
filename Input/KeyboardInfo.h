#ifndef KEYBOARDINFO_H_
#define KEYBOARDINFO_H_

#include <Utilities/Integer.h>
#include <Utilities/DynArr.h>

struct KeyboardInfo : public NXTElement {
        char c;
        Integer code;
        Integer state;
};

#endif
