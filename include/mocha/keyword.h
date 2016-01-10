#ifndef mocha_keyword_h
#define mocha_keyword_h

#include <mocha/string.h>

typedef struct mocha_keyword {
	const mocha_string* string;
	void* user_data;
} mocha_keyword;

void mocha_keyword_init(mocha_keyword* self, const mocha_string* string);

#endif
