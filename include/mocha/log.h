#ifndef mocha_log_h
#define mocha_log_h

#include <stdio.h>

typedef struct mocha_log {
	void (*log)(const char* string);
} mocha_log;

mocha_log g_mocha_log;

#define MOCHA_OUTPUT(...)                                                                                                        \
	{ \
		char _temp_str[1024];                                                                                                    \
		int _characters_written = sprintf(_temp_str, __VA_ARGS__); \
		if (g_mocha_log.log) {                                                                                         \
			g_mocha_log.log(_temp_str);                                                                                \
		} \
		fwrite(_temp_str, 1, _characters_written, stdout);                                                                                                     \
		fflush(stdout);                                                                                                          \
	}

#define MOCHA_LOG(...)                                                                                                           \
	{                                                                                                                            \
		char _temp_str[1024];                                                                                                    \
		sprintf(_temp_str, __VA_ARGS__); \
		if (g_mocha_log.log) {                                                                                         \
			g_mocha_log.log(_temp_str);                                                                                \
		} \
		puts(_temp_str); \
		fflush(stdout);                                                                                                          \
	}

#endif
