#ifndef mocha_runtime_h
#define mocha_runtime_h

#include <mocha/context.h>
#include <mocha/error.h>
#include <mocha/type.h>

struct mocha_object;
struct mocha_error;
struct mocha_values;

typedef struct mocha_runtime {
	mocha_context* context;
	mocha_context** contexts;
	int stack_depth;
	const struct mocha_object* nil;
	struct mocha_values* values;
	struct mocha_error error;
	void* user_data;
} mocha_runtime;

void mocha_runtime_init(mocha_runtime* self, struct mocha_values* values);

void mocha_runtime_clear_contexts(mocha_runtime* self);
void mocha_runtime_pop_context(mocha_runtime* self);
void mocha_runtime_push_context(mocha_runtime* self, mocha_context* context);

const struct mocha_object* mocha_runtime_eval(mocha_runtime* self, const struct mocha_object* o, struct mocha_error* error);
const struct mocha_object* mocha_runtime_eval_symbols(mocha_runtime* self, const struct mocha_object* o, struct mocha_error* error);
const struct mocha_object* mocha_runtime_eval_commands(mocha_runtime* self, const struct mocha_object* o, mocha_error* error);

void mocha_runtime_add_function(mocha_runtime* self, const char* name, mocha_type_invoke func);

#endif
