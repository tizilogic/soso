#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ERROR_BUFFER_SIZE 4096

static void internal_run_test(bool (*func)(void), const char *name, int *count, int *fail) {
	++(*count);
	printf("Running Test '%s'", name);
	if (func())
		printf("success\n");
	else {
		++(*fail);
		printf("failed\n");
	}
}

#define run_test(t, c, f) internal_run_test(t, #t, c, f)

#define test_start()                                                                               \
	int count = 0;                                                                                 \
	int failed = 0;                                                                                \
	char error_buf[ERROR_BUFFER_SIZE];                                                             \
	memset(error_buf, 0, ERROR_BUFFER_SIZE);                                                       \
	int error_top = 0;

#define test_end()                                                                                 \
	{                                                                                              \
		if (failed > 0 && error_top > 0) printf("\nerror(s):\n%s\n", error_buf);                     \
		return failed == 0;                                                                        \
	}

#define test_abort()                                                                               \
	{                                                                                              \
		++failed;                                                                                  \
		printf("\ntest aborted @ %s:%d\n", __FILE__, __LINE__);                                    \
		test_end();                                                                                \
	}

#define append_error(msg)                                                                          \
	{                                                                                              \
		int n = ERROR_BUFFER_SIZE - error_top - 1;                                                 \
		if (n <= 0) test_abort();                                                                  \
		error_top += snprintf(&error_buf[error_top], n, "%s @ %s:%d\n", msg, __FILE__, __LINE__);  \
	}

#define check_eq(a, b)                                                                             \
	{                                                                                              \
		++count;                                                                                   \
		if ((a) != (b)) {                                                                          \
			append_error("check equal failed");                                                    \
			printf("X");                                                                           \
			++failed;                                                                              \
		}                                                                                          \
		else                                                                                       \
			printf(".");                                                                           \
	}

#define check_neq(a, b)                                                                            \
	{                                                                                              \
		++count;                                                                                   \
		if ((a) == (b)) {                                                                          \
			append_error("check not equal failed");                                                \
			printf("X");                                                                           \
			++failed;                                                                              \
		}                                                                                          \
		else                                                                                       \
			printf(".");                                                                           \
	}

#define check_true(cond)                                                                            \
	{                                                                                              \
		++count;                                                                                   \
		if (!(cond)) {                                                                          \
			append_error("check true failed");                                                \
			printf("X");                                                                           \
			++failed;                                                                              \
		}                                                                                          \
		else                                                                                       \
			printf(".");                                                                           \
	}
