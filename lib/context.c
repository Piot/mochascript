#include <mocha/context.h>
#include <mocha/object.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <mocha/values.h>

#include <stdlib.h>

static void mocha_context_print_debug_internal(const char* debug_text, const mocha_context* self, int tab)
{
	if (self->parent) {
		mocha_context_print_debug_internal(debug_text, self->parent, tab + 1);
		return;
	}

	MOCHA_LOG("level:%d, context count:%zu", tab, self->count);
	for (size_t i = 0; i < self->count; i += 2) {
		const mocha_object* key = self->objects[i];
		const mocha_object* value = self->objects[i + 1];
		mocha_print_object_debug(key);
		MOCHA_OUTPUT(" : ");
		mocha_print_object_debug(value);
		MOCHA_LOG("");
	}
}

void mocha_context_print_debug(const char* debug_text, const mocha_context* self)
{
	MOCHA_LOG("--------- CONTEXT debug: '%s'", debug_text);
	mocha_context_print_debug_internal(debug_text, self, 0);
	MOCHA_LOG("----------------------------");
}

mocha_context* mocha_context_create(const mocha_context* self)
{
	mocha_context* context = malloc(sizeof(mocha_context));
	mocha_context_init(context, self);

	return context;
}

void mocha_context_add(mocha_context* self, const mocha_object* key, const mocha_object* value)
{
	if (!key) {
		MOCHA_LOG("ADD: key is null");
		return;
	}
	if (!value) {
		MOCHA_LOG("ADD: value is null");
		return;
	}

	self->objects[self->count] = key;
	self->objects[self->count + 1] = value;
	self->count += 2;
}

void mocha_context_add_function(mocha_context* self, mocha_values* values, const char* name, const struct mocha_type* type)
{
	mocha_string string;
	mocha_string_init_from_c(&string, name);
	const mocha_object* key = mocha_values_create_symbol(values, &string);
	const mocha_object* value = mocha_values_create_internal_function(values, type, name);
	mocha_context_add(self, key, value);
}

const mocha_object** internal_context_lookup(const mocha_context* self, const mocha_object* o)
{
	if (!o) {
		MOCHA_LOG("Can not lookup with null key!");
		return 0;
	}
	for (size_t i = 0; i < self->count; i += 2) {
		const mocha_object* key = self->objects[i];
		if (!key) {
			MOCHA_LOG("key is null!");
			mocha_print_object_debug(o);
			return 0;
		}
		if (key->type == mocha_object_type_symbol && o->type == mocha_object_type_symbol) {
			if (mocha_string_equal(o->data.symbol.string, key->data.symbol.string)) {
				return &self->objects[i + 1];
			}
		} else if (key->type == mocha_object_type_keyword && o->type == mocha_object_type_keyword) {
			if (mocha_string_equal(o->data.keyword.string, key->data.keyword.string)) {
				return &self->objects[i + 1];
			}
		}
	}

	if (self->parent) {
		return internal_context_lookup(self->parent, o);
	}

	return 0;
}

void mocha_context_add_or_replace(mocha_context* self, const mocha_object* key, const mocha_object* value)
{
	const mocha_object** found_value = internal_context_lookup(self, key);
	if (found_value) {
		*found_value = value;
	} else {
		mocha_context_add(self, key, value);
	}
}

const mocha_object* mocha_context_lookup(const mocha_context* self, const mocha_object* o)
{
	const mocha_object** found_value = internal_context_lookup(self, o);
	if (found_value) {
		return *found_value;
	}
	return 0;
}

void mocha_context_init(mocha_context* self, const mocha_context* parent)
{
	self->parent = parent;
	self->objects = malloc(sizeof(mocha_object*) * 1024);
	self->count = 0;
}
