#pragma once

#include <stdbool.h>
#include <stdio.h>

static void internal_run_test(bool (*func)(void), const char *name, int *count, int *fail) {
	++(*count);
	printf("Running Test '%s'...", name);
	if (func())
		printf("success\n");
	else {
		++(*fail);
		printf("failed\n");
	}
}

#define run_test(t, c, f) internal_run_test(t, #t, c, f)

#define check_eq(a, b)                                                                             \
	if ((a) != (b)) printf("\nCheck equal failed @ %s:%d\n", __FILE__, __LINE__)
#define check_neq(a, b)                                                                            \
	if ((a) == (b)) printf("\nCheck not equal failed @ %s:%d\n", __FILE__, __LINE__)
#define test_fail()                                                                                \
	{                                                                                              \
		printf("\nTest failed @ %s:%d\n", __FILE__, __LINE__);                          \
		return false;                                                                              \
	}
