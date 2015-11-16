#ifndef mocha_utils_h
#define mocha_utils_h

#include <mocha/types.h>
#include <mocha/string.h>

struct mocha_object;
struct mocha_values;
struct mocha_map;

const struct mocha_object* mocha_utils_map_lookup_c_string(const struct mocha_map* self, struct mocha_values* values,
														   const char* symbol_name);
const struct mocha_object* mocha_utils_map_lookup_string(const struct mocha_map* self, struct mocha_values* values,
														 const mocha_char* str, size_t len);

#endif
