#include <mocha/list.h>
#include <mocha/object.h>
#include <mocha/log.h>
#include <stdlib.h>

void mocha_list_init(mocha_list* self, const mocha_object** args, size_t count)
{
	size_t octet_count = sizeof(mocha_object*) * count;
	self->objects = malloc(octet_count);
	self->count = count;
	memcpy(self->objects, args, octet_count);
}

mocha_boolean mocha_list_equal(const mocha_list* self, const mocha_list* other)
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
