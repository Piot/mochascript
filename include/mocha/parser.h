#ifndef mocha_parser_h
#define mocha_parser_h

#include <stddef.h>

#include <mocha/context.h>
#include <mocha/string.h>
#include <mocha/error.h>
#include <mocha/values.h>
#include <mocha/char_buffer.h>

typedef struct mocha_parser {
	mocha_char_buffer buffer;
	mocha_context* context;
	mocha_error error;
	mocha_values* values;
} mocha_parser;

void mocha_parser_init(mocha_parser* self, mocha_values* values, mocha_context* context, const mocha_char* input, size_t input_length);
const struct mocha_object* mocha_parser_parse(mocha_parser* self, mocha_error* error);

#endif
