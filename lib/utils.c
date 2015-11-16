#include <mocha/utils.h>
#include <mocha/map.h>
#include <mocha/values.h>
#include <mocha/string.h>

const struct mocha_object* mocha_utils_map_lookup_c_string(const mocha_map* self, mocha_values* values, const char* symbol_name)
{
	mocha_string str;
	mocha_string_init_from_c(&str, symbol_name);

	return mocha_utils_map_lookup_string(self, values, str.string, str.count);
}

const struct mocha_object* mocha_utils_map_lookup_string(const mocha_map* self, mocha_values* values, const mocha_char* str,
														 size_t count)
{
	const mocha_object* string_keyword = mocha_values_create_keyword(values, str, count);

	return mocha_map_lookup(self, string_keyword);
}
