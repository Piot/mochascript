#ifndef mocha_type_h
#define mocha_type_h

#include <mocha/object.h>

struct mocha_list;

typedef const mocha_object* (*mocha_type_invoke)(struct mocha_runtime* runtime, struct mocha_context* context, const struct mocha_list* arguments);
#define MOCHA_FUNCTION(NAME) const mocha_object* NAME (struct mocha_runtime* runtime, struct mocha_context* context, const struct mocha_list* arguments)

typedef struct mocha_type {
	mocha_type_invoke invoke;
} mocha_type;

#endif
