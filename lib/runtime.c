#include <mocha/runtime.h>
#include <mocha/print.h>
#include <mocha/object.h>
#include <mocha/type.h>
#include <mocha/log.h>
#include <mocha/error.h>
#include <mocha/values.h>
#include <stdlib.h>


static const mocha_object* fn(mocha_runtime* self, const mocha_context* context, const mocha_object* arguments, const mocha_object* body)
{
	const mocha_object* r = mocha_values_create_function(self->values, self->context, arguments, body);

	return r;
}

MOCHA_FUNCTION(fn_func)
{
	const mocha_object* r = fn(runtime, context, arguments->objects[1], arguments->objects[2]);
	return r;
}

static const mocha_object* def(mocha_runtime* runtime, const mocha_context* context, const mocha_object* name, const mocha_object* body)
{
	mocha_error error;
	const mocha_object* eval = mocha_runtime_eval(runtime, body, &error);
	const mocha_context* new_context = mocha_context_add(context, name, eval);
	runtime->context = new_context;
	return eval;
}


MOCHA_FUNCTION(def_func)
{
	const mocha_object* eval = def(runtime, context, arguments->objects[1], arguments->objects[2]);
	return eval;
}

MOCHA_FUNCTION(defn_func)
{
	const mocha_object* func = fn(runtime, context, arguments->objects[2], arguments->objects[3]);
	def(runtime, context, arguments->objects[0], func);
	return func;
}

MOCHA_FUNCTION(if_func)
{
	mocha_error error;
	const mocha_object* condition = mocha_runtime_eval(runtime, arguments->objects[1], &error);
	if (condition->type != mocha_object_type_boolean) {
		MOCHA_LOG("Illegal condition type");
		return condition;
	}
	mocha_boolean satisfied = condition->data.b;
	int eval_index = satisfied ? 2 : 3;
	if (eval_index >= arguments->count) {
		const mocha_object* r = mocha_values_create_nil(runtime->values);
		return r;
	}
	const mocha_object* result = mocha_runtime_eval(runtime, arguments->objects[eval_index], &error);

	return result;
}

MOCHA_FUNCTION(let_func)
{
	mocha_error error;
	const mocha_object* assignments = mocha_runtime_eval(runtime, arguments->objects[1], &error);
	if (!assignments || assignments->type != mocha_object_type_vector) {
		MOCHA_LOG("must have vector in let!");
		return 0;
	}

	const mocha_vector* assignment_vector = &assignments->data.vector;
	if ((assignment_vector->count % 2) != 0) {
		MOCHA_LOG("Wrong number of assignments");
		return 0;
	}

	const mocha_context* new_context = context;
	for (size_t i=0; i<assignment_vector->count; i+=2) {
		const mocha_object* symbol = assignment_vector->objects[i];
		if (symbol->type != mocha_object_type_symbol) {
			MOCHA_LOG("must have symbol in let");
		}
		new_context = mocha_context_add(new_context, symbol, assignment_vector->objects[i+1]);
	}

	// mocha_context_print_debug("let context", new_context);
	mocha_runtime_push_context(runtime, new_context);
	const mocha_object* result = mocha_runtime_eval(runtime, arguments->objects[2], &error);
	mocha_runtime_pop_context(runtime);

	return result;
}

static void number_mul(mocha_number* r, const mocha_number* a, const mocha_number* b)
{
	if (a->type == mocha_number_type_integer && b->type == mocha_number_type_integer) {
		r->type = mocha_number_type_integer;
		r->data.i = a->data.i * b->data.i;
	} else {
		r->type = mocha_number_type_float;
		if (a->type == mocha_number_type_integer) {
			r->data.f = (float) a->data.i * b->data.f;
		} else {
			r->data.f =  a->data.f * (float) b->data.i;
		}
	}
}

MOCHA_FUNCTION(mul_func)
{
	mocha_number result;
	result.type = mocha_number_type_integer;
	result.data.i = 1;

	for (size_t c = 1; c < arguments->count; ++c) {
		number_mul(&result, &result, &arguments->objects[c]->data.number);
	}

	const mocha_object* r = mocha_values_create_number(runtime->values, result);

	return r;
}

static void number_add(mocha_number* r, const mocha_number* a, const mocha_number* b)
{
	if (a->type == mocha_number_type_integer && b->type == mocha_number_type_integer) {
		r->type = mocha_number_type_integer;
		r->data.i = a->data.i + b->data.i;
	} else {
		r->data.f = mocha_number_float(a) + mocha_number_float(b);
		r->type = mocha_number_type_float;
	}
}

