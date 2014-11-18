#include <mocha/parser.h>
#include <mocha/runtime.h>
#include <mocha/object.h>
#include <mocha/print.h>
#include <mocha/log.h>
#include <string.h>
#include <stdio.h>
#include <mocha/log.h>


#include <stdlib.h>

int read_line(mocha_char* s, int max_length)
{
	const int c_max_length = 1024;
	if (max_length > c_max_length) {
		max_length = c_max_length;
	}
	char c_input[c_max_length];

	int input_length = 0;
	while (input_length < max_length) {
		int c_char = fgetc(stdin);
		if (c_char == EOF) {
			return EOF;
		}
		s[input_length] = c_char;
		++input_length;
		if (c_char == 10) {
			break;
		}
	}

	return input_length;
}

static const mocha_object* parse_and_print(mocha_runtime* runtime, mocha_parser* parser, mocha_boolean print_only_last, mocha_error* error)
{
	const mocha_object* o = mocha_parser_parse(parser, error);
	runtime->context = parser->context;

	if (o && o->type == mocha_object_type_list) {
		const mocha_list* list = &o->data.list;
		for (int i = 0; i < list->count; ++i) {
			const mocha_object* r = mocha_runtime_eval(runtime, list->objects[i], error);
			if (r && (!print_only_last || i == list->count - 1)) {
				mocha_print_object_debug(r);
				MOCHA_OUTPUT(" ");
			}
		}
		MOCHA_LOG("");
	} else {
		o = 0;
	}

	return o;
}

static void repl(mocha_runtime* runtime, mocha_parser* parser, mocha_error* error)
{
	const int max_length = 1024;
	mocha_char input[max_length];

	while (1) {
		MOCHA_OUTPUT("repl=> ");
		int input_length = read_line(input, max_length);
		if (input_length == EOF) {
			MOCHA_LOG("EOF");
			break;
		}

		mocha_parser_init(parser, runtime->context, input, input_length);

		const mocha_object* o;
		o = parse_and_print(runtime, parser, mocha_false, error);

		if (error->code != mocha_error_code_ok) {
			mocha_error_show(error);
			mocha_error_init(error);
		}
	}
}

static const mocha_object* eval_file(mocha_runtime* runtime, mocha_parser* parser, const char* filename, mocha_error* error)
{
	FILE* fp = fopen(filename, "rt");
	if (!fp) {
		MOCHA_ERR_STRING(mocha_error_code_file_not_found, filename);
	}
	const int max_buffer_count = 128 * 1024;
	char* temp_buffer = malloc(max_buffer_count * sizeof(char));
	mocha_char* temp_input = malloc(max_buffer_count * sizeof(mocha_char));
	int character_count = fread(temp_buffer, 1, max_buffer_count, fp);
	for (int i=0; i<character_count; ++i) {
		temp_input[i] = temp_buffer[i];
	}
	temp_input[character_count] = 0;
	mocha_parser_init(parser, runtime->context, temp_input, character_count);
	const mocha_object* o = parse_and_print(runtime, parser, mocha_true, error);
	MOCHA_LOG("");
	free(temp_input);
	free(temp_buffer);


	return o;
}

int main(int argc, char* argv[])
{
	mocha_parser parser;
	mocha_runtime runtime;
	mocha_runtime_init(&runtime);

	if (argc > 1) {
		const char* filename = argv[1];
		eval_file(&runtime, &parser, filename, &runtime.error);
	} else {
		repl(&runtime, &parser, &runtime.error);
	}

	if (runtime.error.code != mocha_error_code_ok) {
		mocha_error_show(&runtime.error);
	}
}
