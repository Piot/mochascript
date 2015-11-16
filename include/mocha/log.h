#ifndef mocha_log_h
#define mocha_log_h

#include <stdio.h>

#define MOCHA_OUTPUT(...)                                                                                                        \
	{                                                                                                                            \
		printf(__VA_ARGS__);                                                                                                     \
		fflush(stdout);                                                                                                          \
	}

#define MOCHA_LOG(...)                                                                                                           \
	{                                                                                                                            \
		printf(__VA_ARGS__);                                                                                                     \
		printf("\n");                                                                                                            \
		fflush(stdout);                                                                                                          \
	}

#endif
