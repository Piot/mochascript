#include <mocha/runtime.h>
#include <mocha/print.h>
#include <mocha/log.h>
#include <mocha/values.h>
#include <stdlib.h>

void mocha_runtime_init(mocha_runtime* self, mocha_values* values)
{
	self->context = 0;
	const int max_depth = 1024;
	self->contexts = malloc(sizeof(const mocha_context*) * max_depth);
	self->stack_depth = 0;
	self->context = 0;
	self->values = values;
	mocha_error_init(&self->error);
}

static const mocha_object* invoke(mocha_runtime* self, mocha_context* context, const mocha_object* fn, const mocha_list* arguments_list)
{
	const mocha_object* o = 0;
	if (fn->object_type->invoke != 0) {
		o = fn->object_type->invoke(self, context, arguments_list);
	} else if (fn->type == mocha_object_type_function) {
		const mocha_list* args = &fn->data.function.arguments->data.list;
		if (arguments_list->count - 1 != args->count) {
			MOCHA_LOG("Illegal number of arguments: %d", (int)arguments_list->count - 1);
			return fn;
		}
		mocha_context* new_context = mocha_context_create(fn->data.function.context);
		// mocha_context_print_debug("function context:", new_context);
		for (size_t arg_count = 0; arg_count < args->count; ++arg_count) {
			const mocha_object* arg = args->objects[arg_count];
			if (arg->type != mocha_object_type_symbol) {
				MOCHA_LOG("Must use symbols!");
				return 0;
			}
			if (mocha_string_equal_str(arg->data.symbol.string, "&")) {
				const mocha_object* arg = args->objects[arg_count + 1];
				const mocha_object* list = mocha_values_create_list(self->values, &arguments_list->objects[1 + arg_count], args->count - arg_count);
				mocha_context_add(new_context, arg, list);
			} else {
				mocha_context_add(new_context, arg, arguments_list->objects[1 + arg_count]);
			}
		}
		mocha_runtime_push_context(self, new_context);
		o = mocha_runtime_eval(self, fn->data.function.code, &self->error);
		if (fn->object_type->is_macro) {
			o = mocha_runtime_eval(self, o, &self->error);
		}
		mocha_runtime_pop_context(self);
	}

	return o;
}

void mocha_runtime_push_context(mocha_runtime* self, mocha_context* context)
{
	// mocha_context_print_debug("pushing context", context);
	for (size_t i=0; i<context->count; ++i) {
		if (!context->objects[i]) {
			MOCHA_LOG("Pushed context is really bad (null object)");
			return;
		}
	}
	self->contexts[self->stack_depth++] = self->context;
	self->context = context;
}

void mocha_runtime_clear_contexts(mocha_runtime* self)
{
	while (self->stack_depth > 0) {
		mocha_runtime_pop_context(self);
	}
}

void mocha_runtime_pop_context(mocha_runtime* self)
{
	if (self->stack_depth == 0) {
		MOCHA_LOG("Error: Popped too much");
		return;
	}
	--self->stack_depth;
	self->context = self->contexts[self->stack_depth];
}

const struct mocha_object* mocha_runtime_eval_ex(mocha_runtime* self, const struct mocha_object* o, mocha_error* error, mocha_boolean eval_symbols)
{
	if (o->type == mocha_object_type_list) {
		const mocha_list* l = &o->data.list;
		if (l->count == 0) {
			return o;
		}
		const struct mocha_object* fn = mocha_runtime_eval(self, l->objects[0], error);
		if (!fn) {
			MOCHA_LOG("Couldn't find lookup:");
			mocha_print_object_debug(l->objects[0]);
			return 0;
		}
		mocha_boolean should_evaluate_arguments = mocha_true;
		if (fn->object_type) {
			should_evaluate_arguments = fn->object_type->eval_all_arguments;
		}
		mocha_list new_args;
		if (should_evaluate_arguments) {
			const mocha_object* converted_args[32];
			converted_args[0] = fn;
			for (size_t i = 1; i < l->count; ++i) {
				const struct mocha_object* arg = mocha_runtime_eval(self, l->objects[i], error);
				if (!arg) {
					MOCHA_LOG("Couldn't evaluate:");
					mocha_print_object_debug(l->objects[i]);
					return 0;
				}
				converted_args[i] = arg;
			}
			mocha_list_init(&new_args, converted_args, l->count);
			l = &new_args;
		}
		o = invoke(self, self->context, fn, l);
		if (!o) {
			mocha_print_object_debug(fn);
		}
	} else {
		if (o->type == mocha_object_type_map) {
			const mocha_map* m = &o->data.map;
			const mocha_object* converted_args[32];
			for (size_t i = 0; i < m->count; ++i) {
				const struct mocha_object* arg = mocha_runtime_eval(self, m->objects[i], error);
				if (!arg) {
					MOCHA_LOG("Couldn't evaluate:");
					mocha_print_object_debug(m->objects[i]);
					return 0;
				}
				converted_args[i] = arg;
			}
			o = mocha_values_create_map(self->values, converted_args, m->count);
		} else if (o->type == mocha_object_type_symbol) {
			o = mocha_context_lookup(self->context, o);
			if (eval_symbols && o) {
				o = mocha_runtime_eval(self, o, error);
			}
		}
	}

	return o;
}

const struct mocha_object* mocha_runtime_eval_commands(mocha_runtime* self, const struct mocha_object* o, mocha_error* error)
{
	if (o && o->type == mocha_object_type_list) {
		const mocha_list* list = &o->data.list;
		const mocha_object* result;
		for (int i = 0; i < list->count; ++i) {
			result = mocha_runtime_eval(self, list->objects[i], error);
		}
		return result;
	}

	MOCHA_ERR(mocha_error_code_expected_list);
}

const struct mocha_object* mocha_runtime_eval(mocha_runtime* self, const struct mocha_object* o, mocha_error* error)
{
	return mocha_runtime_eval_ex(self, o, error, mocha_false);
}

const struct mocha_object* mocha_runtime_eval_symbols(mocha_runtime* self, const struct mocha_object* o, mocha_error* error)
{
	return mocha_runtime_eval_ex(self, o, error, mocha_true);
}
