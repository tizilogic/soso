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
	// TODO: Remove and make API function out of this
	printf("\nMoves made:\n");
	const char *suits = "CDSH";
	int moves = 0;
	for (int i = 0; i < ctx.moves_top; ++i) {
		soso_move_t m = ctx.moves[i];
		if (m.to == m.from) {
			if (m.to == SOSO_STOCK_WASTE) {
				int draws = m.count;
				int inc = 0;
				for (int j = i + 1; i < ctx.moves_top; ++j) {
					if (ctx.moves[j].to == SOSO_STOCK_WASTE) {
						draws += ctx.moves[j].count;
						++inc;
						continue;
					}
					break;
				}
				printf("DR%d ", draws);
				i += inc;
			}
			else if (m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7)
				printf("F%d ", m.to - SOSO_TABLEAU1 + 1);
		}
		else if (m.to >= SOSO_FOUNDATION1C) {
			if (m.from == SOSO_STOCK_WASTE)
				printf("W%c ", suits[m.to - SOSO_FOUNDATION1C]);
			else
				printf("%d%c ", m.from - SOSO_TABLEAU1 + 1, suits[m.to - SOSO_FOUNDATION1C]);
		}
		else if (m.from == SOSO_STOCK_WASTE)
			printf("W%d ", m.to - SOSO_TABLEAU1 + 1);
		else if (m.from >= SOSO_FOUNDATION1C)
			printf("%c%d ", suits[m.from - SOSO_FOUNDATION1C], m.to - SOSO_TABLEAU1 + 1);
		else {
			printf("%d%d", m.from - SOSO_TABLEAU1 + 1, m.to - SOSO_TABLEAU1 + 1);
			if (m.count > 1) printf("-%d", m.count);
			printf(" ");
		}
		++moves;
	}
	printf("\nMoves: %d\n", moves);
	soso_ctx_destroy(&ctx);
	test_end();
}

static bool sosolver_verify_draw_three(void) {
	test_start();
	soso_deck_t deck;
	soso_game_t game;
	soso_ctx_t ctx;
	soso_shuffle(&deck, 42);
	soso_deal(&game, &deck);
	soso_ctx_init(&ctx, 3, 100000, NULL, NULL, NULL);
	check_true(soso_solve(&ctx, &game));
	test_end();
}

static bool sosolver_verify_solvable_ratio(void) {
	test_start();
	int solved = 0;
	for (int i = 0; i < 10; ++i) {
		soso_deck_t deck;
		soso_game_t game;
		soso_ctx_t ctx;
		soso_shuffle(&deck, 43 + i);
		soso_deal(&game, &deck);
		soso_ctx_init(&ctx, 1, 50000, NULL, NULL, NULL);
		if (soso_solve(&ctx, &game)) ++solved;
		soso_ctx_destroy(&ctx);
	}
	check_true(solved >= 5);
	test_end();
}

static bool sosolver_run_tests(void) {
	printf("\nRunning Solitaire Solver tests:\n");
	int count = 0;
	int fail = 0;
	run_test(sosolver_verify_solve, &count, &fail);
	run_test(sosolver_verify_draw_three, &count, &fail);
	run_test(sosolver_verify_solvable_ratio, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
