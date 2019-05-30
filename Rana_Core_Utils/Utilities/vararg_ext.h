#ifndef VARARG_EXT_H_
#define VARARG_EXT_H_

#include <cstdarg>

#define va_array(/*Type[]*/ array, /*int*/ count, Type) {\
va_list list;\
va_start(list, count);\
for(unsigned int i = 0; i < count; i++) {\
array[i] = va_arg(list, Type);\
}\
va_end(list);\
}

#endif