static void number_dec(mocha_number* r, const mocha_number* a, const mocha_number* b)
{
	if (a->type == mocha_number_type_integer && b->type == mocha_number_type_integer) {
		r->data.i = a->data.i - b->data.i;
		r->type = mocha_number_type_integer;
	} else {
		r->data.f = mocha_number_float(a) - mocha_number_float(b);
		r->type = mocha_number_type_float;
	}
}

static void number_div(mocha_number* r, const mocha_number* a, const mocha_number* b)
{
	if (a->type == mocha_number_type_integer && b->type == mocha_number_type_integer) {
		r->type = mocha_number_type_integer;
		r->data.i = a->data.i / b->data.i;
	} else {
		r->type = mocha_number_type_float;
		if (a->type == mocha_number_type_integer) {
			r->data.f = (float) a->data.i / b->data.f;
		} else {
			r->data.f =  a->data.f / (float) b->data.i;
		}
	}
}

MOCHA_FUNCTION(add_func)
{
	mocha_number v;

	v.type = mocha_number_type_integer;
	v.data.i = 0;

	for (size_t c = 1; c < arguments->count; ++c) {
		number_add(&v, &v, &arguments->objects[c]->data.number);
	}

	const mocha_object* r = mocha_values_create_number(runtime->values, v);
	return r;
}

MOCHA_FUNCTION(dec_func)
{
	mocha_number v;

	v.type = mocha_number_type_integer;
	v.data.i = 0;

	int start_index = 1;
	if (arguments->count > 2) {
		start_index = 2;
		v = arguments->objects[1]->data.number;
	}

	for (size_t c = start_index; c < arguments->count; ++c) {
		number_dec(&v, &v, &arguments->objects[c]->data.number);
	}

	const mocha_object* r = mocha_values_create_number(runtime->values, v);

	return r;
}

MOCHA_FUNCTION(div_func)
{
	mocha_number r;
	r.type = mocha_number_type_integer;
	r.data.i = 1;

	int start_index = 1;
	if (arguments->count > 1) {
		start_index = 2;
		r = arguments->objects[1]->data.number;
	}
	for (size_t c = start_index; c < arguments->count; ++c) {
		number_div(&r, &r, &arguments->objects[c]->data.number);
	}

	const mocha_object* o = mocha_values_create_number(runtime->values, r);
	return o;
}

MOCHA_FUNCTION(equal_func)
{
	mocha_boolean result = mocha_true;

	const mocha_object* source = arguments->objects[1];
	for (size_t i=1; i<arguments->count; ++i) {
		const mocha_object* v = arguments->objects[i];
		if (!mocha_object_equal(source, v)) {
			result = mocha_false;
			break;
		}
	}

	const mocha_object* r = mocha_values_create_boolean(runtime->values, result);
	return r;
}

MOCHA_FUNCTION(case_func)
{
	mocha_error error;
	const mocha_object* compare_value = mocha_runtime_eval(runtime, arguments->objects[1], &error);
	for (size_t i = 2; i < arguments->count; i += 2) {
		const mocha_object* when_value = arguments->objects[i];
		if (mocha_object_equal(compare_value, when_value)) {
			const mocha_object* when_argument = mocha_runtime_eval(runtime, arguments->objects[i+1], &error);
			return when_argument;
		}
	}

	if ((arguments->count % 2) != 0) {
		const mocha_object* default_value = mocha_runtime_eval(runtime, arguments->objects[arguments->count-1], &error);
		return default_value;
	}

	return mocha_values_create_nil(runtime->values);
}

MOCHA_FUNCTION(and_func)
{
	if (arguments->count == 1) {
		return mocha_values_create_boolean(runtime->values, mocha_true);
	}

	size_t last_index = arguments->count - 1;
	for (size_t i=1; i < arguments->count; ++i) {
		const mocha_object* a = arguments->objects[i];
		if (!mocha_object_truthy(a)) {
			last_index = i;
			break;
		}
	}

	return arguments->objects[last_index];
}

