#include <mocha/char_buffer.h>
#include <mocha/log.h>
#include "char_query.h"

#include <stdlib.h>

void mocha_char_buffer_init(mocha_char_buffer* self, const mocha_char* input, size_t input_length)
{
	self->input_buffer = malloc(sizeof(mocha_char) * input_length + 1);
	memcpy(self->input_buffer, input, sizeof(mocha_char) * input_length);
	self->input_buffer[input_length] = 0;

	self->input = self->input_buffer;
	self->input_end = self->input + input_length;
}


static mocha_char skip_to_eol(mocha_char_buffer* self)
{
	int ch;

	do {
		ch = mocha_char_buffer_read_char(self);
	} while (ch != 0 && !mocha_char_is_eol(ch));

	return ch;
}

mocha_char mocha_char_buffer_read_char(mocha_char_buffer* self)
{
	if (self->input > self->input_end) {
		MOCHA_LOG("ERROR: You read too far!");
		return -1;
	}

	mocha_char ch = *self->input++;
	if (ch == ';') {
		ch = skip_to_eol(self);
	}

	return ch;
}

void mocha_char_buffer_unread_char(mocha_char_buffer* self, mocha_char c)
{
	if (self->input == self->input_buffer) {
		MOCHA_LOG("ERROR: You unread too far!");
	}
	self->input--;
	if (c != *self->input) {
		MOCHA_LOG("ERROR: You unread illegal char!");
	}
}

mocha_char mocha_char_buffer_skip_space(mocha_char_buffer* self)
{
	int ch;

	do {
		ch = mocha_char_buffer_read_char(self);
	} while (ch != 0 && mocha_char_is_space(ch));

	return ch;
}

