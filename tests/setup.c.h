#pragma once

#include "test_helper.h"
#include <soso/soso.h>

#include <stdbool.h>
#include <stdio.h>

static bool setup_verify_shuffle(void) {
	test_start();
	soso_deck_t deck;
	soso_shuffle(&deck, 42);
	bool illegal_card = false;
	bool shuffled = false;
	for (int i = 0; i < 52; ++i) {
		if ((deck.cards[i] & 0x0f) > 12) illegal_card = true;
		if (deck.cards[i] != ((i / 13) << 5) | (i % 13)) shuffled = true;
	}
	check_eq(illegal_card, false);
	check_eq(shuffled, true);
	test_end();
}

static bool setup_verify_deal(void) {
	test_start();
	soso_deck_t deck;
	for (soso_int_t i = 0; i < 4; ++i)
		for (soso_int_t j = 0; j < 13; ++j) {
			deck.cards[i * 13 + j] = soso_internal_make_card(i, j);
		}
	soso_game_t game;
	soso_deal(&game, &deck);
	uint8_t card = 51;
	for (int i = 0; i < 7; ++i) {
		check_eq(game.tableau_top[i], i + 1);
		check_eq(game.tableau_up[i], i);
		if (i < 4)
			check_eq(game.foundation_top[i], 0);
		for (int j = i; j < 7; ++j)
			check_eq(game.tableau[j][i], deck.cards[card--]);
	}
	for (int i = 0; i < 24; ++i)
		check_eq(game.stock[23 - i], deck.cards[card--]);
	check_eq(game.stock_count, 24);
	check_eq(game.stock_cur, 24);
	test_end();
}

static bool setup_run_tests(void) {
	printf("\nRunning Setup tests:\n");
	int count = 0;
	int fail = 0;
	run_test(setup_verify_shuffle, &count, &fail);
	run_test(setup_verify_deal, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
