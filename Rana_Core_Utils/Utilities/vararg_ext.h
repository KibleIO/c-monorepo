#ifndef VARARG_EXT_H_
#define VARARG_EXT_H_

#include <cstdarg>

// Interpret the variadic arguments as a list of objects of the same type
#define va_array(/*Type[]*/ array, /*int*/ count, Type) {\
va_list list;\
va_start(list, count);\
for(unsigned int i = 0; i < count; i++) {\
array[i] = va_arg(list, Type);\
}\
va_end(list);\
}

// Interpret the variadic arguments using a character array,
// where individual characters in the array are used to decode the arguments
#define va_character_decoder(/* const char* */ code, /* int */ num_args, /* Type* */ decoder_arg, /* void (*)(Type*, char, va_list) */ decoder_function) {\
/* Start up the variadic list */\
va_list varargs;\
va_start(varargs, num_args);\
if (code) {\
/* Start loop control variables */\
char current_code = code[0];\
uint8_t i = 1;\
/* Call the decoder function for each character in the character pointer*/\
while (current_code != '\0') {\
decoder_function(decoder_arg, current_code, varargs);\
current_code = code[i++];\
}/* end while */\
}/* end if */\
va_end(varargs);\
}

#endif
