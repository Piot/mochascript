#include <mocha/object.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/type.h>

mocha_boolean mocha_object_equal(const mocha_object* a, const mocha_object* b)
{
	if (a->type != b->type) {
		return mocha_false;
	}

	switch (a->type) {
		case mocha_object_type_number:
			return mocha_number_equal(&a->data.number, &b->data.number);
		case mocha_object_type_string:
			return mocha_string_equal(&a->data.string, &b->data.string);
		case mocha_object_type_keyword:
			return mocha_string_equal(a->data.keyword.string, b->data.keyword.string);
		case mocha_object_type_boolean:
			return (a->data.b == b->data.b);
		case mocha_object_type_symbol:
			return mocha_false;
		case mocha_object_type_map:
			return mocha_map_equal(&a->data.map, &b->data.map);
		case mocha_object_type_vector:
			return mocha_vector_equal(&a->data.vector, &b->data.vector);
		case mocha_object_type_list:
			return mocha_list_equal(&a->data.list, &b->data.list);
		case mocha_object_type_nil:
			return mocha_true;
		case mocha_object_type_function:
			return a == b;
		case mocha_object_type_internal_function:
			return a == b;
	}
}

mocha_boolean mocha_object_less(const mocha_object* a, const mocha_object* b)
{
	if (a->type != b->type) {
		return mocha_false;
	}

	switch (a->type) {
		case mocha_object_type_number:
			return mocha_number_less(&a->data.number, &b->data.number);
		case mocha_object_type_string:
			return mocha_string_less(&a->data.string, &b->data.string);
		case mocha_object_type_keyword:
			return mocha_string_less(a->data.keyword.string, b->data.keyword.string);
		case mocha_object_type_boolean:
			return (a->data.b < b->data.b);
		case mocha_object_type_symbol:
			return mocha_false;
		case mocha_object_type_map:
			return mocha_false; // mocha_map_less(&a->data.map, &b->data.map);
		case mocha_object_type_vector:
			return mocha_false; // return mocha_vector_less(&a->data.vector, &b->data.vector);
		case mocha_object_type_list:
			return mocha_false; // return mocha_list_less(&a->data.list, &b->data.list);
		case mocha_object_type_nil:
			return mocha_true;
		case mocha_object_type_function:
			return mocha_false;
		case mocha_object_type_internal_function:
			return mocha_false;
	}
}

mocha_boolean mocha_object_boolean(const mocha_object* a)
{
	if (a->type == mocha_object_type_boolean) {
		return a->data.b;
	} else {
		MOCHA_LOG("ERROR!!!!!!!!");
		return mocha_false;
	}
}

mocha_boolean mocha_object_truthy(const mocha_object* a)
{
	if (a->type == mocha_object_type_nil) {
		return mocha_false;
	} else if (a->type == mocha_object_type_boolean) {
		return a->data.b;
	} else {
		return mocha_true;
	}
}

mocha_boolean mocha_object_is_nil(const mocha_object* a)
{
	return a->type == mocha_object_type_nil;
}

const mocha_map* mocha_object_map(const mocha_object* a)
{
	if (a->type == mocha_object_type_map) {
		return &a->data.map;
	}

	MOCHA_LOG("Error: wasn't map");
	return 0;
}

const mocha_list* mocha_object_list(const mocha_object* a)
{
	if (a->type == mocha_object_type_list) {
		return &a->data.list;
	}

	MOCHA_LOG("Error: wasn't list");
	return 0;
}

const mocha_vector* mocha_object_vector(const mocha_object* a)
{
	if (a->type == mocha_object_type_vector) {
		return &a->data.vector;
	}

	MOCHA_LOG("Error: wasn't vector");
	return 0;
}

const mocha_keyword* mocha_object_keyword(const mocha_object* a)
{
	if (a->type == mocha_object_type_keyword) {
		return &a->data.keyword;
	}

	MOCHA_LOG("Error: wasn't a keyword");
	return 0;
}

float mocha_object_float(const mocha_object* a)
{
	if (a->type == mocha_object_type_number) {
		return mocha_number_float(&a->data.number);
	}
	return -9999999.9999f;
}

int mocha_object_integer(const mocha_object* a)
{
	if (a->type == mocha_object_type_number) {
		return mocha_number_int(&a->data.number);
	}
	return -9999999;
}

size_t mocha_object_unsigned(const mocha_object* a)
{
	int v = mocha_object_integer(a);
	if (v < 0) {
		MOCHA_LOG("Unsigned problem");
		v = 0;
	}
	return v;
}

const mocha_function* mocha_object_function(const mocha_object* a)
{
	if (a->type == mocha_object_type_function) {
		return &a->data.function;
	}

	return 0;
}

mocha_boolean mocha_object_is_function(const mocha_object* a)
{
	return (a->type == mocha_object_type_function) || (a->type == mocha_object_type_internal_function);
}

mocha_boolean mocha_object_is_invokable(const mocha_object* a)
{
	return (a->object_type && a->object_type->invoke != 0) || (a->type == mocha_object_type_function);
}

mocha_boolean mocha_object_is_list(const mocha_object* a)
{
	return (a->type == mocha_object_type_list);
}
