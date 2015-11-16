#include <mocha/string.h>
#include <mocha/log.h>
#include <stdlib.h>

mocha_boolean mocha_string_equal_str(const mocha_string* self, const char* cstr)
{
	const mocha_char* p = self->string;
	const char* s = cstr;

	size_t c_len = strlen(cstr);
	if (self->count != c_len) {
		return mocha_false;
	}
	for (size_t i = 0; i < c_len; ++i) {
		if (s[i] != p[i]) {
			return mocha_false;
		}
	}

	return mocha_true;
}

mocha_boolean mocha_string_equal(const mocha_string* a, const mocha_string* b)
{
	if (a->count != b->count) {
		return mocha_false;
	}

	for (size_t i = 0; i < a->count; ++i) {
		if (a->string[i] != b->string[i]) {
			return mocha_false;
		}
	}

	return mocha_true;
}

mocha_boolean mocha_string_less(const mocha_string* a, const mocha_string* b)
{
	if (a->count > b->count) {
		return mocha_false;
	}

	for (size_t i = 0; i < a->count; ++i) {
		if (a->string[i] > b->string[i]) {
			return mocha_false;
		}
	}

	return mocha_true;
}

const char* mocha_string_to_c(const mocha_string* s)
{
	static char temp[512];
	size_t len = s->count;
	for (size_t i = 0; i < len; ++i) {
		temp[i] = s->string[i];
	}
	temp[len] = 0;
	return temp;
}

void mocha_string_init(mocha_string* self, const mocha_char* s, size_t count)
{
	size_t octet_count = sizeof(mocha_char) * (count + 1);
	self->string = malloc(octet_count);
	self->count = count;
	memcpy(self->string, s, octet_count);
}

void mocha_string_init_from_c(mocha_string* self, const char* s)
{
	size_t count = strlen(s);
	size_t octet_count = sizeof(mocha_char) * (count + 1);
	self->string = malloc(octet_count);
	for (size_t i = 0; i < count + 1; ++i) {
		self->string[i] = s[i];
	}
	self->count = count;
}
