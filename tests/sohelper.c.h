#pragma once

#include "test_helper.h"
#include <soso/soso.h>

#include <stdbool.h>
#include <stdio.h>

static bool sohelper_verify_card(void) {
	soso_int_t c = soso_internal_make_card(3, 10);
	if (soso_internal_ccolor(c) != SOSO_RED) test_fail();
	if (soso_internal_csuit(c) != 3) test_fail();
	if (soso_internal_cvalue(c) != 10) test_fail();
	if (!soso_internal_valid_card(c)) test_fail();
	if (soso_internal_valid_card(soso_internal_make_card(3, 13))) test_fail();
	return true;
}

static bool sohelper_verify_valid(void) {
	soso_game_t game = (soso_game_t){
	    .foundation_top = {0, 2, 4, 6}, .stock = {0, 1, 2, 3, 4}, .stock_count = 5, .stock_cur = 0};
	if (!soso_internal_foundation_valid(soso_internal_make_card(0, 0), game.foundation_top))
		test_fail();
	if (!soso_internal_foundation_valid(soso_internal_make_card(1, 2), game.foundation_top))
		test_fail();
	if (!soso_internal_foundation_valid(soso_internal_make_card(2, 4), game.foundation_top))
		test_fail();
	if (!soso_internal_foundation_valid(soso_internal_make_card(3, 6), game.foundation_top))
		test_fail();
	if (soso_internal_get_waste_card(&game) != 0) test_fail();
	if (!soso_internal_tableau_valid(0, soso_internal_make_card(1, 1))) test_fail();
	return true;
}

static bool sohelper_verify_add_move(void) {
    soso_ctx_t ctx;
    soso_ctx_init(&ctx, 3, 100000, NULL, NULL, NULL);
    soso_move_t m = {.from = SOSO_STOCK_WASTE, .to = SOSO_TABLEAU1, .count = 1, .extra = SOSO_AUTO_MOVE};
    soso_internal_add_move(&ctx, m, true);
    if (ctx.automoves_count != 1) test_fail();
    if (ctx.moves_top != 1) test_fail();
    if (*(int *)&ctx.moves[0] != *(int *)&m) test_fail();
    soso_ctx_destroy(&ctx);
}

static bool sohelper_run_tests(void) {
	printf("\nRunning Solitaire Helper tests:\n");
	int count = 0;
	int fail = 0;
	run_test(sohelper_verify_card, &count, &fail);
	run_test(sohelper_verify_valid, &count, &fail);
	run_test(sohelper_verify_add_move, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
