#ifndef mocha_vector_h
#define mocha_vector_h

#include <mocha/types.h>

struct mocha_object;

typedef struct mocha_vector {
	const struct mocha_object** objects;
	size_t count;
} mocha_vector;

void mocha_vector_init(mocha_vector* self, const struct mocha_object* args[], size_t count);
mocha_boolean mocha_vector_equal(const mocha_vector* self, const mocha_vector* other);

#endif
