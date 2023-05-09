#pragma once

#include "test_helper.h"
#include <soso/soso.h>

#include <stdbool.h>
#include <stdio.h>

static soso_int_t make_card(soso_int_t suit, soso_int_t value) {
    return (suit << 5) | value;
}

static bool sohelper_verify_card(void) {
	soso_int_t c = make_card(3, 10);
	if (soso_internal_ccolor(c) != SOSO_RED) test_fail();
	if (soso_internal_csuit(c) != 3) test_fail();
	if (soso_internal_cvalue(c) != 10) test_fail();
	return true;
}

static bool sohelper_verify_valid(void) {
	soso_game_t game = (soso_game_t){.foundation_top = {0, 2, 4, 6},
	                                 .stock = {0, 1, 2, 3, 4},
	                                 .stock_count = 5,
	                                 .stock_cur = 0};
	if (!soso_internal_foundation_valid(make_card(0, 0), game.foundation_top)) test_fail();
	if (!soso_internal_foundation_valid(make_card(1, 2), game.foundation_top)) test_fail();
	if (!soso_internal_foundation_valid(make_card(2, 4), game.foundation_top)) test_fail();
	if (!soso_internal_foundation_valid(make_card(3, 6), game.foundation_top)) test_fail();
	if (soso_internal_get_waste_card(&game) != 0) test_fail();
    if (!soso_internal_tableau_valid(0, make_card(1, 1))) test_fail();
	return true;
}

static bool sohelper_run_tests(void) {
	printf("Running Solitaire Helper tests:\n");
	int count = 0;
	int fail = 0;
	run_test(sohelper_verify_card, &count, &fail);
	run_test(sohelper_verify_valid, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
