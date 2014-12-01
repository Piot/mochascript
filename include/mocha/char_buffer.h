#ifndef mocha_char_buffer_h
#define mocha_char_buffer_h

#include <mocha/string.h>

typedef struct mocha_char_buffer {
	const mocha_char* input;
	const mocha_char* input_end;
	mocha_char* input_buffer;
} mocha_char_buffer;

void mocha_char_buffer_init(mocha_char_buffer* self, const mocha_char* input, size_t input_length);
mocha_char mocha_char_buffer_read_char(mocha_char_buffer* self);
void mocha_char_buffer_unread_char(mocha_char_buffer* self, mocha_char c);
mocha_char mocha_char_buffer_skip_space(mocha_char_buffer* self);

#endif
