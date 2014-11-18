#include <mocha/parser.h>
#include <mocha/object.h>
#include <mocha/log.h>
#include <mocha/context.h>
#include <mocha/symbol.h>
#include <mocha/string.h>

#include <math.h>
#include <stdlib.h>

static mocha_boolean is_space(mocha_char ch)
{
	return mocha_strchr("\t\n\r, ", ch) != 0;
}

static mocha_boolean is_alpha(mocha_char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (mocha_strchr("_!#$*+-><=./?", ch) != 0);
}

static mocha_boolean is_numerical(mocha_char ch)
{
	return (ch >= '0' && ch <= '9');
}


static mocha_char read_char(mocha_parser* self)
{
	if (self->input > self->input_end) {
		MOCHA_LOG("ERROR: You read too far!");
		return -1;
	}
	mocha_char ch = *self->input++;

	return ch;
}

static void unread_char(mocha_parser* self, mocha_char c)
{
	if (self->input == self->input_buffer) {
		MOCHA_LOG("ERROR: You unread too far!");
	}
	self->input--;
	if (c != *self->input) {
		MOCHA_LOG("ERROR: You unread illegal char!");
	}
}


static mocha_char skip_space(mocha_parser* self)
{
	int ch;

	do {
		ch = read_char(self);
	} while (ch != 0 && is_space(ch));

	return ch;
}

static mocha_boolean is_separator(mocha_char ch)
{
	return mocha_strchr("(){}[]\'`\"", ch) != 0;
}

size_t mocha_string_length(const mocha_char* s)
{
	const mocha_char* p = s;
	size_t count = 0;
	while (!*p++ == 0) {
		count++;
	}
	return count;
}


static const mocha_object* parse_object(mocha_parser* self, mocha_error* error);

static int parse_array(mocha_parser* self, mocha_char end_char, mocha_error* error, const mocha_object* array[], size_t array_max_count)
{
	int count = 0;

	while (count < array_max_count) {
		mocha_char ch = skip_space(self);
		if (ch == end_char) {
			return count;
		} else if (!ch) {
			MOCHA_ERR(mocha_error_code_unexpected_end);
		}
		unread_char(self, ch);
		const mocha_object* o = parse_object(self, error);
		if (!o || error->code != 0) {
			return count;
		}
		array[count++] = o;
	}

	error->code = 1;
	return count;
}


static const mocha_object* parse_map(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[128];
	int count = parse_array(self, '}', error, args, 128);
	const mocha_object* o = mocha_values_create_map(&self->values, args, count);

	return o;
}

static const mocha_object* parse_vector(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[128];
	int count = parse_array(self, ']', error, args, 128);
	const mocha_object* o = mocha_values_create_vector(&self->values, args, count);
	return o;
}

static const mocha_object* parse_list(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[128];
	int count = parse_array(self, ')', error, args, 128);
	if (error->code != 0) {
		return 0;
	}

	const mocha_object* o = mocha_values_create_list(&self->values, args, count);
	return o;
}

static int parse_word(mocha_parser* self, mocha_char* char_buffer, int max_symbol_length, mocha_error* error)
{
	mocha_string word_buffer;

	word_buffer.string = char_buffer;
	word_buffer.count = 0;

	while (1) {
		mocha_char ch = read_char(self);
		if (is_space(ch) || is_separator(ch) || !ch) {
			if (is_separator(ch)) {
				unread_char(self, ch);
			}
			break;
		}
		if (word_buffer.count == max_symbol_length) {
			MOCHA_ERR(mocha_error_code_symbol_too_long);
		}
		word_buffer.string[word_buffer.count++] = ch;
	}

	return word_buffer.count;
}

