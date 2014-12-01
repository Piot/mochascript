#include <mocha/symbol.h>

void mocha_symbol_init(mocha_symbol* self, const mocha_string* string)
{
	self->string = string;
}