MOCHA_FUNCTION(assoc_func)
{
	const mocha_object* map_object = arguments->objects[1];
	const mocha_object* key = arguments->objects[2];
	const mocha_object* value = arguments->objects[3];
	const mocha_map* map = &map_object->data.map;

	const mocha_object* result[128];
	memcpy(result, map->objects, sizeof(mocha_object*) * map->count);

	size_t total_count = map->count + 2;
	size_t overwrite_index = map->count;
	for (size_t i = 0; i<map->count; i+= 2) {
		if (mocha_object_equal(map->objects[i], key)) {
			total_count = map->count;
			overwrite_index = i;
			break;
		}
	}
	result[overwrite_index] = key;
	result[overwrite_index + 1] = value;

	const mocha_object* new_map = mocha_values_create_map(runtime->values, result, total_count);

	return new_map;
}

static const mocha_object* conj_map(mocha_values* values, const mocha_map* self, const mocha_map* arg)
{
	const mocha_object* result[128];
	memcpy(result, self->objects, sizeof(mocha_object*) * self->count);
	memcpy(result + self->count, arg->objects, sizeof(mocha_object*) * arg->count);
	size_t total_count = self->count + arg->count;
	const mocha_object* new_map = mocha_values_create_map(values, result, total_count);

	return new_map;
}

static const mocha_object* conj_vector(mocha_values* values, const mocha_vector* self, const mocha_object** args, size_t count)
{
	const mocha_object* result[128];
	memcpy(result, self->objects, sizeof(mocha_object*) * self->count);
	memcpy(result + self->count, args, sizeof(mocha_object*) * count);
	size_t total_count = self->count + count;
	const mocha_object* o = mocha_values_create_vector(values, result, total_count);

	return o;
}

static const mocha_object* conj_list(mocha_values* values, const mocha_list* self, const mocha_object** args, size_t count)
{
	const mocha_object* result[128];

	for (size_t i = 0; i < count; ++i) {
		result[(count - i) - 1] = args[i];
	}

	size_t total_count = count;
	if (self) {
		memcpy(result + count, self->objects, sizeof(mocha_object*) * self->count);
		total_count += self->count;
	}
	const mocha_object* new_list = mocha_values_create_list(values, result, total_count);

	return new_list;
}

MOCHA_FUNCTION(conj_func)
{
	const mocha_object* sequence = arguments->objects[1];
	const mocha_object* result;
	switch (sequence->type) {
		case mocha_object_type_list:
			result = conj_list(runtime->values, &sequence->data.list, &arguments->objects[2], arguments->count-2);
			break;
		case mocha_object_type_vector:
			result = conj_vector(runtime->values, &sequence->data.vector, &arguments->objects[2], arguments->count-2);
			break;
		case mocha_object_type_nil:
			result = conj_list(runtime->values, 0, &arguments->objects[2], arguments->count-2);
			break;
		case mocha_object_type_map:
			result = conj_map(runtime->values, &sequence->data.map, &arguments->objects[2]->data.map);
			break;
		default:
			break;
	}

	return result;
}

static const mocha_object* cons_vector(mocha_values* values, const mocha_vector* self, const mocha_object** args)
{
	const int count = 1;
	const mocha_object* result[128];
	memcpy(result, args, sizeof(mocha_object*) * count);
	memcpy(result + count, self->objects, sizeof(mocha_object*) * self->count);
	size_t total_count = self->count + count;
	const mocha_object* o = mocha_values_create_list(values, result, total_count);

	return o;
}

static const mocha_object* cons_list(mocha_values* values, const mocha_list* self, const mocha_object** args)
{
	const mocha_object* result[128];
	const int count = 1;
	memcpy(result, args, sizeof(mocha_object*) * count);
	memcpy(result + count, self->objects, sizeof(mocha_object*) * self->count);
	size_t total_count = self->count + count;
	const mocha_object* new_list = mocha_values_create_list(values, args, total_count);

	return new_list;
}


MOCHA_FUNCTION(cons_func)
{
	const mocha_object* sequence = arguments->objects[2];
	const mocha_object* result;
	switch (sequence->type) {
		case mocha_object_type_list:
			result = cons_list(runtime->values, &sequence->data.list, &arguments->objects[1]);
			break;
		case mocha_object_type_vector:
			result = cons_vector(runtime->values, &sequence->data.vector, &arguments->objects[1]);
			break;
		case mocha_object_type_nil: {
			result = mocha_values_create_list(runtime->values, &arguments->objects[1], 1);
		}
		break;
		case mocha_object_type_map:
			MOCHA_LOG("BAD MAP");
			result = 0;
			break;
		default:
			break;
	}

	return result;
}