static float mocha_atof(const char* s, mocha_boolean* worked)
{
	size_t len = strlen(s);
	size_t dot_pos = mocha_strchr(s, '.') - s;
	size_t inverse_dot_pos = (len - 1) - dot_pos;
	float result = 0;
	int number_position = -inverse_dot_pos;
	mocha_boolean negative = mocha_false;

	for (int i=len-1; i>=0; --i) {
		int ch = s[i];
		if (ch == '-') {
			negative = mocha_true;
		} else if (i == dot_pos) {

		} else if (is_numerical(ch)) {
			int v = (ch - '0');
			if (v != 0) {
				float factor = pow(10, number_position);
				// MOCHA_LOG("pos:%d Factor:%f", number_position, factor);
				result += (factor * v);
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

static const mocha_object* parse_number(mocha_parser* self, mocha_error* error)
{
	const int max_symbol_length = 64;
	mocha_char char_buffer[max_symbol_length];
	int length = parse_word(self, char_buffer, max_symbol_length, error);
	if (error->code != mocha_error_code_ok) {
		MOCHA_LOG("couldn't parse word");
		return 0;
	}

	mocha_string word_buffer;
	word_buffer.string = char_buffer;
	word_buffer.count = length;

	const char* s = mocha_string_to_c(&word_buffer);
	mocha_boolean is_floating_point = mocha_strchr(s, '.') != 0;
	const mocha_object* o;

	if (is_floating_point) {
		mocha_boolean worked;
		o = mocha_values_create_float(&self->values, mocha_atof(s, &worked));
		if (!worked) {
			MOCHA_ERR(mocha_error_code_illegal_float_number);
		}
	} else {
		o = mocha_values_create_integer(&self->values, atol(s));
	}
	return o;
}

static const mocha_object* parse_string(mocha_parser* self, mocha_error* error)
{
	const mocha_char* p = self->input;
	mocha_char temp[1024];
	size_t count = 0;
	mocha_char ch;
	while ((ch = read_char(self)) != '"') {
		if (!ch) {
			MOCHA_ERR(mocha_error_code_missing_end_of_string);
		}
		temp[count++] = ch;
	}
	temp[count] = 0;
	const mocha_object* o = mocha_values_create_string(&self->values, temp, count);

	return o;
}

static const mocha_object* create_symbol(mocha_values* values, const mocha_string* string)
{
	const mocha_object* o = mocha_values_create_symbol(values, string);

	return o;
}

static const mocha_object* parse_symbol(mocha_parser* self, mocha_error* error)
{
	const mocha_object* o;

	const int max_symbol_length = 64;
	mocha_char char_buffer[max_symbol_length];
	int length = parse_word(self, char_buffer, max_symbol_length, error);
	if (error->code != mocha_error_code_ok) {
		return 0;
	}

	mocha_string word_buffer;
	word_buffer.string = char_buffer;
	word_buffer.count = length;
	if (mocha_string_equal_str(&word_buffer, "true")) {
		o = mocha_values_create_boolean(&self->values, mocha_true);
	} else if (mocha_string_equal_str(&word_buffer, "false")) {
		o = mocha_values_create_boolean(&self->values, mocha_false);
	} else if (mocha_string_equal_str(&word_buffer, "nil")) {
		o = mocha_values_create_nil(&self->values);
	} else {
		o = create_symbol(&self->values, &word_buffer);
	}
	return o;
}

static const mocha_object* parse_keyword(mocha_parser* self, mocha_error* error)
{
	const int max_symbol_length = 64;
	mocha_char char_buffer[max_symbol_length];
	int length = parse_word(self, char_buffer, max_symbol_length, error);
	if (error->code != mocha_error_code_ok) {
		return 0;
	}

	const mocha_object* o = mocha_values_create_keyword(&self->values, char_buffer, length);
	// self->context = mocha_context_add(self->context, o, o);
	return o;
}

static const mocha_object* parse_unquote(mocha_parser* self, mocha_error* error)
{
	const mocha_object* do_not_eval = parse_object(self, error);

	mocha_string temp_string;
	mocha_string_init_from_c(&temp_string, "unquote");
	const mocha_object* quote_symbol = create_symbol(&self->values, &temp_string);

	const mocha_object* args[2];

	args[0] = quote_symbol;
	args[1] = do_not_eval;

	const mocha_object* l = mocha_values_create_list(&self->values, args, 2);

	return l;
}

static const mocha_object* parse_tick(mocha_parser* self, mocha_error* error)
{
	const mocha_object* do_not_eval = parse_object(self, error);

	mocha_string temp_string;
	mocha_string_init_from_c(&temp_string, "quote");
	const mocha_object* quote_symbol = create_symbol(&self->values, &temp_string);

	const mocha_object* args[2];

	args[0] = quote_symbol;
	args[1] = do_not_eval;

	const mocha_object* l = mocha_values_create_list(&self->values, args, 2);

	return l;
}

static const mocha_object* parse_object(mocha_parser* self, mocha_error* error)
{
	const mocha_object* o = 0;

	mocha_char first_char = skip_space(self);
	switch (first_char) {
		case 0:
			MOCHA_LOG("END!");
			o = 0;
			break;
		case ':':
			o = parse_keyword(self, error);
			break;
		case '{':
			o = parse_map(self, error);
			break;
		case '[':
			o = parse_vector(self, error);
			break;
		case '(':
			o = parse_list(self, error);
			break;
		case '\'':
		case '`':
			o = parse_tick(self, error);
			break;
		case '~':
			o = parse_unquote(self, error);
			break;
		case '\"':
			o = parse_string(self, error);
			break;
		default:
			if (first_char == '-' || first_char == '.') {
				mocha_char ch = read_char(self);
				unread_char(self, ch);
				unread_char(self, first_char);
				if (is_numerical(ch) || ch == '.') {
					o = parse_number(self, error);
				} else {
					o = parse_symbol(self, error);
				}
			} else if (is_numerical(first_char)) {
				unread_char(self, first_char);
				o = parse_number(self, error);
			} else if (is_alpha(first_char)) {
				unread_char(self, first_char);
				o = parse_symbol(self, error);
			} else {
				MOCHA_LOG("'%d' %c", first_char, first_char);
				MOCHA_ERR(mocha_error_code_unexpected_character);
			}
	}

	return o;
}

void mocha_parser_init(mocha_parser* self, mocha_context* context, const mocha_char* input, size_t input_length)
{
	self->input_buffer = malloc(sizeof(mocha_char) * input_length + 1);
	memcpy(self->input_buffer, input, sizeof(mocha_char) * input_length);
	self->input_buffer[input_length] = 0;

	self->input = self->input_buffer;
	self->input_end = self->input + input_length;
	mocha_values_init(&self->values);
	self->context = context;
}

const mocha_object* mocha_parser_parse(mocha_parser* self, mocha_error* error)
{
	const mocha_object* args[128];
	int count = parse_array(self, 0, error, args, 128);
	if (error->code != 0) {
		return 0;
	}

	const mocha_object* o = mocha_values_create_list(&self->values, args, count);

	return o;
}
