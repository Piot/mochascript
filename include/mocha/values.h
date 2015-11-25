#ifndef mocha_values_h
#define mocha_values_h

#include <mocha/string.h>
#include <mocha/number.h>
#include <mocha/type.h>
#include <mocha/map.h>

struct mocha_object;
struct mocha_context;

typedef struct mocha_values {
	mocha_type keyword_def;
	mocha_type map_def;

	const mocha_object** keyword_contents;
	size_t keyword_contents_index;
} mocha_values;

void mocha_values_init(mocha_values* self);
const struct mocha_object* mocha_values_create_boolean(mocha_values* values, mocha_boolean value);
const struct mocha_object* mocha_values_create_internal_function(mocha_values* self, const struct mocha_type* type,
																 const char* name);
const struct mocha_object* mocha_values_create_macro(mocha_values* self, const struct mocha_context* context,
													 const mocha_object* name, const mocha_object* arguments,
													 const mocha_object* body);
const struct mocha_object* mocha_values_create_function(mocha_values* self, const struct mocha_context* context,
														const mocha_object* name, const struct mocha_object* arguments,
														const struct mocha_object* body);
const struct mocha_object* mocha_values_create_symbol(mocha_values* self, const mocha_string* string);
const struct mocha_object* mocha_values_create_map(mocha_values* self, const struct mocha_object** args, size_t count);
const struct mocha_object* mocha_values_create_list(mocha_values* self, const struct mocha_object** args, size_t count);
const struct mocha_object* mocha_values_create_list_cons(mocha_values* self, const mocha_list* list, const mocha_object* a);
const struct mocha_object* mocha_values_create_vector(mocha_values* self, const struct mocha_object** args, size_t count);
const struct mocha_object* mocha_values_create_string_from_cstr(mocha_values* self, const char* s);
const struct mocha_object* mocha_values_create_string(mocha_values* self, const mocha_char* s, size_t count);
const struct mocha_object* mocha_values_create_float(mocha_values* self, float v);
const struct mocha_object* mocha_values_create_integer(mocha_values* self, int v);
const struct mocha_object* mocha_values_create_number(mocha_values* self, mocha_number v);
const struct mocha_object* mocha_values_create_nil(mocha_values* self);
const struct mocha_object* mocha_values_create_keyword(mocha_values* self, const mocha_char* s, size_t count);

#endif
