#include "char_query.h"

mocha_boolean mocha_char_is_space(mocha_char ch)
{
	return mocha_strchr("\t\n\r, ", ch) != 0;
}

mocha_boolean mocha_char_is_alpha(mocha_char ch)
{
	return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (mocha_strchr("&_!#$*+-><=./?", ch) != 0);
}

mocha_boolean mocha_char_is_numerical(mocha_char ch)
{
	return (ch >= '0' && ch <= '9');
}

mocha_boolean mocha_char_is_separator(mocha_char ch)
{
	return mocha_strchr("(){}[]\'`\"", ch) != 0;
}

mocha_boolean mocha_char_is_eol(mocha_char ch)
{
	return ch == 10;
}
