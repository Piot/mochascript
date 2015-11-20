#ifndef mocha_object_h
#define mocha_object_h

#include <mocha/list.h>
#include <mocha/map.h>
#include <mocha/vector.h>
#include <mocha/string.h>
#include <mocha/symbol.h>
#include <mocha/keyword.h>
#include <mocha/types.h>
#include <mocha/function.h>
#include <mocha/number.h>

#include "object_type.h"

struct mocha_type;

typedef struct mocha_object {
	mocha_object_type type;
	union {
		mocha_list list;
		mocha_map map;
		mocha_vector vector;
		mocha_number number;
		mocha_boolean b;
		mocha_string string;
		mocha_symbol symbol;
		mocha_keyword keyword;
		mocha_function function;
	} data;
	const struct mocha_type* object_type;
	const char* debug_string;
} mocha_object;

mocha_boolean mocha_object_boolean(const mocha_object* a);
mocha_boolean mocha_object_equal(const mocha_object* a, const mocha_object* b);
mocha_boolean mocha_object_less(const mocha_object* a, const mocha_object* b);
mocha_boolean mocha_object_truthy(const mocha_object* a);

const mocha_map* mocha_object_map(const mocha_object* a);
const mocha_list* mocha_object_list(const mocha_object* a);
const mocha_vector* mocha_object_vector(const mocha_object* a);
float mocha_object_float(const mocha_object* a);

mocha_boolean mocha_object_is_invokable(const mocha_object* a);
mocha_boolean mocha_object_is_function(const mocha_object* a);

#endif
