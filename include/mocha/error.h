#ifndef mocha_error_h
#define mocha_error_h

#define MOCHA_ERR(err_number) error->code = err_number; return 0;
#define MOCHA_ERR_STRING(err_number, err_string) error->code = err_number; error->string = err_string; return 0;

typedef enum mocha_error_code {
	mocha_error_code_ok = 0,
	mocha_error_code_unexpected_end,
	mocha_error_code_symbol_too_long,
	mocha_error_code_missing_end_of_string,
	mocha_error_code_unexpected_character,
	mocha_error_code_file_not_found,
	mocha_error_code_fail,
	mocha_error_code_illegal_float_number,
    mocha_error_code_expected_list
} mocha_error_code;

typedef struct mocha_error {
	int line_number;
	const char* filename;
	mocha_error_code code;
	const char* string;
} mocha_error;

void mocha_error_show(mocha_error* self);
void mocha_error_init(mocha_error* self);

#endif
