#include "parse_float.h"
#include "char_query.h"
#include <mocha/string.h>

float mocha_atof(const char* s, mocha_boolean* worked)
{
	size_t len = strlen(s);
	size_t dot_pos = mocha_strchr(s, '.') - s;
	size_t inverse_dot_pos = (len - 1) - dot_pos;
	float result = 0;
	int number_position = -inverse_dot_pos;
	mocha_boolean negative = mocha_false;
	unsigned long factor;

	int minimum_position = number_position;
	if (minimum_position < -6) {
		minimum_position = -6;
	}
	factor = 1000000;
	for (int i=0; i < -minimum_position; ++i) {
		factor /= 10;
	}

	for (int i=len-1; i>=0; --i) {
		int ch = s[i];
		if (ch == '-') {
			negative = mocha_true;
		} else if (ch == '+') {

		} else if (i == dot_pos) {

		} else if (mocha_char_is_numerical(ch)) {
			int v = (ch - '0');
			if (number_position >= minimum_position) {
				result += ((factor / 1000000.0f) * v);
				factor *= 10;
			}
			number_position++;
		} else {
			*worked = mocha_false;
			return 0;
		}
	}

	*worked = mocha_true;

	if (negative) {
		result = - result;
	}

	return result;
}
