#include <mocha/map.h>
#include <mocha/object.h>
#include <mocha/log.h>
#include <mocha/print.h>
#include <stdlib.h>

void mocha_map_init(mocha_map* self, const mocha_object* args[], int count)
{
	if ((count % 2) != 0) {
		MOCHA_LOG("ERROR: Must have even numbers in map!");
		self->count = 0;
		return;
	}

	size_t octet_count = sizeof(mocha_object*) * count;
	self->objects = malloc(octet_count);
	self->count = count;
	memcpy(self->objects, args, octet_count);
}

const struct mocha_object* mocha_map_lookup(const mocha_map* self, const struct mocha_object* key)
{
	for (size_t i=0; i<self->count; i += 2) {
		if (mocha_object_equal(self->objects[i], key)) {
			return self->objects[i + 1];
		}
	}
	return 0;
}

mocha_boolean mocha_map_equal(const mocha_map* self, const mocha_map* other)
{
	if (self->count != other->count) {
		return mocha_false;
	}

	for (size_t i=0; i<other->count; i += 2) {
		const mocha_object* key = self->objects[i];
		const mocha_object* other_value = mocha_map_lookup(other, key);
		if (!other_value) {
			return mocha_false;
		}
		const mocha_object* value = self->objects[i + 1];
		if (!mocha_object_equal(value, other_value)) {
			return mocha_false;
		}
	}

	return mocha_true;
}
