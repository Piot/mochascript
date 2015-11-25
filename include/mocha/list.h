#ifndef mocha_list_h
#define mocha_list_h

#include <mocha/types.h>

struct mocha_object;

typedef struct mocha_list {
	const struct mocha_object** objects;
	size_t count;
} mocha_list;

void mocha_list_init(mocha_list* self, const struct mocha_object* args[], size_t count);
size_t mocha_list_init_prepare(mocha_list* self, size_t count);
mocha_boolean mocha_list_equal(const mocha_list* self, const mocha_list* other);

#endif
