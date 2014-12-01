#ifndef mocha_char_query_h
#define mocha_char_query_h

#include <mocha/string.h>

mocha_boolean mocha_char_is_space(mocha_char ch);
mocha_boolean mocha_char_is_alpha(mocha_char ch);
mocha_boolean mocha_char_is_numerical(mocha_char ch);
mocha_boolean mocha_char_is_separator(mocha_char ch);

#endif