static const mocha_object* rest_vector(mocha_values* values, const mocha_vector* self)
{
	const mocha_object* o;
	if (self->count > 0) {
		const mocha_object* result[128];
		memcpy(result, self->objects + 1, sizeof(mocha_object*) * (self->count - 1));
		size_t total_count = self->count - 1;
		o = mocha_values_create_vector(values, result, total_count);
	} else {
		o = mocha_values_create_nil(values);
	}

	return o;
}



MOCHA_FUNCTION(rest_func)
{
	const mocha_object* sequence = arguments->objects[1];
	const mocha_object* result;
	switch (sequence->type) {
		case mocha_object_type_list:
			result = 0;
			break;
		case mocha_object_type_vector:
			result = rest_vector(runtime->values, &sequence->data.vector);
			break;
		case mocha_object_type_nil:
			result = 0;
			break;
		case mocha_object_type_map:
			result = 0;
			break;
		default:
			break;
	}

	return result;
}

static const mocha_object* first_vector(mocha_values* context, const mocha_vector* self)
{
	return self->objects[0];
}

MOCHA_FUNCTION(first_func)
{
	const mocha_object* sequence = arguments->objects[1];
	const mocha_object* result;
	switch (sequence->type) {
		case mocha_object_type_list:
			result = 0;
			break;
		case mocha_object_type_vector:
			result = first_vector(runtime->values, &sequence->data.vector);
			break;
		case mocha_object_type_nil:
			result = 0;
			break;
		case mocha_object_type_map:
			result = 0;
			break;
		default:
			break;
	}

	return result;
}


MOCHA_FUNCTION(quote_func)
{
	return arguments->objects[1];
}

MOCHA_FUNCTION(unquote_func)
{
	mocha_error error;
	return mocha_runtime_eval(runtime, arguments->objects[1], &error);
}

MOCHA_FUNCTION(not_func)
{
	const mocha_object* argument = arguments->objects[1];
	if (argument->type != mocha_object_type_boolean) {
		// error->code = mocha_error_code_expected_boolean_value;
		return 0;
	}
	const mocha_object* o = mocha_values_create_boolean(runtime->values, !argument->data.b);
	return o;
}

MOCHA_FUNCTION(count_func)
{
	const mocha_object* sequence = arguments->objects[1];
	mocha_boolean is_empty = mocha_true;

	int count = 0;
	switch (sequence->type) {
		case mocha_object_type_list:
			count = sequence->data.list.count;
			break;
		case mocha_object_type_vector:
			count = sequence->data.vector.count;
			break;
		case mocha_object_type_nil:
			break;
		case mocha_object_type_map:
			count = sequence->data.map.count;
			break;
		default:
			break;
	}

	const mocha_object* o = mocha_values_create_integer(runtime->values, count);

	return o;
}

MOCHA_FUNCTION(empty_func)
{
	const mocha_object* sequence = arguments->objects[1];
	mocha_boolean is_empty = mocha_true;

	switch (sequence->type) {
		case mocha_object_type_list:
			break;
		case mocha_object_type_vector:
			is_empty = sequence->data.vector.count == 0;
			break;
		case mocha_object_type_nil:
			break;
		case mocha_object_type_map:
			break;
		default:
			break;
	}

	const mocha_object* empty = mocha_values_create_boolean(runtime->values, is_empty);

	return empty;

}

MOCHA_FUNCTION(nil_func)
{
	const mocha_object* o = arguments->objects[1];

	const mocha_object* result = mocha_values_create_boolean(runtime->values, o->type == mocha_object_type_nil);

	return result;
}

#define MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments) \
	static mocha_type name##_def; \
	name##_def.invoke = name##_func; \
	name##_def.eval_all_arguments = eval_arguments; \
	name##_def.is_macro = mocha_false; \

