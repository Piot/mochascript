#include <mocha/vector.h>
#include <mocha/object.h>
#include <stdlib.h>

void mocha_vector_init(mocha_vector* self, const struct mocha_object* args[], size_t count)
{
	size_t octet_count = sizeof(mocha_object*) * count;
	self->objects = malloc(octet_count);
	self->count = count;
	memcpy(self->objects, args, octet_count);
}

mocha_boolean mocha_vector_equal(const mocha_vector* self, const mocha_vector* other)
{
	if (self->count != other->count) {
		return mocha_false;
	}

	for (size_t i = 0; i < self->count; ++i) {
		if (!mocha_object_equal(self->objects[i], other->objects[i])) {
			return mocha_false;
		}
	}

	return mocha_true;
}
