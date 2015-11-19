#ifndef mocha_number_h
#define mocha_number_h

#include <mocha/types.h>

typedef enum mocha_number_type { mocha_number_type_integer, mocha_number_type_float } mocha_number_type;

typedef struct mocha_number {
	mocha_number_type type;
	union {
		float f;
		int i;
	} data;
} mocha_number;

void mocha_number_init_float(mocha_number* self, float f);
void mocha_number_init_int(mocha_number* self, int i);
float mocha_number_float(const mocha_number* self);
mocha_boolean mocha_number_equal(const mocha_number* a, const mocha_number* b);
mocha_boolean mocha_number_less(const mocha_number* a, const mocha_number* b);

#endif
