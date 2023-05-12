#pragma once

#include "test_helper.h"
#include <soso/soso.h>

#include <stdbool.h>
#include <stdio.h>

static bool sosolver_verify_solve(void) {
	test_start();
	soso_deck_t deck;
	soso_game_t game;
	soso_ctx_t ctx;
	soso_shuffle(&deck, 43);
	soso_deal(&game, &deck);
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);
	check_eq(soso_solve(&ctx, &game), true);
	test_end();
}

static bool sosolver_run_tests(void) {
	printf("\nRunning Solitaire Solver tests:\n");
	int count = 0;
	int fail = 0;
	run_test(sosolver_verify_solve, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
