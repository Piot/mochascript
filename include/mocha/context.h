#ifndef mocha_context_h
#define mocha_context_h

#include <mocha/types.h>

struct mocha_object;
struct mocha_type;
struct mocha_values;

typedef struct mocha_context {
	const struct mocha_object** objects;
	size_t count;
	const struct mocha_context* parent;
} mocha_context;

void mocha_context_print_debug(const char* debug_text, const mocha_context* self);

struct mocha_context* mocha_context_create(const mocha_context* self);
const struct mocha_object* mocha_context_lookup(const mocha_context* self, const struct mocha_object* o);
void mocha_context_add_or_replace(struct mocha_context* self, const struct mocha_object* key, const struct mocha_object* value);
void mocha_context_add(struct mocha_context* self, const struct mocha_object* key, const struct mocha_object* value);
void mocha_context_add_function(struct mocha_context* self, struct mocha_values* values, const char* name,
								const struct mocha_type* type);
void mocha_context_init(mocha_context* self, const mocha_context* parent);

#endif
