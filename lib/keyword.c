#include <mocha/keyword.h>

void mocha_keyword_init(mocha_keyword* self, const mocha_string* string)
{
	self->string = string;
	self->user_data = 0;
}
