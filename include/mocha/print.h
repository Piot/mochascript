#ifndef mocha_print_h
#define mocha_print_h

struct mocha_object;

void mocha_print_object_debug(const struct mocha_object* o);
void mocha_print_object_debug_no_quotes(const struct mocha_object* o);

#endif
