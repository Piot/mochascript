#include <mocha/values.h>
#include <mocha/object.h>
#include <mocha/type.h>
#include <mocha/log.h>
#include <mocha/runtime.h>
#include <mocha/print.h>
#include <mocha/utils.h>

#include <stdlib.h>

mocha_object* mocha_values_create_object(mocha_values* self, mocha_object_type object_type)
{
	mocha_object* o = malloc(sizeof(mocha_object));
	o->type = object_type;
	o->object_type = 0;

	return o;
}

const mocha_object* mocha_values_create_boolean(mocha_values* self, mocha_boolean value)
{
	mocha_object* o = mocha_values_create_object(self, mocha_object_type_boolean);
	o->data.b = value;
	return o;
}

const mocha_object* mocha_values_create_internal_function(mocha_values* self, const struct mocha_type* type, const char* name)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_internal_function);
	value->object_type = type;
	value->debug_string = name;

	return value;
}

const mocha_object* mocha_values_create_symbol(mocha_values* self, const mocha_string* string)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_symbol);
	mocha_string* copy = malloc(sizeof(mocha_string));

	mocha_string_init(copy, string->string, string->count);
	value->data.symbol.string = copy;
	return value;
}

const mocha_object* mocha_values_create_map(mocha_values* self, const mocha_object** args, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_map);
	mocha_map_init(&value->data.map, args, count);
	value->object_type = &self->map_def;

	return value;
}

const mocha_object* mocha_values_create_vector(mocha_values* self, const mocha_object** args, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_vector);
	mocha_vector_init(&value->data.vector, args, count);

	return value;
}

const mocha_object* mocha_values_create_list(mocha_values* self, const mocha_object** args, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_list);
	mocha_list_init(&value->data.list, args, count);

	return value;
}

const mocha_object* mocha_values_create_float(mocha_values* self, float v)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_number);
	value->data.number.type = mocha_number_type_float;
	value->data.number.data.f = v;

	return value;
}

const mocha_object* mocha_values_create_integer(mocha_values* self, int v)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_number);
	value->data.number.type = mocha_number_type_integer;
	value->data.number.data.i = v;

	return value;
}

const mocha_object* mocha_values_create_string_from_cstr(mocha_values* self, const char* s)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_string);
	mocha_string_init_from_c(&value->data.string, s);

	return value;
}

const struct mocha_object* mocha_values_create_string(mocha_values* self, const mocha_char* s, size_t count)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_string);
	mocha_string_init(&value->data.string, s, count);

	return value;
}

const mocha_object* mocha_values_create_function(mocha_values* self, const struct mocha_context* context,
												 const mocha_object* name, const mocha_object* arguments,
												 const mocha_object* body)
{
	static mocha_type fn_type;
	fn_type.eval_all_arguments = mocha_true;
	fn_type.invoke = 0;

	mocha_object* r = mocha_values_create_object(self, mocha_object_type_function);
	r->object_type = &fn_type;
	r->data.function.arguments = arguments;
	r->data.function.code = body;
	mocha_context* context_with_own_name = mocha_context_create(context);
	mocha_context_add(context_with_own_name, name, r);
	// mocha_context_print_debug("function context", context_with_own_name);
	r->data.function.context = context_with_own_name;
	return r;
}

const mocha_object* mocha_values_create_macro(mocha_values* self, const struct mocha_context* context, const mocha_object* name,
											  const mocha_object* arguments, const mocha_object* body)
{
	static mocha_type macro_type;
	macro_type.eval_all_arguments = mocha_false;
	macro_type.invoke = 0;

	mocha_object* r = mocha_values_create_object(self, mocha_object_type_function);
	r->object_type = &macro_type;
	r->data.function.arguments = arguments;
	r->data.function.code = body;
	mocha_context* context_with_own_name = mocha_context_create(context);
	mocha_context_add(context_with_own_name, name, r);
	// mocha_context_print_debug("function context", context_with_own_name);
	r->data.function.context = context_with_own_name;
	return r;
}

const struct mocha_object* mocha_values_create_nil(mocha_values* self)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_nil);
	return value;
}

const struct mocha_object* mocha_values_create_number(mocha_values* self, mocha_number number)
{
	mocha_object* value = mocha_values_create_object(self, mocha_object_type_number);
	value->data.number = number;

	return value;
}

MOCHA_FUNCTION(keyword_func)
{
	const mocha_object* argument = arguments->objects[1];
	if (argument->type == mocha_object_type_map) {
		const mocha_object* value = mocha_map_lookup(&argument->data.map, arguments->objects[0]);
		if (value) {
			return value;
		} else {
			if (arguments->count == 3) {
				return arguments->objects[2];
			} else {
				MOCHA_LOG("ERROR");
				return 0;
			}
		}
	}

	return mocha_values_create_nil(runtime->values);
}

MOCHA_FUNCTION(map_func)
{
	const mocha_object* argument = arguments->objects[1];
	const mocha_object* map_self = arguments->objects[0];

	if (argument->type == mocha_object_type_keyword) {
		const mocha_object* value = mocha_map_lookup(&map_self->data.map, argument);
		if (value) {
			return value;
		}
	}

	return mocha_values_create_nil(runtime->values);
}

void mocha_values_init(mocha_values* self)
{
	self->keyword_def.invoke = keyword_func;
	self->keyword_def.eval_all_arguments = mocha_true;

	self->map_def.invoke = map_func;
	self->map_def.eval_all_arguments = mocha_true;

	self->keyword_contents = malloc(sizeof(mocha_string) * 1024);
	self->keyword_contents_index = 0;
}

const struct mocha_object* mocha_values_create_keyword(mocha_values* self, const mocha_char* s, size_t count)
{
	mocha_string key_string;
	mocha_string_init(&key_string, s, count);
	const mocha_object* found_value = 0;

	for (size_t i = 0; i < self->keyword_contents_index; i += 2) {
		const mocha_object* key = self->keyword_contents[i];
		if (mocha_string_equal(&key_string, &key->data.string)) {
			found_value = self->keyword_contents[i + 1];
			break;
		}
	}

	if (found_value == 0) {
		mocha_string* string = malloc(sizeof(mocha_string));
		mocha_string_init(string, s, count);

		const mocha_object* key = mocha_values_create_string(self, s, count);
		self->keyword_contents[self->keyword_contents_index++] = key;

		mocha_object* value = mocha_values_create_object(self, mocha_object_type_keyword);
		self->keyword_contents[self->keyword_contents_index++] = value;
		mocha_keyword_init(&value->data.keyword, string);
		value->object_type = &self->keyword_def;

		/*
		MOCHA_OUTPUT("HAven't seen this yet:");
		mocha_print_object_debug(value);
		MOCHA_LOG("");
		*/
		found_value = value;
	}

	return found_value;
}