#define MOCHA_DEF_FUNCTION(name, eval_arguments) \
	MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments) \
	context = mocha_context_add_function(context, values, #name, &name##_def);

#define MOCHA_DEF_FUNCTION_EX(name, exported_name, eval_arguments) \
	MOCHA_DEF_FUNCTION_HELPER(name, eval_arguments) \
	context = mocha_context_add_function(context, values, exported_name, &name##_def);

static void bootstrap_context(mocha_runtime* self, mocha_values* values)
{
	const mocha_context* context = self->context;
	MOCHA_DEF_FUNCTION(def, mocha_false);
	MOCHA_DEF_FUNCTION(assoc, mocha_true);
	MOCHA_DEF_FUNCTION(conj, mocha_true);
	MOCHA_DEF_FUNCTION(cons, mocha_true);
	MOCHA_DEF_FUNCTION(first, mocha_true);
	MOCHA_DEF_FUNCTION(rest, mocha_true);
	MOCHA_DEF_FUNCTION(let, mocha_false);
	MOCHA_DEF_FUNCTION(defn, mocha_false);
	MOCHA_DEF_FUNCTION_EX(mul, "*", mocha_true);
	MOCHA_DEF_FUNCTION_EX(add, "+", mocha_true);
	MOCHA_DEF_FUNCTION_EX(dec, "-", mocha_true);
	MOCHA_DEF_FUNCTION_EX(div, "/", mocha_true);
	MOCHA_DEF_FUNCTION(and, mocha_true);
	MOCHA_DEF_FUNCTION_EX(equal, "=", mocha_true);
	MOCHA_DEF_FUNCTION_EX(empty, "empty?", mocha_true);
	MOCHA_DEF_FUNCTION(count, mocha_true);
	MOCHA_DEF_FUNCTION_EX(nil, "nil?", mocha_true);
	MOCHA_DEF_FUNCTION(fn, mocha_false);
	MOCHA_DEF_FUNCTION(if, mocha_false);
	MOCHA_DEF_FUNCTION(case, mocha_false);
	MOCHA_DEF_FUNCTION(quote, mocha_false);
	MOCHA_DEF_FUNCTION(unquote, mocha_false);
	MOCHA_DEF_FUNCTION(not, mocha_true);
	mocha_runtime_push_context(self, context);
}

static const mocha_object* invoke(mocha_runtime* self, const mocha_context* context, const mocha_object* fn, const mocha_list* arguments_list)
{
	const mocha_object* o = 0;
	if (fn->object_type->invoke != 0) {
		if (fn->type == mocha_object_type_function) {
			MOCHA_LOG("STRANGE INVOK!");
		}
		// mocha_print_object_debug(fn);
		o = fn->object_type->invoke(self, context, arguments_list);
	} else if (fn->type == mocha_object_type_function) {
		const mocha_list* args = &fn->data.function.arguments->data.list;
		if (arguments_list->count - 1 != args->count) {
			MOCHA_LOG("Illegal number of arguments: %d", (int)arguments_list->count - 1);
			return fn;
		}
		const mocha_context* new_context = fn->data.function.context;
		for (size_t arg_count = 0; arg_count < args->count; ++arg_count) {
			const mocha_object* arg = args->objects[arg_count];
			if (arg->type != mocha_object_type_symbol) {
				MOCHA_LOG("Must use symbols!");
				return 0;
			}
			new_context = mocha_context_add(new_context, arg, arguments_list->objects[1 + arg_count]);
		}
		mocha_runtime_push_context(self, new_context);
		mocha_error error;
		o = mocha_runtime_eval(self, fn->data.function.code, &error);
		if (fn->object_type->is_macro) {
			MOCHA_LOG("MACRO!");
			o = mocha_runtime_eval(self, o, &error);
		}
		mocha_runtime_pop_context(self);
	}

	return o;
}

void mocha_runtime_init(mocha_runtime* self)
{
	self->context = 0;
	const int max_depth = 1024;
	self->contexts = malloc(sizeof(const mocha_context*) * max_depth);
	self->stack_depth = 0;
	mocha_context* context = malloc(sizeof(mocha_context));
	mocha_context_init(context, 0);
	self->context = context;
	bootstrap_context(self, self->values);
}

void mocha_runtime_push_context(mocha_runtime* self, const mocha_context* context)
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

void mocha_runtime_pop_context(mocha_runtime* self)
{
	if (self->stack_depth == 0) {
		MOCHA_LOG("Error: Popped too much");
		return;
	}
	--self->stack_depth;
	self->context = self->contexts[self->stack_depth];
}


const struct mocha_object* mocha_runtime_eval(mocha_runtime* self, const struct mocha_object* o, mocha_error* error)
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
		mocha_list new_args;
		if (fn->object_type->eval_all_arguments) {
			const mocha_object* converted_args[32];
			converted_args[0] = l->objects[0];
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
			MOCHA_LOG("Invoke returned 0:");
			mocha_print_object_debug(fn);
		}
	} else {
		if (o->type == mocha_object_type_symbol) {
			o = mocha_context_lookup(self->context, o);
		}
	}

	return o;
}
