#include <mocha/number.h>
#include <mocha/log.h>

mocha_boolean mocha_number_equal(const mocha_number* a, const mocha_number* b)
{
	if (a->type != b->type) {
		return mocha_false;
	}

	switch (a->type) {
		case mocha_number_type_integer:
			return a->data.i == b->data.i;
		case mocha_number_type_float:
			return a->data.f == b->data.f;
		default:
			return mocha_false;
	}
}

mocha_boolean mocha_number_less(const mocha_number* a, const mocha_number* b)
{
	if (a->type != b->type) {
		return mocha_false;
	}

	switch (a->type) {
		case mocha_number_type_integer:
			return a->data.i < b->data.i;
		case mocha_number_type_float:
			return a->data.f < b->data.f;
		default:
			return mocha_false;
	}
}

float mocha_number_float(const mocha_number* self)
{
	switch (self->type) {
		case mocha_number_type_integer:
			return self->data.i;
		case mocha_number_type_float:
			return self->data.f;
	}
}
